#pragma once

#include "core/grid_adjacency.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace closed_circuit
{

// Returns the route including both endpoints, or an empty vector when the
// goal is unreachable. Equal-length routes favor N, E, S, W at each step.
[[nodiscard]] inline std::vector<NodeId>
shortest_route(std::span<const PortMask, NodeId::max_value + 1> ports,
               NodeId start, NodeId goal)
{
    std::array<std::optional<NodeId>, NodeId::max_value + 1> predecessors{};
    predecessors[start.value()] = start;

    std::vector<NodeId> frontier;
    frontier.reserve(predecessors.size());
    frontier.push_back(start);
    for (std::size_t index = 0;
         index < frontier.size() && !predecessors[goal.value()]; ++index)
    {
        const auto node = frontier[index];
        for (const auto &neighbor : connected_neighbors(ports, node))
        {
            if (neighbor && !predecessors[neighbor->value()])
            {
                predecessors[neighbor->value()] = node;
                frontier.push_back(*neighbor);
            }
        }
    }

    if (!predecessors[goal.value()])
    {
        return {};
    }

    std::vector<NodeId> route;
    for (auto node = goal; node != start; node = *predecessors[node.value()])
    {
        route.push_back(node);
    }
    route.push_back(start);
    std::ranges::reverse(route);
    return route;
}

} // namespace closed_circuit
