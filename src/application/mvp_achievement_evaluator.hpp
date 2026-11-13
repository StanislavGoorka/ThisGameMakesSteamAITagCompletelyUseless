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
                event.key.sector.value() == 3 && without_overdrive_)
                return 4;
        if (std::holds_alternative<SectorCompleted>(event.payload))
            return 2;
        if (const auto *delivery = std::get_if<PacketDelivered>(&event.payload))
        {
            const bool changed = reconfigured_[delivery->target.value()];
            reconfigured_.reset(delivery->target.value());
            return changed ? 9 : 1;
        }
        return 0;
    }

  private:
    bool without_overdrive_;
    bool first_event_ = true;
    std::optional<SectorNumber> tracked_sector_;
    std::bitset<NodeId::max_value + 1> reconfigured_;
};
} // namespace closed_circuit
