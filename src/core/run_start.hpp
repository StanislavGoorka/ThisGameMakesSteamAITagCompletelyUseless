#pragma once

#include "core/run_state.hpp"
#include "core/sector_topology.hpp"

#include <utility>

namespace closed_circuit
{
// A fresh run has no inherited resources, rewards or per-sector usage.
// Sector N will use the Nth local SplitMix64 draw; this creates only sector 1.
[[nodiscard]] inline RunState start_run(RunSeed seed)
{
    SplitMix64 rng{seed};
    const auto topology = generate_sector_topology(RunSeed{rng.next_u64()});
    auto state = *GameState::initial(topology.ports, topology.rotatable_nodes,
                                     topology.entry);
    state.sector_entry = topology.entry;
    state.sector_exit = topology.exit;
    for (const auto receiver : topology.receivers)
        state.receiver_nodes[receiver.value()] = true;
    return RunState{.seed = seed, .sector = std::move(state)};
}
} // namespace closed_circuit
