#pragma once

#include "core/game_state.hpp"
#include "core/grid_pathfinding.hpp"

namespace closed_circuit
{

// Moves the active packet one edge (up to two with packet_stride) toward its
// target in the current topology and reports whether it moved. Ties follow
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

    const std::size_t stride =
        state.modules.contains(ModuleId::packet_stride) ? 2 : 1;
    state.packet->position = route[std::min(stride, route.size() - 1)];
    return true;
}

} // namespace closed_circuit
