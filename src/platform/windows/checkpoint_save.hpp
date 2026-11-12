#pragma once

#include "platform/windows/checkpoint_read.hpp"
#include "platform/windows/checkpoint_stage.hpp"

namespace closed_circuit::windows
{
// Same-directory replacement only; never fall back to copy/delete. One writer
// owns these paths, including between validation and rename.
[[nodiscard]] inline bool
publish_checkpoint_file(const std::filesystem::path &staging,
                        const std::filesystem::path &destination)
{
    if (staging == destination ||
        staging.parent_path() != destination.parent_path() ||
        !read_checkpoint_file(staging).checkpoint)
        return false;
    return MoveFileExW(staging.c_str(), destination.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

// Private, local directory; a single writer per main. Failed staging is left
// for retry. Backup is published before main, so main never has a missing-name
// window. Two renames are not a transaction across both names.
[[nodiscard]] inline bool
save_checkpoint_file(const std::filesystem::path &main,
                     const RunCheckpoint &checkpoint)
{
    if (!is_checkpoint_valid(checkpoint))
        return false;
    const auto previous = read_checkpoint_file(main);
    if (!previous.checkpoint && previous.error != ERROR_FILE_NOT_FOUND)
        return false;
    auto staging = main;
    staging += L".tmp";
    if (!stage_checkpoint_file(staging, checkpoint))
        return false;
    if (previous.checkpoint)
    {
        auto backup = main;
        backup += L".bak";
        auto backup_staging = backup;
        backup_staging += L".tmp";
        if (!stage_checkpoint_file(backup_staging, *previous.checkpoint) ||
            !publish_checkpoint_file(backup_staging, backup))
            return false;
    }
    return publish_checkpoint_file(staging, main);
}
} // namespace closed_circuit::windows
