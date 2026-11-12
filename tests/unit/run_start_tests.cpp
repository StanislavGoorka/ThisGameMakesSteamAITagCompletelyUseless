#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int value)
{
    return *NodeId::from_value(value);
}
struct Vector
{
    RunSeed seed;
    bool top;
    std::array<std::uint8_t, 13> masks;
};
// Independent BigInt SplitMix64 and numerical masks, recorded in P035-02.
const Vector vectors[]{
    {RunSeed{0}, false, {4, 5, 10, 5, 5, 10, 6, 10, 10, 10, 5, 5, 8}},
    {RunSeed{1}, true, {2, 10, 10, 5, 10, 5, 12, 10, 5, 10, 10, 10, 1}},
    {RunSeed{2}, true, {2, 10, 10, 10, 10, 10, 12, 5, 5, 5, 5, 10, 1}},
    {RunSeed{UINT64_MAX}, false, {4, 5, 5, 5, 5, 5, 3, 5, 10, 5, 10, 10, 8}}};
bool cases()
{
    SplitMix64 external{RunSeed{42}}, reference = external;
    for (const auto &v : vectors)
    {
        RunState expected{.seed = v.seed,
                          .sector = GameState{.player_position = node(0)}};
        expected.sector.sector_entry = node(0);
        expected.sector.sector_exit = node(48);
        for (int i = 0; i < 13; ++i)
        {
            const auto n = v.top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                                 : (i <= 6 ? i * 7 : 42 + i - 6);
            expected.sector.ports[n] = *PortMask::from_bits(v.masks[i]);
            expected.sector.rotatable_nodes[n] =
                i != 0 && i != 4 && i != 8 && i != 12;
            if (i == 4 || i == 8)
                expected.sector.receiver_nodes[n] = true;
        }
        // Explicit resource/reset expectations, not copied from start_run.
        expected.sector.player_integrity = 3;
        expected.sector.reserve = 2;
        expected.sector.overload_debt = 0;
        expected.sector.turns_taken = TurnIndex{0};
        expected.sector.modules = {};
        expected.sector.terminal_rotation_used = false;
        expected.reward_claimed = false;
        expected.sector_number = *SectorNumber::from_value(1);
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            auto run = start_run(v.seed);
            const auto before = run;
            if (!test::same_run(run, expected) ||
                run_phase(run) != RunPhase::playing ||
                external.next_u64() != reference.next_u64())
                return false;
            auto copy = run;
            copy.sector.pulses.push_back({*EntityId::from_value(1), node(0)});
            copy.sector.packet = Packet{node(1), node(2)};
            copy.sector.player_integrity = 0;
            copy.sector.reserve = 0;
            copy.sector.overload_debt = 9;
            static_cast<void>(
                copy.sector.modules.add(ModuleId::rotation_range));
            copy.sector.terminal_rotation_used = true;
            copy.reward_claimed = true;
            copy.sector_number = *SectorNumber::from_value(3);
            run.sector.ports[0] = *PortMask::from_bits(15);
            if (!test::same_run(start_run(v.seed), before) ||
                test::same_run(copy, before))
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
        std::cerr << "Fresh run complete-state vector mismatch\n";
        return 1;
    }
    return 0;
}
