#pragma once

#include "application/checkpoint_encode.hpp"
#include "platform/windows/file_handle.hpp"
#include <filesystem>

namespace closed_circuit::windows
{
// Only a private staging path, never main or backup. On failure staging may
// contain partial data; retry truncates it. The caller owns one writer per
// slot.
[[nodiscard]] inline bool
stage_checkpoint_file(const std::filesystem::path &staging,
                      const RunCheckpoint &checkpoint)
{
    const auto bytes = encode_checkpoint(checkpoint);
    if (!bytes)
        return false;
    FileHandle file(
        CreateFileW(staging.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
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
} // namespace closed_circuit::windows
