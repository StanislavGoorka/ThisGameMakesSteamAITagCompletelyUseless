#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace closed_circuit
{
inline constexpr std::uint32_t profile_schema_version = 1;
using ProfileBytes = std::array<std::uint8_t, 24>;

namespace profile_detail
{
constexpr void put_le(std::span<std::uint8_t> bytes, std::size_t offset,
                      std::size_t width, std::uint64_t value) noexcept
{
    for (std::size_t i = 0; i < width; ++i)
        bytes[offset + i] = static_cast<std::uint8_t>(value >> (8 * i));
}

[[nodiscard]] constexpr std::uint64_t
checksum(std::span<const std::uint8_t> bytes) noexcept
{
    std::uint64_t value = 14695981039346656037ull;
    for (const auto byte : bytes)
        value = (value ^ byte) * 1099511628211ull;
    return value;
}
} // namespace profile_detail
} // namespace closed_circuit
