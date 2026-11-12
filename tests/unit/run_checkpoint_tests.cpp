#include "../support/run_checks.hpp"
#include "application/run_checkpoint.hpp"
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
bool rejected(const RunState &run)
{
    const auto before = run;
    return !capture_checkpoint(run) && test::same_run(run, before);
}
bool cases()
{
    for (const std::uint64_t seed : {0ull, 1ull, 2ull, UINT64_MAX})
        for (int number = 1; number <= 3; ++number)
            for (const std::uint32_t debt : {0u, 2u, 3u, 6u, UINT32_MAX})
            {
                auto boundary = start_run(RunSeed{seed});
                if (number > 1)
                {
                    auto previous = boundary;
                    previous.sector_number =
                        *SectorNumber::from_value(number - 1);
                    previous.sector.player_position = node(48);
                    previous.sector.powered_nodes =
                        previous.sector.receiver_nodes;
                    previous.reward_claimed = true;
                    previous.sector.player_integrity = 1;
                    previous.sector.reserve = 0;
                    previous.sector.overload_debt = debt;
                    static_cast<void>(previous.sector.modules.add(
                        ModuleId::terminal_rotation));
                    const auto next = advance_run(previous);
                    if (!next.accepted)
                        return false;
                    boundary = next.state;
                }
                const auto before = boundary;
                const auto cp = capture_checkpoint(boundary);
                const RunCheckpoint expected{
                    RunSeed{seed},
                    *SectorNumber::from_value(number),
                    static_cast<std::uint8_t>(number == 1 ? 3 : 1),
                    static_cast<std::uint8_t>(number == 1 ? 2 : 0),
                    number == 1 ? 0
                    : debt >= 3 ? debt - 3
                                : debt,
                    boundary.sector.modules,
                    number > 1 && debt >= 3};
                if (!cp || *cp != expected)
                    return false;
                for (int repeat = 0; repeat < 3; ++repeat)
                {
                    auto restored = restore_checkpoint(*cp);
                    if (!restored || !test::same_run(*restored, before) ||
                        !test::same_run(boundary, before) ||
                        capture_checkpoint(*restored) != cp)
                        return false;
                    restored->sector.pulses.clear();
                    restored->sector.player_integrity = 0;
                    if (!test::same_run(boundary, before))
                        return false;
                }
                for (int i = 0; i < 49; ++i)
                    for (int field = 0; field < 5; ++field)
                    {
                        auto bad = boundary;
                        if (field == 0)
                            bad.sector.ports[i] = *PortMask::from_bits(
                                (bad.sector.ports[i].bits() + 1) % 16);
                        if (field == 1)
                            bad.sector.rotatable_nodes[i] =
                                !bad.sector.rotatable_nodes[i];
                        if (field == 2)
                            bad.sector.blocked_nodes[i] =
                                !bad.sector.blocked_nodes[i];
                        if (field == 3)
                            bad.sector.powered_nodes[i] =
                                !bad.sector.powered_nodes[i];
                        if (field == 4)
                            bad.sector.receiver_nodes[i] =
                                !bad.sector.receiver_nodes[i];
                        if (!rejected(bad))
                            return false;
                    }
                for (int field = 0; field < 12; ++field)
                {
                    auto bad = boundary;
                    if (field == 0)
                        bad.sector.player_position = node(1);
                    if (field == 1)
                        bad.sector.turns_taken = TurnIndex{1};
                    if (field == 2)
                        bad.sector.packet = Packet{node(0), node(1)};
                    if (field == 3)
                        bad.sector.sector_entry.reset();
                    if (field == 4)
                        bad.sector.sector_exit.reset();
                    if (field == 5)
                        bad.sector.pulses = {
                            {*EntityId::from_value(9), node(0)}};
                    if (field == 6)
                        bad.sector.pulses = {
                            {*EntityId::from_value(0), node(1)}};
                    if (field == 7)
                        bad.sector.terminal_rotation_used = true;
                    if (field == 8)
                        bad.reward_claimed = true;
                    if (field == 9)
                        bad.sector.player_integrity = 0;
                    if (field == 10)
                        bad.sector.reserve = 4;
                    if (field == 11)
                        bad.sector.overload_debt = UINT32_MAX;
                    if (!rejected(bad))
                        return false;
                }
            }
    // Complete legal scalar ranges and all canonical module sets.
    for (int hp = 0; hp <= 4; ++hp)
        for (int reserve = 0; reserve <= 4; ++reserve)
            for (unsigned modules = 0; modules < 4; ++modules)
                for (bool pulse : {false, true})
                    for (std::uint32_t debt : {0u, 2u, 3u, UINT32_MAX - 3,
                                               UINT32_MAX - 2, UINT32_MAX})
                    {
                        RunCheckpoint cp{RunSeed{0},
                                         *SectorNumber::from_value(3),
                                         static_cast<std::uint8_t>(hp),
                                         static_cast<std::uint8_t>(reserve),
                                         debt,
                                         {},
                                         pulse};
                        if (modules & 1)
                            static_cast<void>(
                                cp.modules.add(ModuleId::rotation_range));
                        if (modules & 2)
                            static_cast<void>(
                                cp.modules.add(ModuleId::terminal_rotation));
                        const auto before = cp;
                        const bool valid =
                            hp >= 1 && hp <= 3 && reserve <= 3 &&
                            (pulse ? debt <= UINT32_MAX - 3 : debt < 3);
                        const auto run = restore_checkpoint(cp);
                        if (run.has_value() != valid || cp != before)
                            return false;
                        if (run && capture_checkpoint(*run) != cp)
                            return false;
                    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Checkpoint projection full-state mismatch\n";
        return 1;
    }
    return 0;
}
