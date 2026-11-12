#pragma once

#include "application/profile_unlock.hpp"
#include "platform/windows/profile_save.hpp"

namespace closed_circuit::windows
{
enum class ProfileFileUnlockResult
{
    unlocked,
    already_unlocked,
    invalid_id,
    read_error,
    write_error
};

// One writer in a private local directory. Always reread the persisted set;
// never replace it with a stale in-memory profile. Only a missing main starts
// a default profile; invalid/inaccessible main must be handled by the caller.
[[nodiscard]] inline ProfileFileUnlockResult
unlock_profile_file(const std::filesystem::path &main, AchievementId id)
{
    if (!is_achievement_id_valid(id))
        return ProfileFileUnlockResult::invalid_id;
    const auto read = read_profile_file(main);
    if (!read.profile && read.error != ERROR_FILE_NOT_FOUND)
        return ProfileFileUnlockResult::read_error;
    auto next = read.profile.value_or(LocalProfile{});
    if (unlock_achievement(next, id) == ProfileUnlockResult::already_unlocked)
        return ProfileFileUnlockResult::already_unlocked;
    return save_profile_file(main, next) ? ProfileFileUnlockResult::unlocked
                                         : ProfileFileUnlockResult::write_error;
}
} // namespace closed_circuit::windows
