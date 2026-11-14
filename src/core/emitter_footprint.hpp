#pragma once
#include "core/graph_footprint.hpp"
#include "core/periodic_emitter.hpp"

namespace closed_circuit
{
[[nodiscard]] inline std::array<bool, NodeId::max_value + 1>
emitter_footprint(std::span<const PortMask, NodeId::max_value + 1> ports,
                  const PeriodicEmitter &emitter) noexcept
{
    return graph_footprint(ports, emitter.position(), emitter.range());
}
} // namespace closed_circuit
