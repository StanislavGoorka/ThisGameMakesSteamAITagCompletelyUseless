#pragma once

#include "core/sector_topology.hpp"

namespace closed_circuit
{
// Same logical perimeter family, with a third fixed receiver at path index 10.
// This standalone variant does not change production run boundaries.
struct FinalSectorTopology
{
    std::array<PortMask, GameState::node_count> ports;
    std::array<bool, GameState::node_count> rotatable_nodes;
    NodeId entry;
    NodeId exit;
    std::array<NodeId, 3> receivers;
};

[[nodiscard]] inline FinalSectorTopology
generate_final_sector_topology(RunSeed seed)
{
    const auto normal = generate_sector_topology(seed);
    const bool top = normal.receivers[0].value() == 4;
    const auto third = *NodeId::from_value(top ? 34 : 46);
    FinalSectorTopology final{
        normal.ports,
        normal.rotatable_nodes,
        normal.entry,
        normal.exit,
        {normal.receivers[0], normal.receivers[1], third}};
    final.ports[third.value()] = *PortMask::from_bits(top ? 5 : 10);
    final.rotatable_nodes[third.value()] = false;
    return final;
}
} // namespace closed_circuit
