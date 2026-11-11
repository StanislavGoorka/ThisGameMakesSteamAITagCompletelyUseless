#pragma once

#include "core/game_state.hpp"

#include <cstddef>

namespace closed_circuit
{

enum class SectorOutcome
{
    ongoing,
    completed,
    death,
    overload
};

// Reads the settled state only: pending deliveries and contacts are not phases
// of this query. Death precedes completion; completion on turn 48 precedes
// overload, but a state beyond the limit can never complete late.
[[nodiscard]] constexpr SectorOutcome
sector_outcome(const GameState &state) noexcept
{
    if (!state.player_alive())
    {
        return SectorOutcome::death;
    }

    constexpr auto turn_limit = TurnIndex{48};
    if (state.turns_taken <= turn_limit && state.sector_exit &&
        state.player_position == *state.sector_exit)
    {
        std::size_t powered_receivers = 0;
        for (std::size_t index = 0; index < GameState::node_count; ++index)
        {
            if (state.receiver_nodes[index] && state.powered_nodes[index])
            {
                ++powered_receivers;
            }
        }
        if (powered_receivers >= 2)
        {
            return SectorOutcome::completed;
        }
    }

    return state.turns_taken >= turn_limit ? SectorOutcome::overload
                                           : SectorOutcome::ongoing;
}

} // namespace closed_circuit
