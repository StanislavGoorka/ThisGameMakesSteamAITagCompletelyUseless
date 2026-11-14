#pragma once
#include "core/capacitor_footprint.hpp"
#include "core/game_state.hpp"

namespace closed_circuit
{
// The caller invokes this once per accepted action, sharing the damage limit.
inline void resolve_capacitor_phase(GameState &state,
                                    std::span<DelayedCapacitor> capacitors,
                                    bool &damage_applied) noexcept
{
    for (auto &capacitor : capacitors)
    {
        if (capacitor.advance_fuse() && !damage_applied &&
            state.player_alive() &&
            capacitor_footprint(state.ports,
                                capacitor)[state.player_position.value()])
        {
            --state.player_integrity;
            damage_applied = true;
        }
    }
}
} // namespace closed_circuit
