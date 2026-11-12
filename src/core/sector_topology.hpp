#pragma once

#include "core/game_state.hpp"
#include "core/splitmix64.hpp"

namespace closed_circuit
{

// Logical data only. This constrained family has 13 active nodes along two
// sides of the grid. It is separate from both GameState and the replay fixture.
struct SectorTopology
{
    std::array<PortMask, GameState::node_count> ports{};
    std::array<bool, GameState::node_count> rotatable_nodes{};
    NodeId entry;
    NodeId exit;
    std::array<NodeId, 2> receivers;
};

[[nodiscard]] inline SectorTopology generate_sector_topology(RunSeed seed)
{
    SplitMix64 rng{seed};
    const bool along_top = *rng.next_in_range(0, 1) == 0;
    std::array<std::uint8_t, 13> path{};
    for (std::size_t index = 0; index < path.size(); ++index)
    {
        path[index] = static_cast<std::uint8_t>(
            along_top ? (index <= 6 ? index : 6 + (index - 6) * 7)
                      : (index <= 6 ? index * 7 : 42 + index - 6));
    }
    SectorTopology topology{.entry = *NodeId::from_value(path.front()),
                            .exit = *NodeId::from_value(path.back()),
                            .receivers = {*NodeId::from_value(path[4]),
                                          *NodeId::from_value(path[8])}};
    const auto direction = [](int from, int to) -> std::uint8_t
    {
        if (to == from + 1)
        {
            return PortMask::east_bit;
        }
        if (to == from - 1)
        {
            return PortMask::west_bit;
        }
        return to > from ? PortMask::south_bit : PortMask::north_bit;
    };
    for (std::size_t index = 0; index < path.size(); ++index)
    {
        std::uint8_t bits = 0;
        if (index > 0)
        {
            bits |= direction(path[index], path[index - 1]);
        }
        if (index + 1 < path.size())
        {
            bits |= direction(path[index], path[index + 1]);
        }
        auto mask = *PortMask::from_bits(bits);
        const bool rotatable =
            index != 0 && index != 4 && index != 8 && index != path.size() - 1;
        if (rotatable)
        {
            const auto turns = *rng.next_in_range(0, 3);
            for (std::uint64_t turn = 0; turn < turns; ++turn)
            {
                mask = mask.rotate_clockwise();
            }
        }
        topology.ports[path[index]] = mask;
        topology.rotatable_nodes[path[index]] = rotatable;
    }
    return topology;
}

} // namespace closed_circuit
