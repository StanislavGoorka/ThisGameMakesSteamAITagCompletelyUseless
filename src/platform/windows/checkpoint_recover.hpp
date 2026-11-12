#pragma once

#include "platform/windows/checkpoint_read.hpp"

namespace closed_circuit::windows
{
enum class CheckpointSource
{
    none,
    main,
    backup
};

struct CheckpointRecoveryResult
{
    std::optional<RunCheckpoint> checkpoint;
    CheckpointSource source;
    DWORD main_error;
    // Empty means backup was not attempted, rather than a successful read.
    std::optional<DWORD> backup_error;
};

// Read-only selection, with the same single-writer/private-directory contract
// as save. Missing (file/path not found) or invalid main permits fallback;
// any other IO failure stops selection and remains visible to the caller.
// Never repair main, overwrite backup, or promote either staging file.
[[nodiscard]] inline CheckpointRecoveryResult
recover_checkpoint_file(const std::filesystem::path &main)
{
    const auto primary = read_checkpoint_file(main);
    if (primary.checkpoint)
        return {primary.checkpoint, CheckpointSource::main, primary.error,
                std::nullopt};
    if (primary.error != ERROR_FILE_NOT_FOUND &&
        primary.error != ERROR_PATH_NOT_FOUND &&
        primary.error != ERROR_INVALID_DATA)
        return {std::nullopt, CheckpointSource::none, primary.error,
                std::nullopt};
    auto backup = main;
    backup += L".bak";
    const auto secondary = read_checkpoint_file(backup);
    return {secondary.checkpoint,
            secondary.checkpoint ? CheckpointSource::backup
                                 : CheckpointSource::none,
            primary.error, secondary.error};
}
} // namespace closed_circuit::windows
