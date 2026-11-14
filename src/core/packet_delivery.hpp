#pragma once

#include "core/game_state.hpp"

namespace closed_circuit
{

// Delivers the active packet standing on its target: the packet leaves the
// board, the target becomes powered and the step reports true. Without a
// packet, off the target or on a target that is already powered, every field
// of the state stays untouched, so each target is powered at most once and a
// repeated step delivers nothing. A delivery restores one reserve (two with
// delivery_reserve) up to the limit. The step never charges a turn.
[[nodiscard]] constexpr bool deliver_packet(GameState &state) noexcept
{
    if (!state.packet || state.packet->position != state.packet->target)
    {
        return false;
    }

    auto &powered = state.powered_nodes[state.packet->target.value()];
    if (powered)
    {
        return false;
    }

    if (state.reserve < GameState::reserve_limit)
    {
        const auto restored =
            state.modules.contains(ModuleId::delivery_reserve) ? 2 : 1;
        state.reserve = static_cast<std::uint8_t>(
            std::min<int>(GameState::reserve_limit, state.reserve + restored));
    }
    powered = true;
    state.packet.reset();
    return true;
}

} // namespace closed_circuit
