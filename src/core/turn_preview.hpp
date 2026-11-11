#pragma once

#include "core/turn_resolver.hpp"

namespace closed_circuit
{

// The resolver owns the simulated state. Preview shares its rules and has no
// access to persistence, achievements or an external random generator.
[[nodiscard]] inline TurnResult preview_turn(const GameState &state,
                                             const Command &command)
{
    return resolve_turn(state, command);
}

} // namespace closed_circuit
