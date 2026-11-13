#include "../support/run_checks.hpp"
#include "application/checkpoint_boundary.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "core/run_turn.hpp"
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
// The decision is pure: the same input gives the same answer and stays intact.
bool decides(const RunState &run, CheckpointAction action)
{
    const auto before = run;
    const auto first = checkpoint_boundary(run);
    const auto second = checkpoint_boundary(run);
    const auto expected = action == CheckpointAction::save
                              ? capture_checkpoint(run)
                              : std::nullopt;
    return test::same_run(run, before) && first.action == action &&
           second.action == action && first.checkpoint == expected &&
           second.checkpoint == expected &&
           (action != CheckpointAction::save || expected);
}
RunState completed(RunState run)
{
    run.sector.player_position = *run.sector.sector_exit;
    run.sector.powered_nodes = run.sector.receiver_nodes;
    return run;
}
} // namespace
int main()
{
    for (const std::uint64_t seed : {0ull, 1ull, 2ull, UINT64_MAX})
    {
        auto run = start_run(RunSeed{seed});
        if (!decides(run, CheckpointAction::save))
            return 1;
        const auto waited = resolve_run_turn(run, WaitCommand{});
        if (!waited.accepted || !decides(waited.state, CheckpointAction::keep))
            return 2;
        for (int number = 1; number <= 3; ++number)
        {
            auto finished = completed(run);
            if (number == 3)
            {
                if (run_phase(finished) != RunPhase::won ||
                    !decides(finished, CheckpointAction::clear))
                    return 3;
                break;
            }
            if (run_phase(finished) != RunPhase::choosing_reward ||
                !decides(finished, CheckpointAction::keep))
                return 4;
            finished.reward_claimed = true;
            finished.sector.overload_debt = number == 1 ? 3 : 1;
            if (run_phase(finished) != RunPhase::ready_for_next ||
                !decides(finished, CheckpointAction::keep))
                return 5;
            const auto next = advance_run(finished);
            if (!next.accepted ||
                next.state.sector_number.value() != number + 1 ||
                !decides(next.state, CheckpointAction::save))
                return 6;
            // A paid debt pulse is part of the saved sector start.
            if (checkpoint_boundary(next.state).checkpoint->entry_debt_pulse !=
                (number == 1))
                return 7;
            run = next.state;
        }
        auto dead = run;
        dead.sector.player_integrity = 0;
        auto overload = run;
        overload.sector.turns_taken = TurnIndex{48};
        if (run_phase(dead) != RunPhase::lost ||
            run_phase(overload) != RunPhase::lost ||
            !decides(dead, CheckpointAction::clear) ||
            !decides(overload, CheckpointAction::clear))
            return 8;
        // Any local change at turn 0 is not a reconstructible sector start.
        auto moved = run;
        moved.sector.player_position =
            moved.sector.player_position == node(0) ? node(1) : node(0);
        if (!decides(moved, CheckpointAction::keep))
            return 9;
    }
    return 0;
}
