#pragma once

#include "application/session_resume.hpp"
#include "platform/windows/checkpoint_recover.hpp"
#include <algorithm>

namespace closed_circuit::windows
{
struct CheckpointResume
{
    std::optional<RunState> run;
    ResumeStatus status;
};

[[nodiscard]] constexpr bool is_missing_file(DWORD error) noexcept
{
    return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
}

// Both names missing means no saved run. Any read failure other than missing
// or invalid data may be transient, so it outranks corruption.
[[nodiscard]] inline CheckpointResume
classify_checkpoint_recovery(const CheckpointRecoveryResult &recovery)
{
    if (recovery.checkpoint)
    {
        auto run = restore_checkpoint(*recovery.checkpoint);
        return {std::move(run),
                run ? ResumeStatus::resumed : ResumeStatus::corrupt};
    }
    if (!recovery.backup_error)
        return {std::nullopt, ResumeStatus::unavailable};
    const auto errors = {recovery.main_error, *recovery.backup_error};
    if (std::ranges::all_of(errors, is_missing_file))
        return {std::nullopt, ResumeStatus::none};
    if (std::ranges::all_of(
            errors, [](DWORD error)
            { return is_missing_file(error) || error == ERROR_INVALID_DATA; }))
        return {std::nullopt, ResumeStatus::corrupt};
    return {std::nullopt, ResumeStatus::unavailable};
}

// Read-only: never repairs, deletes or promotes any checkpoint file.
[[nodiscard]] inline CheckpointResume
resume_checkpoint_file(const std::filesystem::path &main)
{
    return classify_checkpoint_recovery(recover_checkpoint_file(main));
}
} // namespace closed_circuit::windows
