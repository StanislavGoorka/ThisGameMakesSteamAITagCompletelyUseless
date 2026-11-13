#pragma once

#include "application/event_delivery.hpp"

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
        if (first_event_)
        {
            without_overdrive_ =
                without_overdrive_ && event.key.sector.value() == 1 &&
                event.key.turn == TurnIndex{1} && event.key.ordinal == 0 &&
                std::holds_alternative<ActionCommitted>(event.payload);
            first_event_ = false;
        }
        if (const auto *action = std::get_if<ActionCommitted>(&event.payload))
            if (std::holds_alternative<OverdriveCommand>(action->command))
                without_overdrive_ = false;
        if (const auto *end = std::get_if<RunEnded>(&event.payload))
            if (end->reason == RunEndReason::won &&
                event.key.sector.value() == 3 && without_overdrive_)
                return 4;
        if (std::holds_alternative<SectorCompleted>(event.payload))
            return 2;
        return std::holds_alternative<PacketDelivered>(event.payload) ? 1 : 0;
    }

  private:
    bool without_overdrive_;
    bool first_event_ = true;
};
} // namespace closed_circuit
