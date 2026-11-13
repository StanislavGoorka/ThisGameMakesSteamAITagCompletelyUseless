#pragma once
#include "platform/windows/file_handle.hpp"
#include <filesystem>
#include <memory>
#include <shlobj.h>

namespace closed_circuit::windows
{
struct ProfileLocationResult
{
    std::filesystem::path path;
    HRESULT error;
};

// Read the per-user Windows location, without creating a directory or file.
[[nodiscard]] inline ProfileLocationResult local_profile_base()
{
    PWSTR raw = nullptr;
    const auto result =
        SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &raw);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> owner(
        raw, CoTaskMemFree);
    if (FAILED(result))
        return {{}, result};
    if (!raw || !std::filesystem::path(raw).is_absolute())
        return {{}, E_INVALIDARG};
    return {std::filesystem::path(raw), S_OK};
}

// One writer owns this directory. Never repair, truncate or reset a profile.
// The explicit base keeps tests independent of the user's actual profile.
[[nodiscard]] inline ProfileLocationResult
profile_location_in(const std::filesystem::path &base)
{
    if (!base.is_absolute())
        return {{}, E_INVALIDARG};
    const auto directory = base / L"ClosedCircuit";
    if (!CreateDirectoryW(directory.c_str(), nullptr))
    {
        const auto error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS)
            return {{}, HRESULT_FROM_WIN32(error)};
    }
    const auto attributes = GetFileAttributesW(directory.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES)
        return {{}, HRESULT_FROM_WIN32(GetLastError())};
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT))
        return {{}, HRESULT_FROM_WIN32(ERROR_DIRECTORY)};
    return {directory / L"profile.bin", S_OK};
}

[[nodiscard]] inline ProfileLocationResult local_profile_location()
{
    const auto base = local_profile_base();
    return FAILED(base.error) ? base : profile_location_in(base.path);
}
} // namespace closed_circuit::windows
