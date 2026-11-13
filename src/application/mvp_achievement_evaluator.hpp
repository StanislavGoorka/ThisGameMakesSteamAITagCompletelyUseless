#pragma once

#include "application/event_delivery.hpp"

namespace closed_circuit
{
// One instance per dispatch scope. Candidates are persisted only by the sink.
class MvpAchievementEvaluator final : public AchievementEvaluator
{
  public:
    [[nodiscard]] std::uint16_t
    evaluate(const DomainEvent &event) noexcept override
    {
        if (std::holds_alternative<SectorCompleted>(event.payload))
            return 2;
        return std::holds_alternative<PacketDelivered>(event.payload) ? 1 : 0;
    }
};
} // namespace closed_circuit
