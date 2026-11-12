#pragma once

#include "application/local_profile.hpp"

namespace closed_circuit
{
enum class AchievementSinkResult
{
    unlocked,
    already_unlocked,
    invalid,
    unavailable
};

// Serial, idempotent requests. Success means the set is durably updated or
// already contains this ID. Failure may be retried with the same ID.
class AchievementSink
{
  public:
    virtual ~AchievementSink() = default;
    [[nodiscard]] virtual AchievementSinkResult unlock(AchievementId id) = 0;
};
} // namespace closed_circuit
