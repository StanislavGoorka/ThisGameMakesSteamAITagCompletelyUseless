#pragma once

#include "core/sector_topology.hpp"

namespace closed_circuit::developer
{
// Baseline only: no inherited resources or debt spawning. Not the replay
// fixture and not a substitute for the future run boundary.
[[nodiscard]] inline GameState sector_fixture(RunSeed seed)
{
    const auto topology = generate_sector_topology(seed);
    auto state = *GameState::initial(topology.ports, topology.rotatable_nodes,
                                     topology.entry);
    state.sector_entry = topology.entry;
    state.sector_exit = topology.exit;
    for (const auto receiver : topology.receivers)
    {
        state.receiver_nodes[receiver.value()] = true;
    }
    return state;
}
} // namespace closed_circuit::developer
