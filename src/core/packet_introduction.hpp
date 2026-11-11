#pragma once

#include "core/game_state.hpp"
#include "core/packet_target.hpp"

namespace closed_circuit
{

// Called once at the beginning of a turn, before movement and delivery.
// Places the next packet on the sector entry, including the first packet.
// Only the packet changes; no turn is charged. With an active packet, no
// entry or no unpowered receiver, the entire state stays untouched.
// The caller must not repeat this step after delivery in the same turn.
[[nodiscard]] constexpr bool introduce_packet(GameState &state) noexcept
{
    if (state.packet || !state.sector_entry)
    {
        return false;
    }

    const auto target = next_packet_target(state);
    if (!target)
    {
        return false;
    }

    state.packet = Packet{.position = *state.sector_entry, .target = *target};
    return true;
}

} // namespace closed_circuit
