#pragma once
#include "application/achievement_sink.hpp"
#include "application/event_delivery.hpp"
#include "application/profile_unlock.hpp"
#include <stdexcept>
#include <vector>

namespace closed_circuit::test
{
class CountingEvaluator final : public AchievementEvaluator
{
  public:
    [[nodiscard]] std::uint16_t evaluate(const DomainEvent &) noexcept override
    {
        ++calls;
        return candidates;
    }
    unsigned calls = 0;
    std::uint16_t candidates = 0;
};
class RecordingSink final : public AchievementSink
{
  public:
    [[nodiscard]] AchievementSinkResult unlock(AchievementId id) override
    {
        calls.push_back(id);
        if (fail_id == id)
            return failure;
        const auto result = unlock_achievement(profile, id);
        if (throw_after_id == id)
        {
            throw_after_id.reset();
            throw std::runtime_error("Synthetic failure after effect");
        }
        return result == ProfileUnlockResult::unlocked
                   ? AchievementSinkResult::unlocked
               : result == ProfileUnlockResult::already_unlocked
                   ? AchievementSinkResult::already_unlocked
                   : AchievementSinkResult::invalid;
    }
    LocalProfile profile;
    std::vector<AchievementId> calls;
    std::optional<AchievementId> fail_id;
    std::optional<AchievementId> throw_after_id;
    AchievementSinkResult failure = AchievementSinkResult::unavailable;
};
} // namespace closed_circuit::test
