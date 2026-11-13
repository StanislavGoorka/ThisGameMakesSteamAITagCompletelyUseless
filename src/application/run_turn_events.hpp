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
    const auto &next = result.turn.state.sector;
    ActionCommitted action{command};
    if (std::holds_alternative<RotateCommand>(command) ||
        std::holds_alternative<OverdriveCommand>(command))
    {
        // Introduction precedes the action. The remaining resolver phases
        // do not change ports, so final ports also describe post-action ports.
        auto introduced = run.sector;
        static_cast<void>(introduce_packet(introduced));
        if (const auto &packet = introduced.packet)
        {
            const auto after =
                shortest_route(next.ports, packet->position, packet->target);
            if (!after.empty() &&
                after != shortest_route(introduced.ports, packet->position,
                                        packet->target))
                action.reconfigured_packet_target = packet->target;
        }
    }
    append(action);
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
