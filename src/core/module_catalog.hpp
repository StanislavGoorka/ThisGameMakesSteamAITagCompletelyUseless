#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

namespace closed_circuit
{
// Stable internal IDs, never player-facing labels. Zero is not a module.
enum class ModuleId : std::uint8_t
{
    rotation_range = 1,
    terminal_rotation = 2,
    delivery_reserve = 3
};

inline constexpr std::array module_catalog{ModuleId::rotation_range,
                                           ModuleId::terminal_rotation,
                                           ModuleId::delivery_reserve};
// Implementation does not imply admission to player rewards.
inline constexpr std::array reward_module_catalog{ModuleId::rotation_range,
                                                  ModuleId::terminal_rotation};

[[nodiscard]] constexpr bool is_known_module(ModuleId id) noexcept
{
    return std::ranges::find(module_catalog, id) != module_catalog.end();
}
} // namespace closed_circuit
