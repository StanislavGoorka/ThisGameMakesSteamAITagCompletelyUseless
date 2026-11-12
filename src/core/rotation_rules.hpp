#pragma once

#include "core/module_loadout.hpp"

namespace closed_circuit
{
struct RotationRules
{
    int maximum_distance{1};
};

// One deterministic port for rotation rule changes. Registered module IDs
// carry no effects until their own implementation steps.
[[nodiscard]] constexpr RotationRules
rotation_rules(const ModuleLoadout &) noexcept
{
    return {};
}
} // namespace closed_circuit
