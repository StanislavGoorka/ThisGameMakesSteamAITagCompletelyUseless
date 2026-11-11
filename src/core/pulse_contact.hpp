#pragma once

#include "core/game_state.hpp"

#include <algorithm>

namespace closed_circuit
{

// The caller shares damage_applied across every contact in one turn. A
// contact always removes its pulse, including after the damage limit or death.
[[nodiscard]] inline bool resolve_pulse_contact(GameState &state, EntityId id,
                                                bool &damage_applied) noexcept
{
    const auto pulse = std::ranges::find(state.pulses, id, &Pulse::id);
    if (pulse == state.pulses.end() || pulse->position != state.player_position)
    {
        return false;
    }

    state.pulses.erase(pulse);
    if (!damage_applied && state.player_alive())
    {
        --state.player_integrity;
        damage_applied = true;
    }
    return true;
}

} // namespace closed_circuit
