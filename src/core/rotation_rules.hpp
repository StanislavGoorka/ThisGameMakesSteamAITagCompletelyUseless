#pragma once

#include "core/module_loadout.hpp"

namespace closed_circuit
{
struct RotationRules
{
    int maximum_distance{1};
    bool allows_fixed_terminal{false};
    // Only overdrive reads this; overdrive_range extends it by one.
    int overdrive_distance{1};
    // Clockwise quarter turns of an ordinary rotation; reverse_rotation makes
    // it three, i.e. counter-clockwise. Overdrive always turns twice.
    int quarter_turns{1};
};

// One deterministic port for implemented rotation rule changes.
[[nodiscard]] constexpr RotationRules
rotation_rules(const ModuleLoadout &modules) noexcept
{
    const int distance = modules.contains(ModuleId::rotation_range) ? 2 : 1;
    return {distance, modules.contains(ModuleId::terminal_rotation),
            distance + (modules.contains(ModuleId::overdrive_range) ? 1 : 0),
            modules.contains(ModuleId::reverse_rotation) ? 3 : 1};
}
} // namespace closed_circuit
