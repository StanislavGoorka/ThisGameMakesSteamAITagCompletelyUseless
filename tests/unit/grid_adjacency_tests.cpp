#include "core/grid_adjacency.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <span>
#include <vector>

using closed_circuit::connected_neighbors;
using closed_circuit::NodeId;
using closed_circuit::PortMask;

namespace
{

constexpr std::size_t node_count = NodeId::max_value + 1;
constexpr auto all_ports =
    *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit |
                         PortMask::south_bit | PortMask::west_bit);
constexpr auto no_ports = *PortMask::from_bits(0);

bool expect_neighbors(std::span<const PortMask, node_count> ports, int node,
                      const std::array<int, 4> &expected)
{
    const auto neighbors =
        connected_neighbors(ports, *NodeId::from_value(node));
    for (std::size_t index = 0; index < neighbors.size(); ++index)
    {
        const int actual = neighbors[index] ? neighbors[index]->value() : -1;
        if (actual != expected[index])
        {
            std::cerr << "Node " << node << " direction "
                      << "NESW"[index] << " expected " << expected[index]
                      << ", got " << actual << '\n';
            return false;
        }
    }
    return true;
}

bool connections_require_reciprocal_ports()
{
    std::vector<PortMask> ports(node_count, no_ports);
    const std::span<const PortMask, node_count> board{ports};
    ports[24] = all_ports;
    if (!expect_neighbors(board, 24, {-1, -1, -1, -1}))
    {
        return false;
    }

    ports[17] = *PortMask::from_bits(PortMask::south_bit);
    ports[25] = *PortMask::from_bits(PortMask::west_bit);
    ports[31] = *PortMask::from_bits(PortMask::north_bit);
    ports[23] = *PortMask::from_bits(PortMask::east_bit);
    if (!expect_neighbors(board, 24, {17, 25, 31, 23}) ||
        !expect_neighbors(board, 17, {-1, -1, 24, -1}) ||
        !expect_neighbors(board, 25, {-1, -1, -1, 24}) ||
        !expect_neighbors(board, 31, {24, -1, -1, -1}) ||
        !expect_neighbors(board, 23, {-1, 24, -1, -1}))
    {
        return false;
    }

    ports[24] = no_ports;
    if (!expect_neighbors(board, 24, {-1, -1, -1, -1}))
    {
        return false;
    }

    ports[17] = *PortMask::from_bits(PortMask::north_bit);
    ports[24] = *PortMask::from_bits(PortMask::north_bit);
    return expect_neighbors(board, 24, {-1, -1, -1, -1});
}

bool connections_stop_at_grid_edges()
{
    const std::vector<PortMask> ports(node_count, all_ports);
    const std::span<const PortMask, node_count> board{ports};
    return expect_neighbors(board, 0, {-1, 1, 7, -1}) &&
           expect_neighbors(board, 6, {-1, -1, 13, 5}) &&
           expect_neighbors(board, 42, {35, 43, -1, -1}) &&
           expect_neighbors(board, 48, {41, -1, -1, 47}) &&
           expect_neighbors(board, 20, {13, -1, 27, 19}) &&
           expect_neighbors(board, 21, {14, 22, 28, -1});
}

} // namespace

int main()
{
    const bool passed = connections_require_reciprocal_ports() &&
                        connections_stop_at_grid_edges();
    return passed ? 0 : 1;
}
