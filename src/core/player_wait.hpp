#pragma once

#include "core/game_state.hpp"
#include "core/turn_cost.hpp"

namespace closed_circuit
{

// Wait has no board effect, so unlike move_player and rotate_from_player it
// is a complete command: the liveness check plus the cost of one turn. It
// restores no resource. A dead player rejects it and every field of the
// state stays untouched.
[[nodiscard]] constexpr bool wait_player(GameState &state) noexcept
{
    if (!state.player_alive())
    {
        return false;
    }

    charge_turn(state);
    return true;
}

} // namespace closed_circuit
