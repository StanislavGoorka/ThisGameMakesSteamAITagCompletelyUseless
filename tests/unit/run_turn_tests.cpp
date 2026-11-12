#include "../support/run_checks.hpp"
#include "core/run_turn.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
bool cases()
{
    for (int number = 1; number <= 3; ++number)
        for (int scenario = 0; scenario < 5; ++scenario)
        {
            RunState run{.seed = RunSeed{123},
                         .sector_number = *SectorNumber::from_value(number),
                         .sector = GameState{.player_position = node(0)}};
            run.sector.ports[0] = *PortMask::from_bits(2);
            run.sector.ports[1] = *PortMask::from_bits(8);
            run.sector.sector_exit = node(1);
            run.sector.receiver_nodes[7] = run.sector.receiver_nodes[48] = true;
            run.sector.powered_nodes[7] = run.sector.powered_nodes[48] = true;
            run.sector.overload_debt = 6;
            run.sector.terminal_rotation_used = true;
            run.sector.turns_taken = TurnIndex{47};
            if (scenario == 2)
            {
                run.sector.player_integrity = 1;
                run.sector.pulses = {{*EntityId::from_value(9), node(1)}};
            }
            if (scenario == 4)
            {
                static_cast<void>(
                    run.sector.modules.add(ModuleId::rotation_range));
                static_cast<void>(
                    run.sector.modules.add(ModuleId::terminal_rotation));
            }
            const Command command =
                scenario == 0   ? Command{MoveCommand{node(48)}}
                : scenario == 3 ? Command{WaitCommand{}}
                                : Command{MoveCommand{node(1)}};
            const auto before = run;
            auto expected = run;
            if (scenario != 0)
                expected.sector.turns_taken = TurnIndex{48};
            if (scenario == 1 || scenario == 2 || scenario == 4)
                expected.sector.player_position = node(1);
            if (scenario == 2)
            {
                expected.sector.player_integrity = 0;
                expected.sector.pulses.clear();
            }
            const auto phase = scenario == 0 ? RunPhase::playing
                               : scenario == 2 || scenario == 3 ? RunPhase::lost
                               : number == 3                    ? RunPhase::won
                               : scenario == 4 ? RunPhase::ready_for_next
                                               : RunPhase::choosing_reward;
            for (int repeat = 0; repeat < 3; ++repeat)
            {
                auto preview = preview_run_turn(run, command);
                auto actual = resolve_run_turn(run, command);
                if (actual.accepted != (scenario != 0) ||
                    actual.phase != phase ||
                    !test::same_run(actual.state, expected) ||
                    preview.accepted != actual.accepted ||
                    preview.phase != phase ||
                    !test::same_run(preview.state, expected) ||
                    !test::same_run(run, before))
                    return false;
                // Every non-playing phase rejects before any sector phases.
                if (scenario != 0)
                {
                    const auto refused =
                        resolve_run_turn(actual.state, WaitCommand{});
                    if (refused.accepted || refused.phase != phase ||
                        !test::same_run(refused.state, expected))
                        return false;
                }
                actual.state.sector.pulses.push_back(
                    {*EntityId::from_value(88), node(48)});
                actual.state.reward_claimed = true;
                if (!test::same_run(preview.state, expected) ||
                    !test::same_run(run, before))
                    return false;
            }
        }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Run turn full-state mismatch\n";
        return 1;
    }
    return 0;
}
