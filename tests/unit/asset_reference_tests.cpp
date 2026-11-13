#include "application/asset_reference.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
constexpr std::string_view hash =
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";
constexpr std::string_view path = "assets/approved/pack/Item.png";
bool check(bool value, const char *message)
{
    if (!value)
        std::cerr << message << '\n';
    return value;
}
bool fields()
{
    auto id = std::string("asset-1");
    const auto reference = AssetReference::from_fields(id, path, hash, 19451);
    id.clear();
    if (!reference || reference->id() != "asset-1" ||
        reference->path() != path || reference->bytes() != 19451)
        return false;
    for (std::size_t i = 0; i < 32; ++i)
        if (reference->sha256()[i] != i)
            return false;
    const auto upper = AssetReference::from_fields(
        "asset-1", path,
        "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F",
        19451);
    if (upper != reference)
        return false;
    auto copy = reference;
    copy =
        AssetReference::from_fields("other", "assets/approved/other", hash, 0);
    if (!copy || copy->id() == reference->id() || reference->bytes() != 19451)
        return false;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const char c = static_cast<char>(raw);
        const bool alnum = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
        if (AssetReference::from_fields(std::string(1, c), path, hash, 0)
                    .has_value() != alnum ||
            AssetReference::from_fields(std::string("a") + c, path, hash, 0)
                    .has_value() != (alnum || c == '_' || c == '-'))
            return false;
        for (std::size_t index = 0; index < 64; ++index)
        {
            auto corrupted = std::string(hash);
            corrupted[index] = c;
            const bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
                             (c >= 'A' && c <= 'F');
            if (AssetReference::from_fields("a", path, corrupted, 0)
                    .has_value() != hex)
                return false;
        }
    }
    for (const auto bad : {"", "_a", "-a", "A", "a/b", "a b"})
        if (AssetReference::from_fields(bad, path, hash, 0))
            return false;
    const std::string max_path = "assets/approved/" + std::string(224, 'a');
    return AssetReference::from_fields(std::string(64, 'a'), max_path, hash,
                                       asset_byte_limit) &&
           !AssetReference::from_fields(std::string(65, 'a'), path, hash, 0) &&
           !AssetReference::from_fields("a", max_path + "a", hash, 0) &&
           !AssetReference::from_fields("a", path, hash,
                                        asset_byte_limit + 1) &&
           !AssetReference::from_fields("a", path, hash, UINT64_MAX) &&
           !AssetReference::from_fields("a", path, hash.substr(1), 0) &&
           !AssetReference::from_fields("a", path, std::string(hash) + "0", 0);
}
bool paths()
{
    for (const auto bad : {"",
                           "assets/approved/",
                           "Assets/approved/a",
                           "assets/approvedx/a",
                           "assets/approved/a/",
                           "assets/approved/a//b",
                           "assets/approved/.",
                           "assets/approved/../a",
                           "assets/approved/a/./b",
                           "assets/approved/a./b",
                           "assets/approved/a ",
                           "assets/approved/a:stream",
                           "assets/approved/a\\b",
                           "assets/approved/CON",
                           "assets/approved/p/nuL.txt",
                           "assets/approved/PRN.a",
                           "assets/approved/AuX.png",
                           "assets/approved/CONIN$",
                           "C:/assets/approved/a",
                           "/assets/approved/a",
                           "//server/assets/approved/a",
                           "assets/approved/a?",
                           "assets/approved/a%20b",
                           "assets/approved/\xc5\x81.png"})
        if (AssetReference::from_fields("a", bad, hash, 0))
            return false;
    for (unsigned n = 1; n <= 9; ++n)
        for (const auto stem : {"CoM", "LpT"})
            if (AssetReference::from_fields("a",
                                            "assets/approved/" +
                                                std::string(stem) +
                                                std::to_string(n) + ".txt",
                                            hash, 0))
                return false;
    const std::string nul_path("assets/approved/a\0.png", 22);
    return !AssetReference::from_fields("a", nul_path, hash, 0) &&
           AssetReference::from_fields("a", "assets/approved/.hidden/COM0.txt",
                                       hash, 0) &&
           AssetReference::from_fields("a", "assets/approved/com10/lpt10", hash,
                                       0);
}
bool manifests()
{
    auto original = *AssetReference::from_fields("a", path, hash, 0);
    const auto moved = std::move(original);
    if (moved.id() != "a" || moved.path() != path)
        return false;
    if ((original.id().empty() || original.path().empty()) &&
        (is_asset_manifest_valid(std::array{original}) ||
         find_asset(std::array{original}, "")))
        return false;
    AssetManifest manifest;
    if (!is_asset_manifest_valid(manifest) || find_asset(manifest, "unknown"))
        return false;
    for (unsigned i = 0; i < asset_count_limit; ++i)
    {
        const auto id = "a" + std::to_string(i);
        manifest.push_back(
            *AssetReference::from_fields(id, "assets/approved/" + id, hash, i));
    }
    if (!is_asset_manifest_valid(manifest) || find_asset(manifest, "A0") ||
        find_asset(manifest, "missing") || find_asset(manifest, "../a0") ||
        find_asset(manifest, "a0") != &manifest.front())
        return false;
    auto copy = manifest;
    copy[1] =
        *AssetReference::from_fields("a0", "assets/approved/distinct", hash, 1);
    if (is_asset_manifest_valid(copy) || find_asset(copy, "a0") ||
        !is_asset_manifest_valid(manifest))
        return false;
    copy[1] =
        *AssetReference::from_fields("other", "assets/approved/A0", hash, 1);
    if (is_asset_manifest_valid(copy) || find_asset(copy, "other"))
        return false;
    manifest.push_back(
        *AssetReference::from_fields("last", "assets/approved/last", hash, 0));
    return !is_asset_manifest_valid(manifest) && !find_asset(manifest, "a0");
}
} // namespace
int main()
{
    return check(fields(), "reference fields/digest/limits/copies") &&
                   check(paths(), "canonical paths and Windows devices") &&
                   check(manifests(), "manifest uniqueness/bounds/lookup")
               ? 0
               : 1;
}
