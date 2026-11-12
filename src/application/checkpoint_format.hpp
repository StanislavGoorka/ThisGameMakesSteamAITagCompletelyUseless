#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace closed_circuit
{
inline constexpr std::uint32_t checkpoint_schema_version = 1;
inline constexpr std::uint32_t checkpoint_run_rules_version = 1;
using CheckpointBytes = std::array<std::uint8_t, 40>;

namespace checkpoint_detail
{
// Internal calls use fixed, specification-defined offsets and widths.
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
} // namespace checkpoint_detail
} // namespace closed_circuit
