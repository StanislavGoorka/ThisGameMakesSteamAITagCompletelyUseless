#pragma once

#include "application/checkpoint_store.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include <utility>

namespace closed_circuit::windows
{
// Backup first: a crash between the deletes leaves only the newest boundary,
// never an older backup that recovery would otherwise resume. Staging files
// are left alone; recovery never reads them and the next save replaces them.
[[nodiscard]] inline bool
clear_checkpoint_files(const std::filesystem::path &main)
{
    auto backup = main;
    backup += L".bak";
    for (const auto &path : {backup, main})
        if (!DeleteFileW(path.c_str()))
        {
            const auto error = GetLastError();
            if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND)
                return false;
        }
    return true;
}

class LocalCheckpointStore final : public CheckpointStore
{
  public:
    explicit LocalCheckpointStore(std::filesystem::path main)
        : main_(std::move(main))
    {
    }
    [[nodiscard]] bool save(const RunCheckpoint &checkpoint) override
    {
        return save_checkpoint_file(main_, checkpoint);
    }
    [[nodiscard]] bool clear() override
    {
        return clear_checkpoint_files(main_);
    }

  private:
    std::filesystem::path main_;
};
} // namespace closed_circuit::windows
