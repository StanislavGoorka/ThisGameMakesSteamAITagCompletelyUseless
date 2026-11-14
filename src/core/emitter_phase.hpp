#pragma once
#include "core/emitter_footprint.hpp"
#include "core/game_state.hpp"

namespace closed_circuit
{
// The caller owns the damage limit shared with other threats in this turn.
// This helper never advances time or changes an emitter's configuration.
inline void resolve_emitter_phase(GameState &state,
                                  std::span<const PeriodicEmitter> emitters,
                                  bool &damage_applied) noexcept
{
    if (damage_applied || !state.player_alive())
        return;
    for (const auto &emitter : emitters)
    {
        if (emitter.fires_next(state.turns_taken) &&
            emitter_footprint(state.ports,
                              emitter)[state.player_position.value()])
        {
            --state.player_integrity;
            damage_applied = true;
            return;
        }
    }
}
} // namespace closed_circuit
