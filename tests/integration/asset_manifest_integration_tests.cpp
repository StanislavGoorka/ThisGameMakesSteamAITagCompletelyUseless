#include "../support/checkpoint_files.hpp"
#include "platform/windows/asset_manifest_parse.hpp"
#include "platform/windows/asset_read.hpp"
#include <iostream>
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    winrt::init_apartment();
    const std::filesystem::path root(argv[1]);
    const auto registry_bytes = test::read_bytes(root / L"content/assets.json");
    const std::string json(registry_bytes.begin(), registry_bytes.end());
    const auto manifest = windows::parse_asset_manifest(json);
    if (!manifest || manifest->size() != 3 || find_asset(*manifest, "unknown"))
        return 1;
    test::CheckpointDirectory isolated;
    const std::array ids{"kenney1bit12-monochrome", "kenney1bit12-license",
                         "kenney1bit12-tilesheet-info"};
    for (const auto id : ids)
    {
        const auto *ref = find_asset(*manifest, id);
        if (!ref)
            return 1;
        const auto original = root / ref->path();
        const auto expected = test::read_bytes(original);
        for (unsigned repeat = 0; repeat != 2; ++repeat)
        {
            const auto result = windows::read_asset_file(root, *ref);
            if (result.error != ERROR_SUCCESS || result.bytes != expected)
            {
                std::cerr << id << ": " << result.error << '\n';
                return 1;
            }
        }
        const auto copy = isolated.path / ref->path();
        std::filesystem::create_directories(copy.parent_path());
        test::write_bytes(copy, expected);
        const auto result = windows::read_asset_file(isolated.path, *ref);
        if (result.error != ERROR_SUCCESS || result.bytes != expected)
            return 1;
        auto corrupt = expected;
        corrupt.front() ^= 1;
        test::write_bytes(copy, corrupt);
        const auto broken = windows::read_asset_file(isolated.path, *ref);
        if (broken.bytes || broken.error != ERROR_CRC)
            return 1;
        std::filesystem::remove(copy);
        const auto missing = windows::read_asset_file(isolated.path, *ref);
        if (missing.bytes || missing.error != ERROR_FILE_NOT_FOUND ||
            test::read_bytes(original) != expected)
            return 1;
    }
    auto invalid_path = json;
    const auto path = manifest->front().path();
    invalid_path.replace(invalid_path.find(path), path.size(),
                         "assets/approved/../outside.png");
    if (windows::parse_asset_manifest(invalid_path))
        return 1;
    const auto bad_hash = AssetReference::from_fields(
        ids.front(), path, std::string(64, '0'), manifest->front().bytes());
    const auto mismatch = windows::read_asset_file(root, *bad_hash);
    if (mismatch.bytes || mismatch.error != ERROR_CRC ||
        test::read_bytes(root / L"content/assets.json") != registry_bytes)
        return 1;
    return 0;
}
