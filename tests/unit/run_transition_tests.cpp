#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
struct Vector
{
    std::uint64_t root;
    int number;
    bool top;
    std::array<std::uint8_t, 13> masks;
};
// Independent BigInt root draws and mask calculation: P035-05-vectors.js.
const Vector vectors[]{
    {0, 2, true, {2, 10, 10, 10, 10, 5, 6, 5, 5, 5, 5, 5, 1}},
    {0, 3, true, {2, 10, 5, 5, 10, 5, 9, 5, 5, 10, 10, 10, 1}},
    {1, 2, true, {2, 5, 10, 10, 10, 5, 12, 5, 5, 5, 10, 10, 1}},
    {1, 3, true, {2, 10, 10, 5, 10, 10, 3, 5, 5, 10, 10, 5, 1}},
    {2, 2, true, {2, 5, 5, 5, 10, 10, 9, 10, 5, 10, 5, 5, 1}},
    {2, 3, true, {2, 10, 5, 5, 10, 10, 12, 5, 5, 5, 10, 10, 1}},
    {UINT64_MAX, 2, false, {4, 10, 10, 10, 5, 10, 12, 10, 10, 5, 10, 5, 8}},
    {UINT64_MAX, 3, true, {2, 10, 10, 10, 10, 5, 3, 5, 5, 10, 5, 10, 1}}};
RunState completed(std::uint64_t seed, int number)
{
    auto run = start_run(RunSeed{seed});
    run.sector_number = *SectorNumber::from_value(number);
    run.sector.player_position = node(48);
    run.sector.powered_nodes = run.sector.receiver_nodes;
    run.sector.turns_taken = TurnIndex{47};
    run.reward_claimed = true;
    return run;
}
bool cases()
{
    for (const auto &v : vectors)
        for (const std::uint32_t debt : {0u, 2u, 3u, 6u, UINT32_MAX})
            for (const std::uint8_t hp : {1, 2, 3})
                for (const std::uint8_t reserve : {0, 1, 2, 3})
                {
                    auto run = completed(v.root, v.number - 1);
                    run.sector.player_integrity = hp;
                    run.sector.reserve = reserve;
                    run.sector.overload_debt = debt;
                    run.sector.blocked_nodes[5] = true;
                    run.sector.packet = Packet{node(5), node(6)};
                    run.sector.pulses = {{*EntityId::from_value(77), node(2)}};
                    run.sector.terminal_rotation_used = true;
                    static_cast<void>(
                        run.sector.modules.add(ModuleId::terminal_rotation));
                    const auto before = run;
                    RunState expected{
                        .seed = RunSeed{v.root},
                        .sector_number = *SectorNumber::from_value(v.number),
                        .sector = GameState{.player_position = node(0)}};
                    expected.sector.sector_entry = node(0);
                    expected.sector.sector_exit = node(48);
                    expected.sector.player_integrity = hp;
                    expected.sector.reserve = reserve;
                    expected.sector.overload_debt = debt >= 3 ? debt - 3 : debt;
                    static_cast<void>(expected.sector.modules.add(
                        ModuleId::terminal_rotation));
                    for (int i = 0; i < 13; ++i)
                    {
                        const auto n = v.top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                                             : (i <= 6 ? i * 7 : 42 + i - 6);
                        expected.sector.ports[n] =
                            *PortMask::from_bits(v.masks[i]);
                        expected.sector.rotatable_nodes[n] =
                            i != 0 && i != 4 && i != 8 && i != 12;
                        if (i == 4 || i == 8)
                            expected.sector.receiver_nodes[n] = true;
                    }
                    if (debt >= 3)
                        expected.sector.pulses = {
                            {*EntityId::from_value(0), node(0)}};
                    for (int repeat = 0; repeat < 2; ++repeat)
                    {
                        auto actual = advance_run(run);
                        if (!actual.accepted ||
                            actual.phase != RunPhase::playing ||
                            !test::same_run(actual.state, expected) ||
                            !test::same_run(run, before))
                            return false;
                        const auto refused = advance_run(actual.state);
                        if (refused.accepted ||
                            !test::same_run(refused.state, expected))
                            return false;
                        actual.state.sector.pulses.clear();
                        actual.state.sector.player_integrity = 0;
                        if (!test::same_run(run, before))
                            return false;
                    }
                }
    for (int scenario = 0; scenario < 5; ++scenario)
    {
        auto run = completed(0, scenario == 4 ? 3 : 1);
        if (scenario == 0)
            run.sector.player_position = node(0);
        if (scenario == 1)
            run.reward_claimed = false;
        if (scenario == 2)
            run.sector.player_integrity = 0;
        if (scenario == 3)
            run.sector.turns_taken = TurnIndex{49};
        const auto before = run;
        const auto refused = advance_run(run);
        if (refused.accepted || !test::same_run(refused.state, before) ||
            !test::same_run(run, before) || refused.phase != run_phase(before))
            return false;
    }
    // A full loadout bypasses the reward even when the flag is false.
    auto full = completed(0, 1);
    full.reward_claimed = false;
    static_cast<void>(full.sector.modules.add(ModuleId::rotation_range));
    static_cast<void>(full.sector.modules.add(ModuleId::terminal_rotation));
    const auto next = advance_run(full);
    return next.accepted && next.state.sector.modules == full.sector.modules &&
           !next.state.reward_claimed;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Sector transition full-state mismatch\n";
        return 1;
    }
    return 0;
}
