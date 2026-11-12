#pragma once

#include "application/profile_encode.hpp"
#include "platform/windows/profile_read.hpp"

namespace closed_circuit::windows
{
// Only a private staging name. Failure may leave partial data; retry truncates.
[[nodiscard]] inline bool stage_profile_file(const std::filesystem::path &path,
                                             const LocalProfile &profile)
{
    const auto bytes = encode_profile(profile);
    if (!bytes)
        return false;
    FileHandle file(
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr));
    if (!file)
        return false;
    DWORD count = 0;
    if (!WriteFile(file.get(), bytes->data(), static_cast<DWORD>(bytes->size()),
                   &count, nullptr) ||
        count != bytes->size() || !FlushFileBuffers(file.get()))
        return false;
    return file.close();
}

[[nodiscard]] inline bool
publish_profile_file(const std::filesystem::path &staging,
                     const std::filesystem::path &destination)
{
    if (staging == destination ||
        staging.parent_path() != destination.parent_path() ||
        !read_profile_file(staging).profile)
        return false;
    return MoveFileExW(staging.c_str(), destination.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

// Private local directory and one writer per slot, including validation through
// rename. Never move main away: publish its validated copy to backup first,
// then replace main. These two renames are not a transaction across both names.
// Invalid or inaccessible main is refused; no silent reset or backup repair.
[[nodiscard]] inline bool save_profile_file(const std::filesystem::path &main,
                                            const LocalProfile &profile)
{
    if (!is_profile_valid(profile))
        return false;
    const auto previous = read_profile_file(main);
    if (!previous.profile && previous.error != ERROR_FILE_NOT_FOUND)
        return false;
    auto staging = main;
    staging += L".tmp";
    if (!stage_profile_file(staging, profile))
        return false;
    if (previous.profile)
    {
        auto backup = main;
        backup += L".bak";
        auto backup_staging = backup;
        backup_staging += L".tmp";
        if (!stage_profile_file(backup_staging, *previous.profile) ||
            !publish_profile_file(backup_staging, backup))
            return false;
    }
    return publish_profile_file(staging, main);
}
} // namespace closed_circuit::windows
