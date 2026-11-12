#pragma once

#include <array>
#include <cstdint>

namespace closed_circuit
{
inline constexpr std::uint32_t profile_schema_version = 1;
using ProfileBytes = std::array<std::uint8_t, 24>;
} // namespace closed_circuit
