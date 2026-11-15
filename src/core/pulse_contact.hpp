#pragma once

#include "core/game_state.hpp"

#include <algorithm>

namespace closed_circuit
{

// ID8: during the first accepted turn the entry absorbs pulse contacts. The
// position is read after the action, so leaving the entry drops protection.
[[nodiscard]] constexpr bool entry_insulated(const GameState &state) noexcept
{
    return state.modules.contains(ModuleId::entry_insulation) &&
           state.turns_taken == TurnIndex{0} && state.sector_entry &&
           state.player_position == *state.sector_entry;
}

// The caller shares damage_applied across every contact in one turn. A
// contact always removes its pulse, including after the damage limit or death.
// An insulated contact neither costs integrity nor uses the shared limit.
[[nodiscard]] inline bool resolve_pulse_contact(GameState &state, EntityId id,
                                                bool &damage_applied) noexcept
{
    const auto pulse = std::ranges::find(state.pulses, id, &Pulse::id);
    if (pulse == state.pulses.end() || pulse->position != state.player_position)
    {
        return false;
    }

    state.pulses.erase(pulse);
    if (!damage_applied && state.player_alive() && !entry_insulated(state))
    {
        --state.player_integrity;
        damage_applied = true;
    }
    return true;
}

} // namespace closed_circuit
