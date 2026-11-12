#pragma once

#include "application/local_profile.hpp"
#include "application/profile_format.hpp"
#include <optional>

namespace closed_circuit
{
namespace profile_detail
{
[[nodiscard]] constexpr std::uint64_t
get_le(std::span<const std::uint8_t> bytes, std::size_t offset,
       std::size_t width) noexcept
{
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < width; ++i)
        value |= static_cast<std::uint64_t>(bytes[offset + i]) << (8 * i);
    return value;
}
} // namespace profile_detail

[[nodiscard]] constexpr std::optional<LocalProfile>
decode_profile(std::span<const std::uint8_t> bytes) noexcept
{
    if (bytes.size() != ProfileBytes{}.size())
        return std::nullopt;
    if (bytes[0] != 0x43 || bytes[1] != 0x43 || bytes[2] != 0x50 ||
        bytes[3] != 0x52 ||
        profile_detail::get_le(bytes, 4, 4) != profile_schema_version ||
        profile_detail::get_le(bytes, 16, 8) !=
            profile_detail::checksum(bytes.first(16)) ||
        bytes[12] > 1 || bytes[13] != 0 || bytes[14] != 0 || bytes[15] != 0)
        return std::nullopt;
    const LocalProfile profile{
        static_cast<std::uint16_t>(profile_detail::get_le(bytes, 8, 2)),
        bytes[10], bytes[11], bytes[12] == 1};
    if (!is_profile_valid(profile))
        return std::nullopt;
    return profile;
}
} // namespace closed_circuit
