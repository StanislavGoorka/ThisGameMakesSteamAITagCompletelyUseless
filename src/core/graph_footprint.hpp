#pragma once
#include "core/grid_adjacency.hpp"

namespace closed_circuit
{
// Logical reachability only: includes the source and ignores unit occupancy.
[[nodiscard]] inline std::array<bool, NodeId::max_value + 1>
graph_footprint(std::span<const PortMask, NodeId::max_value + 1> ports,
                NodeId source, std::uint8_t range) noexcept
{
    constexpr std::size_t count = NodeId::max_value + 1;
    std::array<bool, count> reached{};
    std::array<std::uint8_t, count> distance{};
    std::array<NodeId::Value, count> queue{};
    std::size_t head = 0, tail = 1;
    queue[0] = source.value();
    reached[queue[0]] = true;
    while (head < tail)
    {
        const auto current = queue[head++];
        if (distance[current] >= range)
            continue;
        for (const auto neighbor :
             connected_neighbors(ports, *NodeId::from_value(current)))
        {
            if (!neighbor || reached[neighbor->value()])
                continue;
            const auto next = neighbor->value();
            reached[next] = true;
            distance[next] = static_cast<std::uint8_t>(distance[current] + 1);
            queue[tail++] = next;
        }
    }
    return reached;
}
} // namespace closed_circuit
