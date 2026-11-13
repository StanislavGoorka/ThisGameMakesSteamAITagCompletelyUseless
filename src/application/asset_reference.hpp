#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace closed_circuit
{
inline constexpr std::uint64_t asset_byte_limit = 64ull * 1024 * 1024;
inline constexpr std::size_t asset_count_limit = 256;
namespace asset_detail
{
constexpr char lower(char c) noexcept
{
    return c >= 'A' && c <= 'Z' ? static_cast<char>(c + ('a' - 'A')) : c;
}
constexpr bool alnum(char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}
inline bool valid_id(std::string_view id) noexcept
{
    if (id.empty() || id.size() > 64 || !alnum(id.front()))
        return false;
    for (const auto c : id)
        if (!alnum(c) && c != '_' && c != '-')
            return false;
    return true;
}
inline bool same_path(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size())
        return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (lower(a[i]) != lower(b[i]))
            return false;
    return true;
}
inline bool valid_path(std::string_view path) noexcept
{
    constexpr std::string_view prefix = "assets/approved/";
    if (path.size() > 240 || !path.starts_with(prefix))
        return false;
    path.remove_prefix(prefix.size());
    while (!path.empty())
    {
        const auto slash = path.find('/');
        const auto part = path.substr(0, slash);
        if (part.empty() || part.back() == '.')
            return false;
        for (const auto c : part)
            if (!alnum(lower(c)) && c != '_' && c != '-' && c != '.')
                return false;
        const auto stem = part.substr(0, part.find('.'));
        for (const auto reserved : {"con", "prn", "aux", "nul"})
            if (same_path(stem, reserved))
                return false;
        if (stem.size() == 4 && stem[3] >= '1' && stem[3] <= '9' &&
            (same_path(stem.substr(0, 3), "com") ||
             same_path(stem.substr(0, 3), "lpt")))
            return false;
        if (slash == std::string_view::npos)
            return true;
        path.remove_prefix(slash + 1);
    }
    return false; // Empty relative name or a trailing separator.
}
constexpr int hex(char c) noexcept
{
    return c >= '0' && c <= '9'                 ? c - '0'
           : lower(c) >= 'a' && lower(c) <= 'f' ? lower(c) - 'a' + 10
                                                : -1;
}
} // namespace asset_detail

// Only validated, owning references can be constructed. No file or service
// access.
class AssetReference
{
  public:
    [[nodiscard]] static std::optional<AssetReference>
    from_fields(std::string_view id, std::string_view path,
                std::string_view sha256, std::uint64_t bytes)
    {
        if (!asset_detail::valid_id(id) || !asset_detail::valid_path(path) ||
            sha256.size() != 64 || bytes > asset_byte_limit)
            return std::nullopt;
        std::array<std::uint8_t, 32> digest{};
        for (std::size_t i = 0; i < digest.size(); ++i)
        {
            const auto high = asset_detail::hex(sha256[i * 2]);
            const auto low = asset_detail::hex(sha256[i * 2 + 1]);
            if (high < 0 || low < 0)
                return std::nullopt;
            digest[i] = static_cast<std::uint8_t>(high * 16 + low);
        }
        return AssetReference{std::string(id), std::string(path), digest,
                              bytes};
    }
    [[nodiscard]] const std::string &id() const noexcept
    {
        return id_;
    }
    [[nodiscard]] const std::string &path() const noexcept
    {
        return path_;
    }
    [[nodiscard]] const auto &sha256() const noexcept
    {
        return sha256_;
    }
    [[nodiscard]] std::uint64_t bytes() const noexcept
    {
        return bytes_;
    }
    [[nodiscard]] bool
    operator==(const AssetReference &) const noexcept = default;

  private:
    AssetReference(std::string id, std::string path,
                   std::array<std::uint8_t, 32> digest, std::uint64_t bytes)
        : id_(std::move(id)), path_(std::move(path)), sha256_(digest),
          bytes_(bytes)
    {
    }
    std::string id_, path_;
    std::array<std::uint8_t, 32> sha256_;
    std::uint64_t bytes_;
};
using AssetManifest = std::vector<AssetReference>;

[[nodiscard]] inline bool
is_asset_manifest_valid(std::span<const AssetReference> manifest) noexcept
{
    if (manifest.size() > asset_count_limit)
        return false;
    for (std::size_t i = 0; i < manifest.size(); ++i)
    {
        // A moved-from value may have empty strings; never publish it via
        // lookup.
        if (!asset_detail::valid_id(manifest[i].id()) ||
            !asset_detail::valid_path(manifest[i].path()) ||
            manifest[i].bytes() > asset_byte_limit)
            return false;
        for (std::size_t j = 0; j < i; ++j)
            if (manifest[i].id() == manifest[j].id() ||
                asset_detail::same_path(manifest[i].path(), manifest[j].path()))
                return false;
    }
    return true;
}
[[nodiscard]] inline const AssetReference *
find_asset(std::span<const AssetReference> manifest,
           std::string_view id) noexcept
{
    if (is_asset_manifest_valid(manifest))
        for (const auto &reference : manifest)
            if (reference.id() == id)
                return &reference;
    return nullptr;
}
} // namespace closed_circuit
