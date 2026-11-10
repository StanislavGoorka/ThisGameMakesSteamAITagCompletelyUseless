#include "core/grid_adjacency.hpp"
#include "core/node_rotation.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <span>
#include <vector>

using closed_circuit::connected_neighbors;
using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::rotate_node;

namespace
{

constexpr std::size_t node_count = NodeId::max_value + 1;
constexpr auto center = *NodeId::from_value(24);
constexpr auto north = *NodeId::from_value(17);
constexpr auto east = *NodeId::from_value(25);
constexpr auto no_ports = *PortMask::from_bits(0);
constexpr auto north_port = *PortMask::from_bits(PortMask::north_bit);
constexpr auto east_port = *PortMask::from_bits(PortMask::east_bit);

bool fixed_node_rejects_rotation_without_changing_ports()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<PortMask, node_count> board{ports};
    ports[center.value()] = north_port;
    const auto initial_ports = ports;
    std::array<bool, node_count> rotatable_nodes{};
    rotatable_nodes[east.value()] = true;

    if (rotate_node(board, rotatable_nodes, center) ||
        !std::ranges::equal(ports, initial_ports, {}, &PortMask::bits,
                            &PortMask::bits))
    {
        std::cerr
            << "A fixed node must reject rotation and preserve the board\n";
        return false;
    }
    return true;
}

bool rotating_node_changes_connections_then_restores_them()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<PortMask, node_count> board{ports};
    ports[center.value()] = north_port;
    ports[north.value()] = *PortMask::from_bits(PortMask::south_bit);
    ports[east.value()] = *PortMask::from_bits(PortMask::west_bit);
    const auto initial_ports = ports;
    const auto initial_neighbors = connected_neighbors(board, center);
    std::array<bool, node_count> rotatable_nodes{};
    rotatable_nodes[center.value()] = true;

    auto rotated_ports = initial_ports;
    rotated_ports[center.value()] = east_port;
    const bool rotated = rotate_node(board, rotatable_nodes, center);
    const auto neighbors = connected_neighbors(board, center);
    if (!rotated || initial_neighbors[0] != north || initial_neighbors[1] ||
        neighbors[0] || neighbors[1] != east ||
        connected_neighbors(board, north)[2] ||
        connected_neighbors(board, east)[3] != center ||
        !std::ranges::equal(ports, rotated_ports, {}, &PortMask::bits,
                            &PortMask::bits))
    {
        std::cerr
            << "Clockwise rotation must replace the north connection "
               "with an east connection and change only the target mask\n";
        return false;
    }

    for (int turn = 1; turn < 4; ++turn)
    {
        if (!rotate_node(board, rotatable_nodes, center))
        {
            std::cerr << "A rotatable node must accept each quarter turn\n";
            return false;
        }
    }

    if (connected_neighbors(board, center) != initial_neighbors ||
        !std::ranges::equal(ports, initial_ports, {}, &PortMask::bits,
                            &PortMask::bits))
    {
        std::cerr << "Four rotations must restore the original board\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed = fixed_node_rejects_rotation_without_changing_ports() &&
                        rotating_node_changes_connections_then_restores_them();
    return passed ? 0 : 1;
}
