#pragma once

#include "core/game_state.hpp"
#include "core/grid_adjacency.hpp"

#include <algorithm>

namespace closed_circuit
{

// Applies only the player action. The turn resolver accounts for turn costs.
// A rejected move leaves every field of the state untouched.
[[nodiscard]] constexpr bool move_player(GameState &state,
                                         NodeId target) noexcept
{
    if (state.blocked_nodes[target.value()])
    {
        return false;
    }

    const auto neighbors =
        connected_neighbors(state.ports, state.player_position);
    if (std::ranges::find(neighbors, target) == neighbors.end())
    {
        return false;
    }

    state.player_position = target;
    return true;
}

} // namespace closed_circuit
