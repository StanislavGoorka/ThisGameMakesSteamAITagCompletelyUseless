#pragma once

#include "core/game_state.hpp"
#include "core/grid_pathfinding.hpp"

namespace closed_circuit
{

// Moves the active packet one edge along the shortest route to its target in
// the current topology and reports whether it moved. Ties follow
// shortest_route. The route depends on ports only. Without a packet, without
// a route or on the target, every field of the state stays untouched: the
// packet waits and keeps its target. The step never charges a turn.
[[nodiscard]] inline bool advance_packet(GameState &state)
{
    if (!state.packet)
    {
        return false;
    }

    const auto route = shortest_route(state.ports, state.packet->position,
                                      state.packet->target);
    if (route.size() < 2)
    {
        return false;
    }

    state.packet->position = route[1];
    return true;
}

} // namespace closed_circuit
