#pragma once

#include "application/profile_decode.hpp"
#include "platform/windows/file_handle.hpp"
#include <filesystem>

namespace closed_circuit::windows
{
struct ProfileReadResult
{
    std::optional<LocalProfile> profile;
    DWORD error;
};

// One fixed-size file, read-only. Keep the same handle through size validation
// and read, denying concurrent writes/deletes. No fallback or default profile.
[[nodiscard]] inline ProfileReadResult
read_profile_file(const std::filesystem::path &path)
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
        size.QuadPart != static_cast<LONGLONG>(ProfileBytes{}.size()))
        return {std::nullopt, ERROR_INVALID_DATA};
    ProfileBytes bytes{};
    DWORD count = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &count, nullptr))
        return {std::nullopt, GetLastError()};
    if (count != bytes.size())
        return {std::nullopt, ERROR_INVALID_DATA};
    const auto profile = decode_profile(bytes);
    return {profile,
            static_cast<DWORD>(profile ? ERROR_SUCCESS : ERROR_INVALID_DATA)};
}
} // namespace closed_circuit::windows
