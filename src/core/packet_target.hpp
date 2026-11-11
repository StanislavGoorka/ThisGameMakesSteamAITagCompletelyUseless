#pragma once

#include "core/game_state.hpp"

#include <cstddef>
#include <optional>

namespace closed_circuit
{

// Target of the next packet: the lowest NodeId that is a receiver and is not
// powered yet. Empty without receivers and once every receiver is powered. A
// powered node that is not a receiver does not matter. The predicate reads
// the receivers and the delivery record only and never changes the state.
[[nodiscard]] constexpr std::optional<NodeId>
next_packet_target(const GameState &state) noexcept
{
    for (std::size_t index = 0; index < GameState::node_count; ++index)
    {
        if (state.receiver_nodes[index] && !state.powered_nodes[index])
        {
            return NodeId::from_value(index);
        }
    }
    return std::nullopt;
}

} // namespace closed_circuit
