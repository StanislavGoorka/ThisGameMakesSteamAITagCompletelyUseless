#pragma once

#include "application/achievement_sink.hpp"
#include "platform/windows/profile_unlock.hpp"
#include <utility>

namespace closed_circuit::windows
{
class LocalAchievementSink final : public AchievementSink
{
  public:
    explicit LocalAchievementSink(std::filesystem::path profile_path)
        : profile_path_(std::move(profile_path))
    {
    }

    [[nodiscard]] AchievementSinkResult unlock(AchievementId id) override
    {
        switch (unlock_profile_file(profile_path_, id))
        {
        case ProfileFileUnlockResult::unlocked:
            return AchievementSinkResult::unlocked;
        case ProfileFileUnlockResult::already_unlocked:
            return AchievementSinkResult::already_unlocked;
        case ProfileFileUnlockResult::invalid_id:
            return AchievementSinkResult::invalid;
        case ProfileFileUnlockResult::read_error:
        case ProfileFileUnlockResult::write_error:
            return AchievementSinkResult::unavailable;
        }
        return AchievementSinkResult::unavailable;
    }

  private:
    std::filesystem::path profile_path_;
};
} // namespace closed_circuit::windows
