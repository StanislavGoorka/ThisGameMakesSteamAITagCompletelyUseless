#pragma once

#include "core/game_state.hpp"
#include "core/node_rotation.hpp"

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
    const int offset = destination - source;
    const bool horizontal_neighbor =
        source / grid_size == destination / grid_size &&
        (offset == -1 || offset == 1);
    if (offset != 0 && offset != -grid_size && offset != grid_size &&
        !horizontal_neighbor)
    {
        return false;
    }

    return rotate_node(state.ports, state.rotatable_nodes, target);
}

} // namespace closed_circuit
