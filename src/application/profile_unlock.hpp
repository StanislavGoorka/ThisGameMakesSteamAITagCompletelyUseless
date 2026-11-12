#pragma once

#include "application/local_profile.hpp"

namespace closed_circuit
{
enum class ProfileUnlockResult
{
    unlocked,
    already_unlocked,
    invalid
};

[[nodiscard]] constexpr bool is_achievement_id_valid(AchievementId id) noexcept
{
    return static_cast<unsigned>(id) >= 1 && static_cast<unsigned>(id) <= 5;
}

[[nodiscard]] constexpr bool
is_achievement_unlocked(const LocalProfile &profile, AchievementId id) noexcept
{
    return is_profile_valid(profile) && is_achievement_id_valid(id) &&
           (profile.achievement_bits &
            (1u << (static_cast<unsigned>(id) - 1))) != 0;
}

// Only the achievement bit changes. Validation always precedes bit shifting.
[[nodiscard]] constexpr ProfileUnlockResult
unlock_achievement(LocalProfile &profile, AchievementId id) noexcept
{
    if (!is_profile_valid(profile) || !is_achievement_id_valid(id))
        return ProfileUnlockResult::invalid;
    if (is_achievement_unlocked(profile, id))
        return ProfileUnlockResult::already_unlocked;
    profile.achievement_bits |=
        static_cast<std::uint16_t>(1u << (static_cast<unsigned>(id) - 1));
    return ProfileUnlockResult::unlocked;
}
} // namespace closed_circuit
