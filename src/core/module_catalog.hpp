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
    delivery_reserve = 3,
    first_overdrive_debt = 4,
    packet_stride = 5,
    pulse_cadence = 6,
    powered_pulse_barrier = 7,
    entry_insulation = 8,
    delivery_debt_relief = 9
};

inline constexpr std::array module_catalog{
    ModuleId::rotation_range,        ModuleId::terminal_rotation,
    ModuleId::delivery_reserve,      ModuleId::first_overdrive_debt,
    ModuleId::packet_stride,         ModuleId::pulse_cadence,
    ModuleId::powered_pulse_barrier, ModuleId::entry_insulation,
    ModuleId::delivery_debt_relief};
// Implementation does not imply admission to player rewards.
inline constexpr std::array reward_module_catalog{ModuleId::rotation_range,
                                                  ModuleId::terminal_rotation};

[[nodiscard]] constexpr bool is_known_module(ModuleId id) noexcept
{
    return std::ranges::find(module_catalog, id) != module_catalog.end();
}
} // namespace closed_circuit
