#pragma once

#include "core/game_state.hpp"
#include "core/splitmix64.hpp"

#include <cstdint>

namespace closed_circuit::developer
{

// Synthetic, undisplayed developer data for replay v1, not a sector generator.
// Only node 1's initial orientation depends on the seed.
[[nodiscard]] inline GameState seeded_fixture(RunSeed seed)
{
    GameState state{.player_position = *NodeId::from_value(0)};
    state.ports[0] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
    state.ports[1] =
        *PortMask::from_bits(PortMask::west_bit | PortMask::south_bit);
    state.ports[7] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    state.ports[8] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
    state.rotatable_nodes[1] = true;
    state.receiver_nodes[1] = true;
    state.receiver_nodes[8] = true;
    state.sector_entry = *NodeId::from_value(0);
    state.sector_exit = *NodeId::from_value(8);
    state.pulses.push_back({*EntityId::from_value(1), *NodeId::from_value(7)});

    SplitMix64 rng{seed};
    const auto rotations = *rng.next_in_range(0, 3);
    for (std::uint64_t index = 0; index < rotations; ++index)
    {
        state.ports[1] = state.ports[1].rotate_clockwise();
    }
    return state;
}

} // namespace closed_circuit::developer
