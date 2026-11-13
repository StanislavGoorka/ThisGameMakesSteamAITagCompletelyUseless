#pragma once
#include "core/run_turn.hpp"

namespace closed_circuit
{
struct ActionPreview
{
    RunResult result;
    std::vector<NodeId> packet_route;
};

// The route starts before movement on this turn, using the action's ports.
// It survives delivery, but promises nothing about later topology changes.
[[nodiscard]] inline ActionPreview project_action(const RunState &run,
                                                  const Command &command)
{
    ActionPreview preview{preview_run_turn(run, command), {}};
    if (!preview.result.accepted)
        return preview;
    auto ready = run.sector;
    static_cast<void>(introduce_packet(ready));
    if (ready.packet)
        preview.packet_route =
            shortest_route(preview.result.state.sector.ports,
                           ready.packet->position, ready.packet->target);
    return preview;
}
} // namespace closed_circuit
