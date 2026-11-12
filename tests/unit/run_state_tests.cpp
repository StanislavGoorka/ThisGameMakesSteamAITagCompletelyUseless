#include "../support/run_checks.hpp"
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_convertible_v<int, SectorNumber>);
static_assert(!std::is_default_constructible_v<SectorNumber>);
static_assert(!SectorNumber::from_value(-1));
static_assert(!SectorNumber::from_value(UINT64_MAX));
namespace
{
bool cases()
{
    for (const int invalid : {-100, -1, 0, 4, 255})
        if (SectorNumber::from_value(invalid))
            return false;
    const auto zero = *NodeId::from_value(0), last = *NodeId::from_value(48);
    for (int number = 1; number <= 3; ++number)
        for (int outcome = 0; outcome < 6; ++outcome)
            for (unsigned modules = 0; modules < 4; ++modules)
                for (const bool claimed : {false, true})
                {
                    RunState run{.seed = RunSeed{UINT64_MAX},
                                 .sector_number =
                                     *SectorNumber::from_value(number),
                                 .sector = GameState{.player_position = zero}};
                    run.reward_claimed = claimed;
                    if (modules & 1)
                        static_cast<void>(
                            run.sector.modules.add(ModuleId::rotation_range));
                    if (modules & 2)
                        static_cast<void>(run.sector.modules.add(
                            ModuleId::terminal_rotation));
                    run.sector.terminal_rotation_used = true;
                    run.sector.overload_debt = 6;
                    run.sector.pulses = {{*EntityId::from_value(7), last}};
                    if (outcome != 0)
                    {
                        run.sector.sector_exit = zero;
                        run.sector.receiver_nodes[1] =
                            run.sector.receiver_nodes[2] = true;
                        run.sector.powered_nodes[1] =
                            run.sector.powered_nodes[2] = true;
                    }
                    if (outcome == 2 || outcome == 4)
                        run.sector.turns_taken = TurnIndex{48};
                    if (outcome == 3 || outcome == 4)
                        run.sector.player_integrity = 0;
                    if (outcome == 4)
                        run.sector.sector_exit = last;
                    if (outcome == 5)
                        run.sector.turns_taken = TurnIndex{49};
                    const RunPhase expected = outcome >= 3   ? RunPhase::lost
                                              : outcome == 0 ? RunPhase::playing
                                              : number == 3  ? RunPhase::won
                                              : (claimed || modules == 3)
                                                  ? RunPhase::ready_for_next
                                                  : RunPhase::choosing_reward;
                    const auto before = run;
                    for (int repeat = 0; repeat < 3; ++repeat)
                        if (run_phase(run) != expected ||
                            !test::same_run(run, before))
                            return false;
                    auto copy = run;
                    copy.sector.pulses.clear();
                    copy.sector.modules = {};
                    copy.reward_claimed = !claimed;
                    copy.sector_number =
                        *SectorNumber::from_value(number == 3 ? 1 : 3);
                    copy.seed = RunSeed{0};
                    if (!test::same_run(run, before) ||
                        test::same_run(copy, before))
                        return false;
                }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Run phase or complete-state contract mismatch\n";
        return 1;
    }
    return 0;
}
