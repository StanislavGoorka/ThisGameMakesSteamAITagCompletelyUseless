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
    delivery_debt_relief = 9,
    overdrive_range = 10,
    reverse_rotation = 11,
    packet_priority = 12
};

inline constexpr std::array module_catalog{
    ModuleId::rotation_range,        ModuleId::terminal_rotation,
    ModuleId::delivery_reserve,      ModuleId::first_overdrive_debt,
    ModuleId::packet_stride,         ModuleId::pulse_cadence,
    ModuleId::powered_pulse_barrier, ModuleId::entry_insulation,
    ModuleId::delivery_debt_relief,  ModuleId::overdrive_range,
    ModuleId::reverse_rotation,      ModuleId::packet_priority};
// Every implemented module is admitted to rewards since P085-19. Admission
// stays a separate ordered list so that a future module can be held back.
inline constexpr std::array reward_module_catalog = module_catalog;

[[nodiscard]] constexpr bool is_known_module(ModuleId id) noexcept
{
    return std::ranges::find(module_catalog, id) != module_catalog.end();
}
} // namespace closed_circuit
