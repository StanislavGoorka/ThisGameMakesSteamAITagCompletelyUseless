#pragma once

#include "application/local_profile.hpp"
#include "application/profile_format.hpp"
#include <optional>

namespace closed_circuit
{
[[nodiscard]] constexpr std::optional<ProfileBytes>
encode_profile(const LocalProfile &profile) noexcept
{
    if (!is_profile_valid(profile))
        return std::nullopt;
    ProfileBytes bytes{};
    bytes[0] = bytes[1] = 0x43;
    bytes[2] = 0x50;
    bytes[3] = 0x52;
    profile_detail::put_le(bytes, 4, 4, profile_schema_version);
    profile_detail::put_le(bytes, 8, 2, profile.achievement_bits);
    bytes[10] = profile.variant_bits;
    bytes[11] = profile.master_volume;
    bytes[12] = static_cast<std::uint8_t>(profile.muted);
    profile_detail::put_le(bytes, 16, 8,
                           profile_detail::checksum(
                               std::span<const std::uint8_t>{bytes}.first(16)));
    return bytes;
}
} // namespace closed_circuit
