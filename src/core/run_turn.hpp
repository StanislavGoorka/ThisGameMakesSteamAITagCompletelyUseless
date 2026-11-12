#pragma once

#include "core/run_state.hpp"
#include "core/turn_resolver.hpp"

namespace closed_circuit
{
struct RunResult
{
    RunState state;
    bool accepted;
    RunPhase phase;
};

[[nodiscard]] inline RunResult resolve_run_turn(const RunState &run,
                                                const Command &command)
{
    RunResult result{run, false, run_phase(run)};
    if (result.phase != RunPhase::playing)
        return result;
    auto turn = resolve_turn(run.sector, command);
    result.accepted = turn.accepted;
    result.state.sector = std::move(turn.state);
    result.phase = run_phase(result.state);
    return result;
}

[[nodiscard]] inline RunResult preview_run_turn(const RunState &run,
                                                const Command &command)
{
    return resolve_run_turn(run, command);
}
} // namespace closed_circuit
