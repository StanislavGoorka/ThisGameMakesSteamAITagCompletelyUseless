#pragma once

#include "core/turn_resolver.hpp"

#include <span>
#include <utility>

namespace closed_circuit
{

// A certificate proves actual play, not graph reachability. Every supplied
// command must be accepted, and completion must occur at the end of the trace.
// The normal sector limit bounds accepted commands; this is not a solver.
[[nodiscard]] inline bool validate_solution(const GameState &state,
                                            std::span<const Command> commands)
{
    if (sector_outcome(state) != SectorOutcome::ongoing)
    {
        return false;
    }
    auto current = state;
    for (const auto &command : commands)
    {
        auto result = resolve_turn(current, command);
        if (!result.accepted)
        {
            return false;
        }
        current = std::move(result.state);
    }
    return sector_outcome(current) == SectorOutcome::completed;
}

} // namespace closed_circuit
