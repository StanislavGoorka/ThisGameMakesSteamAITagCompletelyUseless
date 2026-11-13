#include "../support/checkpoint_files.hpp"
#include "platform/windows/asset_manifest_parse.hpp"
#include "platform/windows/png_read.hpp"
#include <iostream>
using namespace closed_circuit;

std::string digest(std::vector<std::uint8_t> bytes)
{
    windows::asset_read_detail::Sha256Provider provider;
    std::array<std::uint8_t, 32> hash{};
    if (BCryptOpenAlgorithmProvider(&provider.handle, BCRYPT_SHA256_ALGORITHM,
                                    nullptr, 0) < 0 ||
        BCryptHash(provider.handle, nullptr, 0, bytes.data(),
                   static_cast<ULONG>(bytes.size()), hash.data(),
                   static_cast<ULONG>(hash.size())) < 0)
        return {};
    constexpr char hex[] = "0123456789abcdef";
    std::string result;
    for (auto byte : hash)
    {
        result += hex[byte >> 4];
        result += hex[byte & 15];
    }
    return result;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    const std::filesystem::path root(argv[1]);
    const auto reference = *AssetReference::from_fields(
        "kenney1bit12-monochrome",
        "assets/approved/kenney-1bit-1.2/Tilesheet/monochrome-transparent.png",
        "888B96CEF0777CE03E9A9F3EB2C7AFC02B2EED9C8F4E9E36E9871F8B624D2D00",
        19451);
    const auto uninitialized = windows::read_png_asset(root, reference);
    if (uninitialized.image || uninitialized.error != CO_E_NOTINITIALIZED)
        return 2;
    winrt::init_apartment();
    const auto registry = test::read_bytes(root / L"content/assets.json");
    const auto manifest = windows::parse_asset_manifest(
        std::string(registry.begin(), registry.end()));
    if (!manifest)
        return 3;
    const auto actual = find_asset(*manifest, reference.id());
    if (!actual || *actual != reference)
        return 4;
    const auto original = test::read_bytes(root / reference.path());
    auto decoded = windows::read_png_asset(root, *actual);
    if (!decoded.image || decoded.error != S_OK)
    {
        std::cerr << "WIC failed: " << decoded.error << '\n';
        return 5;
    }
    const auto expected = *decoded.image;
    // Independently decoded with GDI+ Format32bppPArgb (see PNG_DECODING.md).
    if (expected.width != 832 || expected.height != 373 ||
        expected.stride != 3328 || expected.pixels.size() != 1241344 ||
        digest(expected.pixels) !=
            "50fe0ea51e1fb5c947b3d40d7caff4d08800a897323e5b6937563300adeb850e")
        return 6;
    test::CheckpointDirectory directory;
    const auto isolated = directory.path / L"png \u017c\u00f3\u0142\u0107";
    const auto copy = isolated / reference.path();
    std::filesystem::create_directories(copy.parent_path());
    test::write_bytes(copy, original);
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        const auto result = windows::read_png_asset(isolated, reference);
        if (result.error != S_OK || result.image != expected)
            return 7;
    }
    decoded.image->pixels.front() ^= 1;
    if (windows::read_png_asset(root, reference).image != expected)
        return 8;
    auto corrupt = original;
    corrupt.front() ^= 1;
    test::write_bytes(copy, corrupt);
    const auto mismatch = windows::read_png_asset(isolated, reference);
    if (mismatch.image || mismatch.error != HRESULT_FROM_WIN32(ERROR_CRC))
        return 9;
    // A matching digest cannot make malformed PNG data decodable.
    const auto malformed = *AssetReference::from_fields(
        "corrupt", reference.path(), digest(corrupt), corrupt.size());
    const auto bad_png = windows::read_png_asset(isolated, malformed);
    if (bad_png.image || !FAILED(bad_png.error))
        return 10;
    std::filesystem::remove(copy);
    const auto missing = windows::read_png_asset(isolated, reference);
    if (missing.image ||
        missing.error != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        return 11;
    test::write_bytes(copy, original);
    if (windows::read_png_asset(isolated, reference).image != expected)
        return 12;
    const auto *license = find_asset(*manifest, "kenney1bit12-license");
    if (!license)
        return 13;
    const auto non_png = windows::read_png_asset(root, *license);
    if (non_png.image || !FAILED(non_png.error) ||
        test::read_bytes(root / reference.path()) != original ||
        test::read_bytes(root / L"content/assets.json") != registry)
        return 14;
    return 0;
}
