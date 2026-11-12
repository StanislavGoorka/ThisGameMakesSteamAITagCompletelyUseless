#pragma once

#include "application/checkpoint_decode.hpp"
#include "platform/windows/file_handle.hpp"
#include <filesystem>

namespace closed_circuit::windows
{
struct CheckpointReadResult
{
    std::optional<RunCheckpoint> checkpoint;
    DWORD error;
};

// A single file only: no fallback and no writes. Deny concurrent writes/deletes
// while checking the size and reading this same handle.
[[nodiscard]] inline CheckpointReadResult
read_checkpoint_file(const std::filesystem::path &path)
{
    FileHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                nullptr));
    if (!file)
        return {std::nullopt, GetLastError()};
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size))
        return {std::nullopt, GetLastError()};
    if (GetFileType(file.get()) != FILE_TYPE_DISK ||
        size.QuadPart != static_cast<LONGLONG>(CheckpointBytes{}.size()))
        return {std::nullopt, ERROR_INVALID_DATA};
    CheckpointBytes bytes{};
    DWORD count = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &count, nullptr))
        return {std::nullopt, GetLastError()};
    if (count != bytes.size())
        return {std::nullopt, ERROR_INVALID_DATA};
    const auto checkpoint = decode_checkpoint(bytes);
    return {checkpoint, static_cast<DWORD>(checkpoint ? ERROR_SUCCESS
                                                      : ERROR_INVALID_DATA)};
}
} // namespace closed_circuit::windows
