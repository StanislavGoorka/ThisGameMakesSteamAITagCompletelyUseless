#pragma once

#include "core/identifiers.hpp"
#include "core/port_mask.hpp"

#include <span>

namespace closed_circuit
{

// Both spans are indexed by NodeId. A fixed node leaves the board untouched.
[[nodiscard]] constexpr bool
rotate_node(std::span<PortMask, NodeId::max_value + 1> ports,
            std::span<const bool, NodeId::max_value + 1> rotatable_nodes,
            NodeId node) noexcept
{
    const auto index = node.value();
    if (!rotatable_nodes[index])
    {
        return false;
    }

    ports[index] = ports[index].rotate_clockwise();
    return true;
}

} // namespace closed_circuit
