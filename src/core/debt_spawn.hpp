#pragma once

#include "core/pulse_identity.hpp"
#include "core/pulse_occupancy.hpp"

namespace closed_circuit
{

// The run calls this once after assembling a new sector. One entry has room
// for one pulse; any debt not materialized remains for a later sector.
[[nodiscard]] inline bool spawn_debt_pulse(GameState &state)
{
    constexpr std::uint32_t debt_per_pulse = 3;
    if (state.overload_debt < debt_per_pulse || !state.sector_entry ||
        state.blocked_nodes[state.sector_entry->value()] ||
        pulse_occupies(state, *state.sector_entry))
    {
        return false;
    }
    const auto id = available_pulse_id(state);
    if (!id)
    {
        return false;
    }
    // Pulse is nothrow movable: failed vector growth preserves the old state.
    state.pulses.push_back({*id, *state.sector_entry});
    state.overload_debt -= debt_per_pulse;
    return true;
}

} // namespace closed_circuit
