#pragma once
#include "application/asset_reference.hpp"
#include "platform/windows/file_handle.hpp"
#include <bcrypt.h>
#include <filesystem>

namespace closed_circuit::windows
{
struct AssetReadResult
{
    std::optional<std::vector<std::uint8_t>> bytes;
    DWORD error;
};
namespace asset_read_detail
{
inline std::optional<std::wstring> final_path(HANDLE file)
{
    std::wstring path(32768, L'\0');
    const auto size = GetFinalPathNameByHandleW(file, path.data(),
                                                static_cast<DWORD>(path.size()),
                                                FILE_NAME_NORMALIZED);
    if (size == 0 || size >= path.size())
        return std::nullopt;
    path.resize(size);
    return path;
}
struct Sha256Provider
{
    BCRYPT_ALG_HANDLE handle = nullptr;
    ~Sha256Provider()
    {
        if (handle)
            BCryptCloseAlgorithmProvider(handle, 0);
    }
    Sha256Provider() = default;
    Sha256Provider(const Sha256Provider &) = delete;
    Sha256Provider &operator=(const Sha256Provider &) = delete;
};
} // namespace asset_read_detail

// The caller owns a trusted, stable repository root and approved directory.
// No bytes are published until location, exact length and SHA-256 agree.
[[nodiscard]] inline AssetReadResult
read_asset_file(const std::filesystem::path &trusted_root,
                const AssetReference &reference)
{
    if (!is_asset_manifest_valid(std::span{&reference, 1}))
        return {std::nullopt, ERROR_INVALID_DATA};
    const auto approved = trusted_root / L"assets" / L"approved";
    FileHandle root(CreateFileW(
        approved.c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr));
    if (!root)
        return {std::nullopt, GetLastError()};
    BY_HANDLE_FILE_INFORMATION root_info{};
    if (!GetFileInformationByHandle(root.get(), &root_info))
        return {std::nullopt, GetLastError()};
    if (!(root_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
        (root_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
        return {std::nullopt, ERROR_ACCESS_DENIED};
    const auto root_path = asset_read_detail::final_path(root.get());
    if (!root_path)
        return {std::nullopt, ERROR_ACCESS_DENIED};
    auto relative =
        reference.path().substr(std::string_view("assets/approved/").size());
    std::wstring suffix(relative.begin(), relative.end());
    for (auto &c : suffix)
        if (c == L'/')
            c = L'\\';
    const auto expected = *root_path + L"\\" + suffix;
    FileHandle file(CreateFileW(
        expected.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr));
    if (!file)
        return {std::nullopt, GetLastError()};
    BY_HANDLE_FILE_INFORMATION info{};
    if (!GetFileInformationByHandle(file.get(), &info))
        return {std::nullopt, GetLastError()};
    if (GetFileType(file.get()) != FILE_TYPE_DISK ||
        (info.dwFileAttributes &
         (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)))
        return {std::nullopt, ERROR_ACCESS_DENIED};
    const auto actual = asset_read_detail::final_path(file.get());
    if (!actual || CompareStringOrdinal(expected.c_str(), -1, actual->c_str(),
                                        -1, TRUE) != CSTR_EQUAL)
        return {std::nullopt, ERROR_ACCESS_DENIED};
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size))
        return {std::nullopt, GetLastError()};
    if (size.QuadPart != static_cast<LONGLONG>(reference.bytes()))
        return {std::nullopt, ERROR_BAD_LENGTH};
    std::vector<std::uint8_t> bytes(
        static_cast<std::size_t>(reference.bytes()));
    DWORD count = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()),
                  &count, nullptr))
        return {std::nullopt, GetLastError()};
    if (count != bytes.size())
        return {std::nullopt, ERROR_BAD_LENGTH};
    asset_read_detail::Sha256Provider provider;
    std::array<std::uint8_t, 32> digest{};
    if (BCryptOpenAlgorithmProvider(&provider.handle, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) < 0 ||
        BCryptHash(provider.handle, nullptr, 0, bytes.data(),
                   static_cast<ULONG>(bytes.size()), digest.data(),
                   static_cast<ULONG>(digest.size())) < 0)
        return {std::nullopt, ERROR_GEN_FAILURE};
    if (digest != reference.sha256())
        return {std::nullopt, ERROR_CRC};
    return {std::move(bytes), ERROR_SUCCESS};
}
} // namespace closed_circuit::windows
