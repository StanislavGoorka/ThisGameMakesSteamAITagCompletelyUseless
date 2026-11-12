#pragma once

#include "core/module_loadout.hpp"

namespace closed_circuit
{
struct RotationRules
{
    int maximum_distance{1};
};

// One deterministic port for implemented rotation rule changes.
[[nodiscard]] constexpr RotationRules
rotation_rules(const ModuleLoadout &modules) noexcept
{
    return {modules.contains(ModuleId::rotation_range) ? 2 : 1};
}
} // namespace closed_circuit
