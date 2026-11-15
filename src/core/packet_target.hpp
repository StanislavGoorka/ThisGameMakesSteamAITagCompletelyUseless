#pragma once

#include "core/game_state.hpp"

#include <cstddef>
#include <optional>

namespace closed_circuit
{

// Target of the next packet: the lowest NodeId that is a receiver and is not
// powered yet; the highest one with packet_priority. An active packet keeps
// its target because only packet introduction asks for the next one. Empty
// without receivers and once every receiver is powered. A powered node that is
// not a receiver does not matter. The predicate reads the receivers and the
// delivery record only and never changes the state.
[[nodiscard]] constexpr std::optional<NodeId>
next_packet_target(const GameState &state) noexcept
{
    const bool highest = state.modules.contains(ModuleId::packet_priority);
    for (std::size_t step = 0; step < GameState::node_count; ++step)
    {
        const auto index = highest ? GameState::node_count - 1 - step : step;
        if (state.receiver_nodes[index] && !state.powered_nodes[index])
        {
            return NodeId::from_value(index);
        }
    }
    return std::nullopt;
}

} // namespace closed_circuit
