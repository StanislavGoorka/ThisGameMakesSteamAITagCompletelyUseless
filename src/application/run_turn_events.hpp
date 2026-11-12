#pragma once

#include "application/domain_event.hpp"
#include "core/run_turn.hpp"
#include <utility>
#include <vector>

namespace closed_circuit
{
struct RunTurnEventsResult
{
    RunResult turn;
    std::vector<DomainEvent> events;
};

// Pure value result for the application commit path. Resolve once, then
// describe its effects; no callbacks, IO or publication to services.
[[nodiscard]] inline RunTurnEventsResult
resolve_run_turn_events(const RunState &run, const Command &command)
{
    RunTurnEventsResult result{resolve_run_turn(run, command), {}};
    if (!result.turn.accepted)
        return result;
    const auto append = [&](DomainEventPayload payload)
    {
        result.events.push_back(
            {{run.seed, run.sector_number, result.turn.state.sector.turns_taken,
              static_cast<std::uint8_t>(result.events.size())},
             std::move(payload)});
    };
    append(ActionCommitted{command});
    const auto &next = result.turn.state.sector;
    // Only deliver_packet changes powered_nodes in the shared resolver, and
    // the single active packet can power at most one new target per turn.
    for (std::size_t i = 0; i < GameState::node_count; ++i)
        if (!run.sector.powered_nodes[i] && next.powered_nodes[i])
            append(PacketDelivered{*NodeId::from_value(i)});
    const auto outcome = sector_outcome(next);
    if (outcome == SectorOutcome::completed)
        append(SectorCompleted{});
    if (result.turn.phase == RunPhase::won)
        append(RunEnded{RunEndReason::won});
    else if (result.turn.phase == RunPhase::lost)
        append(RunEnded{outcome == SectorOutcome::death
                            ? RunEndReason::death
                            : RunEndReason::overload});
    return result;
}
} // namespace closed_circuit
