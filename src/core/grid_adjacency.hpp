#pragma once

#include "core/identifiers.hpp"
#include "core/port_mask.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace closed_circuit
{

// Masks follow NodeId order, starting at the upper-left corner and proceeding
// row by row. The result has one slot per direction in N, E, S, W order.
[[nodiscard]] constexpr std::array<std::optional<NodeId>, 4>
connected_neighbors(std::span<const PortMask, NodeId::max_value + 1> ports,
                    NodeId node) noexcept
{
    constexpr int grid_size = 7;
    static_assert(grid_size * grid_size == NodeId::max_value + 1);

    struct Direction
    {
        int row_offset;
        int column_offset;
        std::uint8_t outgoing_port;
        std::uint8_t incoming_port;
    };
    constexpr std::array directions{
        Direction{-1, 0, PortMask::north_bit, PortMask::south_bit},
        Direction{0, 1, PortMask::east_bit, PortMask::west_bit},
        Direction{1, 0, PortMask::south_bit, PortMask::north_bit},
        Direction{0, -1, PortMask::west_bit, PortMask::east_bit}};

    const int row = node.value() / grid_size;
    const int column = node.value() % grid_size;
    const auto source_ports = ports[node.value()].bits();
    std::array<std::optional<NodeId>, directions.size()> neighbors{};
    for (std::size_t index = 0; index < directions.size(); ++index)
    {
        const auto &direction = directions[index];
        const int neighbor_row = row + direction.row_offset;
        const int neighbor_column = column + direction.column_offset;
        if (neighbor_row < 0 || neighbor_row >= grid_size ||
            neighbor_column < 0 || neighbor_column >= grid_size)
        {
            continue;
        }

        const auto neighbor_index = static_cast<std::size_t>(
            neighbor_row * grid_size + neighbor_column);
        if ((source_ports & direction.outgoing_port) != 0 &&
            (ports[neighbor_index].bits() & direction.incoming_port) != 0)
        {
            neighbors[index] = NodeId::from_value(neighbor_index);
        }
    }
    return neighbors;
}

} // namespace closed_circuit
