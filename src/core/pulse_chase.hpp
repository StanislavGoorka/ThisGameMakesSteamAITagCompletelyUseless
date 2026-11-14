#pragma once

#include "core/game_state.hpp"
#include "core/grid_pathfinding.hpp"
#include "core/pulse_occupancy.hpp"

#include <algorithm>
#include <optional>

namespace closed_circuit
{

// Selects one edge of the current shortest route to the player. An occupied
// first step means waiting, without looking for a detour. Nothing is moved
// and no turn is charged; contact is handled by a later step.
[[nodiscard]] inline std::optional<NodeId>
next_pulse_step(const GameState &state, EntityId id)
{
    const auto pulse = std::ranges::find(state.pulses, id, &Pulse::id);
    if (pulse == state.pulses.end())
    {
        return std::nullopt;
    }

    const auto route =
        shortest_route(state.ports, pulse->position, state.player_position);
    if (route.size() < 2 || pulse_occupies(state, route[1]))
    {
        return std::nullopt;
    }
    const auto next = route[1];
    if (state.modules.contains(ModuleId::powered_pulse_barrier) &&
        state.receiver_nodes[next.value()] && state.powered_nodes[next.value()])
        return std::nullopt;
    return next;
}

} // namespace closed_circuit
