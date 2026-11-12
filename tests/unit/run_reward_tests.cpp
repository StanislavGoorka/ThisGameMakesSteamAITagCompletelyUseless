#include "../support/run_checks.hpp"
#include "core/run_reward.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
bool cases()
{
    const auto zero = *NodeId::from_value(0), last = *NodeId::from_value(48);
    for (int number = 1; number <= 3; ++number)
        for (int outcome = 0; outcome < 4; ++outcome)
            for (unsigned modules = 0; modules < 4; ++modules)
                for (const bool claimed : {false, true})
                    for (const int choice : {0, 1, 2, 3, 255})
                    {
                        RunState run{
                            .seed = RunSeed{UINT64_MAX},
                            .sector_number = *SectorNumber::from_value(number),
                            .sector = GameState{.player_position = zero}};
                        run.reward_claimed = claimed;
                        run.sector.player_integrity = 2;
                        run.sector.reserve = 1;
                        run.sector.overload_debt = 6;
                        run.sector.terminal_rotation_used = true;
                        run.sector.packet = Packet{last, zero};
                        run.sector.pulses = {{*EntityId::from_value(7), last}};
                        if (modules & 1)
                            static_cast<void>(run.sector.modules.add(
                                ModuleId::rotation_range));
                        if (modules & 2)
                            static_cast<void>(run.sector.modules.add(
                                ModuleId::terminal_rotation));
                        if (outcome != 0)
                        {
                            run.sector.sector_exit = zero;
                            run.sector.receiver_nodes[1] =
                                run.sector.receiver_nodes[2] = true;
                            run.sector.powered_nodes[1] =
                                run.sector.powered_nodes[2] = true;
                        }
                        if (outcome == 2)
                            run.sector.player_integrity = 0;
                        if (outcome == 3)
                            run.sector.turns_taken = TurnIndex{49};
                        const auto before = run;
                        const bool accepted =
                            outcome == 1 && number < 3 && !claimed &&
                            ((choice == 1 && !(modules & 1)) ||
                             (choice == 2 && !(modules & 2)));
                        auto expected = before;
                        const auto id = static_cast<ModuleId>(choice);
                        if (accepted)
                        {
                            static_cast<void>(expected.sector.modules.add(id));
                            expected.reward_claimed = true;
                        }
                        if (choose_run_module(run, id) != accepted ||
                            !test::same_run(run, expected))
                            return false;
                        // Even a different valid ID cannot claim a second
                        // reward.
                        if (accepted)
                        {
                            if (run_phase(run) != RunPhase::ready_for_next)
                                return false;
                            for (const auto next :
                                 {ModuleId::rotation_range,
                                  ModuleId::terminal_rotation})
                                if (choose_run_module(run, next) ||
                                    !test::same_run(run, expected))
                                    return false;
                        }
                        auto copy = run;
                        copy.sector.pulses.clear();
                        if (!test::same_run(run, expected))
                            return false;
                    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Run reward atomicity mismatch\n";
        return 1;
    }
    return 0;
}
