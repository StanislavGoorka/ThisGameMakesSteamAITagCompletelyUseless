#pragma once
#include "core/delayed_capacitor.hpp"
#include "core/graph_footprint.hpp"

namespace closed_circuit
{
[[nodiscard]] inline std::array<bool, NodeId::max_value + 1>
capacitor_footprint(std::span<const PortMask, NodeId::max_value + 1> ports,
                    const DelayedCapacitor &capacitor) noexcept
{
    return graph_footprint(ports, capacitor.position(), capacitor.range());
}
} // namespace closed_circuit
