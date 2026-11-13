#include "../support/checkpoint_files.hpp"
#include "platform/windows/asset_read.hpp"
#include <iostream>
#include <winioctl.h>
using namespace closed_circuit;

// A directory junction needs no symbolic-link privilege. C: supports reparse
// points; the repository's D: volume does not.
bool junction(const std::filesystem::path &link,
              const std::filesystem::path &target)
{
    std::filesystem::create_directory(link);
    windows::FileHandle handle(CreateFileW(
        link.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr));
    if (!handle)
        return false;
    struct Buffer
    {
        DWORD tag = IO_REPARSE_TAG_MOUNT_POINT;
        WORD length = 0, reserved = 0, offset = 0, name_length = 0;
        WORD print_offset = 0, print_length = 0;
        wchar_t name[32768]{};
    } buffer;
    const auto name = L"\\??\\" + target.wstring();
    std::copy(name.begin(), name.end(), buffer.name);
    buffer.name_length = static_cast<WORD>(name.size() * sizeof(wchar_t));
    buffer.print_offset = buffer.name_length + sizeof(wchar_t);
    buffer.length = buffer.print_offset + sizeof(wchar_t) + 8;
    DWORD count = 0;
    return DeviceIoControl(handle.get(), FSCTL_SET_REPARSE_POINT, &buffer,
                           buffer.length + 8, nullptr, 0, &count, nullptr) != 0;
}

int main()
{
    test::CheckpointDirectory directory(std::filesystem::temp_directory_path());
    const auto root = directory.path / L"baza \u017c\u00f3\u0142\u0107";
    const auto approved = root / L"assets" / L"approved";
    std::filesystem::create_directories(approved);
    const auto file = approved / L"data.bin";
    const std::vector<std::uint8_t> abc{97, 98, 99};
    constexpr auto hash =
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
    const auto reference = *AssetReference::from_fields(
        "test", "assets/approved/data.bin", hash, 3);
    auto check = [&](const AssetReference &ref, DWORD expected)
    {
        const auto result = windows::read_asset_file(root, ref);
        if (result.error != expected ||
            (expected == ERROR_SUCCESS ? result.bytes != abc
                                       : result.bytes.has_value()))
        {
            std::cerr << "asset error: " << result.error << " expected "
                      << expected << '\n';
            return false;
        }
        return true;
    };
    if (!check(reference, ERROR_FILE_NOT_FOUND))
        return 1;
    test::write_bytes(file, abc);
    if (!check(reference, ERROR_SUCCESS) || !check(reference, ERROR_SUCCESS))
        return 1;
    for (const auto size : {0u, 2u, 4u, 67108864u})
        if (!check(*AssetReference::from_fields("test", reference.path(), hash,
                                                size),
                   ERROR_BAD_LENGTH))
            return 1;
    if (!check(*AssetReference::from_fields("test", reference.path(),
                                            std::string(64, '0'), 3),
               ERROR_CRC))
        return 1;
    {
        windows::FileHandle lock(CreateFileW(
            file.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!lock || !check(reference, ERROR_SHARING_VIOLATION))
            return 1;
    }
    auto moved = reference;
    const auto owner = std::move(moved);
    if (!check(moved, ERROR_INVALID_DATA) || test::read_bytes(file) != abc)
        return 1;
    std::filesystem::create_directory(approved / L"folder");
    if (!check(*AssetReference::from_fields("dir", "assets/approved/folder",
                                            hash, 3),
               ERROR_ACCESS_DENIED))
        return 1;
    const auto empty = approved / L"empty.bin";
    test::write_bytes(empty, {});
    const auto empty_result = windows::read_asset_file(
        root,
        *AssetReference::from_fields(
            "empty", "assets/approved/empty.bin",
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            0));
    if (!empty_result.bytes || !empty_result.bytes->empty() ||
        empty_result.error != ERROR_SUCCESS)
        return 1;
    // Only isolated nonvisual bytes are linked. Never touch repo approved.
    const auto outside = root / L"outside";
    std::filesystem::create_directory(outside);
    test::write_bytes(outside / L"data.bin", abc);
    if (!junction(approved / L"link.bin", outside) ||
        !junction(approved / L"escape", outside))
    {
        std::cerr << "Cannot create reparse fixture: " << GetLastError()
                  << '\n';
        return 1;
    }
    for (const auto path :
         {"assets/approved/link.bin", "assets/approved/escape/data.bin"})
        if (!check(*AssetReference::from_fields("link", path, hash, 3),
                   ERROR_ACCESS_DENIED))
            return 1;
    return 0;
}
