#pragma once

#include "core/game_state.hpp"
#include "core/node_rotation.hpp"
#include "core/rotation_rules.hpp"

namespace closed_circuit
{

// Range depends on grid positions, not connected ports or blocking units.
// Applies only the action; the turn resolver accounts for turn costs.
// A rejected rotation leaves every field of the state untouched.
[[nodiscard]] constexpr bool rotate_from_player(GameState &state,
                                                NodeId target) noexcept
{
    constexpr int grid_size = 7;
    static_assert(grid_size * grid_size == GameState::node_count);

    const int source = state.player_position.value();
    const int destination = target.value();
    const auto absolute = [](int value) { return value < 0 ? -value : value; };
    const int distance =
        absolute(source / grid_size - destination / grid_size) +
        absolute(source % grid_size - destination % grid_size);
    const auto rules = rotation_rules(state.modules);
    if (distance > rules.maximum_distance)
    {
        return false;
    }

    return rotate_node(state.ports, state.rotatable_nodes, target);
}

} // namespace closed_circuit
