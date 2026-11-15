#pragma once

#include "application/event_delivery.hpp"
#include <bitset>

namespace closed_circuit
{
enum class RunHistoryOrigin
{
    fresh_run,
    restored_checkpoint
};
// One instance per dispatch scope. Candidates are persisted only by the sink.
class MvpAchievementEvaluator final : public AchievementEvaluator
{
  public:
    explicit MvpAchievementEvaluator(
        RunHistoryOrigin origin =
            RunHistoryOrigin::restored_checkpoint) noexcept
        : without_overdrive_(origin == RunHistoryOrigin::fresh_run)
    {
    }
    [[nodiscard]] std::uint16_t
    evaluate(const DomainEvent &event) noexcept override
    {
        if (tracked_sector_ != event.key.sector)
        {
            reconfigured_.reset();
            tracked_sector_ = event.key.sector;
        }
        if (first_event_)
        {
            without_overdrive_ =
                without_overdrive_ && event.key.sector.value() == 1 &&
                event.key.turn == TurnIndex{1} && event.key.ordinal == 0 &&
                std::holds_alternative<ActionCommitted>(event.payload);
            first_event_ = false;
        }
        if (const auto *action = std::get_if<ActionCommitted>(&event.payload))
        {
            if (std::holds_alternative<OverdriveCommand>(action->command))
                without_overdrive_ = false;
            if (action->reconfigured_packet_target)
                reconfigured_.set(action->reconfigured_packet_target->value());
        }
        if (const auto *end = std::get_if<RunEnded>(&event.payload))
            if (end->reason == RunEndReason::won &&
                event.key.sector.value() == sector_count)
            {
                // won_preinstalled_range / won_charged_debt: the run started
                // from ID2 / ID3.
                const std::uint16_t config =
                    end->start_config == StartConfigId::preinstalled_range
                        ? 0x40
                    : end->start_config == StartConfigId::charged_debt ? 0x80
                                                                       : 0;
                // won_short_turn_limit: the run kept modifier bit 1.
                const std::uint16_t short_turns =
                    end->difficulty.contains(
                        DifficultyModifier::short_turn_limit)
                        ? 0x100
                        : 0;
                // won_lean_delivery: the run kept modifier bit 2.
                const std::uint16_t lean =
                    end->difficulty.contains(DifficultyModifier::lean_delivery)
                        ? 0x200
                        : 0;
                // won_heavy_overdrive_debt: the run kept modifier bit 4.
                const std::uint16_t heavy =
                    end->difficulty.contains(
                        DifficultyModifier::heavy_overdrive_debt)
                        ? 0x400
                        : 0;
                // won_all_modifiers: the run kept every implemented bit.
                const std::uint16_t all =
                    end->difficulty.bits() == implemented_difficulty_bits()
                        ? 0x800
                        : 0;
                return static_cast<std::uint16_t>(
                    (without_overdrive_ ? 20 : 16) | config | short_turns |
                    lean | heavy | all);
            }
        // swift_sector: completion within the first 16 turns of any sector.
        if (std::holds_alternative<SectorCompleted>(event.payload))
            return event.key.turn <= swift_sector_turns ? 0x22 : 2;
        if (const auto *delivery = std::get_if<PacketDelivered>(&event.payload))
        {
            const bool changed = reconfigured_[delivery->target.value()];
            reconfigured_.reset(delivery->target.value());
            return changed ? 9 : 1;
        }
        return 0;
    }

  private:
    static constexpr TurnIndex swift_sector_turns{16};
    bool without_overdrive_;
    bool first_event_ = true;
    std::optional<SectorNumber> tracked_sector_;
    std::bitset<NodeId::max_value + 1> reconfigured_;
};
} // namespace closed_circuit
