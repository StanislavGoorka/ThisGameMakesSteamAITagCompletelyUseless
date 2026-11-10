#include "core/grid_pathfinding.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <span>
#include <vector>

using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::shortest_route;

namespace
{

constexpr std::size_t node_count = NodeId::max_value + 1;
constexpr auto no_ports = *PortMask::from_bits(0);
constexpr auto all_ports =
    *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit |
                         PortMask::south_bit | PortMask::west_bit);

bool expect_route(std::span<const PortMask, node_count> ports, int start,
                  int goal, std::initializer_list<int> expected)
{
    const auto route = shortest_route(ports, *NodeId::from_value(start),
                                      *NodeId::from_value(goal));
    if (!std::ranges::equal(route, expected, {}, &NodeId::value))
    {
        std::cerr << "Unexpected route from " << start << " to " << goal
                  << '\n';
        return false;
    }
    return true;
}

bool shorter_route_wins_over_earlier_direction()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<const PortMask, node_count> board{ports};
    ports[17] = *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
    ports[18] = *PortMask::from_bits(PortMask::south_bit | PortMask::west_bit);
    ports[24] = *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    ports[25] = *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
    if (!expect_route(board, 24, 25, {24, 25}))
    {
        return false;
    }

    ports[24] = *PortMask::from_bits(PortMask::north_bit);
    return expect_route(board, 24, 25, {24, 17, 18, 25});
}

bool route_to_same_node_requires_no_connections()
{
    const std::vector<PortMask> ports(node_count, no_ports);
    return expect_route(std::span<const PortMask, node_count>{ports}, 48, 48,
                        {48});
}

bool unreachable_goal_returns_empty_route()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<const PortMask, node_count> board{ports};
    ports[24] = *PortMask::from_bits(PortMask::east_bit);
    ports[25] = *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    return expect_route(board, 24, 26, {});
}

bool route_cannot_wrap_between_rows()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<const PortMask, node_count> board{ports};
    ports[6] = *PortMask::from_bits(PortMask::east_bit);
    ports[7] = *PortMask::from_bits(PortMask::west_bit);
    return expect_route(board, 6, 7, {});
}

bool equal_routes_follow_direction_order()
{
    const std::vector<PortMask> ports(node_count, all_ports);
    const std::span<const PortMask, node_count> board{ports};
    return expect_route(board, 24, 18, {24, 17, 18}) &&
           expect_route(board, 24, 32, {24, 25, 32}) &&
           expect_route(board, 24, 30, {24, 31, 30}) &&
           expect_route(board, 24, 16, {24, 17, 16});
}

} // namespace

int main()
{
    const bool passed = shorter_route_wins_over_earlier_direction() &&
                        route_to_same_node_requires_no_connections() &&
                        unreachable_goal_returns_empty_route() &&
                        route_cannot_wrap_between_rows() &&
                        equal_routes_follow_direction_order();
    return passed ? 0 : 1;
}
