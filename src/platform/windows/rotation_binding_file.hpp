#pragma once
#include "application/rotation_binding_store.hpp"
#include "platform/windows/file_handle.hpp"
#include <filesystem>

namespace closed_circuit::windows
{
struct RotationBindingReadResult
{
    std::optional<RotationBinding> binding;
    DWORD error;
};
[[nodiscard]] inline RotationBindingReadResult
read_rotation_binding_file(const std::filesystem::path &path)
{
    if (!path.is_absolute())
        return {std::nullopt, ERROR_INVALID_NAME};
    FileHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                                nullptr, OPEN_EXISTING,
                                FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    if (!file)
        return {std::nullopt, GetLastError()};
    BY_HANDLE_FILE_INFORMATION info{};
    LARGE_INTEGER size{};
    if (!GetFileInformationByHandle(file.get(), &info) ||
        !GetFileSizeEx(file.get(), &size))
        return {std::nullopt, GetLastError()};
    if (GetFileType(file.get()) != FILE_TYPE_DISK || size.QuadPart != 16 ||
        (info.dwFileAttributes &
         (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY)))
        return {std::nullopt, ERROR_INVALID_DATA};
    RotationBindingBytes bytes{};
    DWORD count = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &count, nullptr))
        return {std::nullopt, GetLastError()};
    const auto value =
        count == bytes.size() ? decode_rotation_binding(bytes) : std::nullopt;
    return {value,
            static_cast<DWORD>(value ? ERROR_SUCCESS : ERROR_INVALID_DATA)};
}

// Private directory, one writer. Never replace a corrupt or inaccessible main.
class LocalRotationBindingStore final : public RotationBindingStore
{
  public:
    explicit LocalRotationBindingStore(std::filesystem::path path)
        : path_(std::move(path))
    {
    }
    [[nodiscard]] bool save(RotationBinding binding) override
    {
        const auto previous = read_rotation_binding_file(path_);
        if (!previous.binding && previous.error != ERROR_FILE_NOT_FOUND)
            return false;
        if (previous.binding == binding)
            return true;
        auto staging = path_;
        staging += L".tmp";
        FileHandle file(CreateFileW(
            staging.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr));
        if (!file)
            return false;
        const auto bytes = encode_rotation_binding(binding);
        DWORD count = 0;
        if (!WriteFile(file.get(), bytes.data(),
                       static_cast<DWORD>(bytes.size()), &count, nullptr) ||
            count != bytes.size() || !FlushFileBuffers(file.get()) ||
            !file.close())
            return false;
        if (read_rotation_binding_file(staging).binding != binding)
            return false;
        return MoveFileExW(staging.c_str(), path_.c_str(),
                           MOVEFILE_REPLACE_EXISTING |
                               MOVEFILE_WRITE_THROUGH) != 0;
    }

  private:
    std::filesystem::path path_;
};
} // namespace closed_circuit::windows
