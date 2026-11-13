#include "../support/run_checks.hpp"
#include "application/action_preview.hpp"
#include "core/splitmix64.hpp"
#include <array>
#include <iostream>
using namespace closed_circuit;

namespace
{
constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}
RunState board()
{
    RunState run{.seed = RunSeed{17},
                 .sector = GameState{.player_position = node(1)}};
    auto &s = run.sector;
    s.player_position = node(1);
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.receiver_nodes[2] = true;
    s.receiver_nodes[48] = true;
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(5);
    s.ports[2] = *PortMask::from_bits(8);
    s.rotatable_nodes[1] = true;
    s.packet = Packet{node(0), node(2)};
    return run;
}
bool check(const RunState &run, const Command &command, bool accepted,
           const std::vector<NodeId> &route)
{
    const auto before = run;
    const auto command_before = command;
    SplitMix64 rng{RunSeed{42}};
    const auto rng_before = rng;
    auto reference_rng = rng_before;
    const auto committed = resolve_run_turn(run, command);
    for (int i = 0; i < 4; ++i)
    {
        auto preview = project_action(run, command);
        if (preview.result.accepted != accepted ||
            preview.result.accepted != committed.accepted ||
            preview.result.phase != committed.phase ||
            !test::same_run(preview.result.state, committed.state) ||
            preview.packet_route != route || !test::same_run(run, before) ||
            command != command_before)
            return false;
        preview.result.state.sector.ports[0] = PortMask{};
        preview.result.state.sector.pulses.push_back(
            {*EntityId::from_value(4), node(3)});
        preview.packet_route.clear();
    }
    return rng.next_u64() == reference_rng.next_u64();
}
bool scenarios()
{
    auto run = board();
    if (!check(run, WaitCommand{}, true, {}) ||
        !check(run, RotateCommand{node(1)}, true,
               {node(0), node(1), node(2)}) ||
        !check(run, OverdriveCommand{node(1)}, true, {}) ||
        !check(run, MoveCommand{node(2)}, false, {}))
        return false;
    run.sector.ports[1] = *PortMask::from_bits(10);
    if (!check(run, WaitCommand{}, true, {node(0), node(1), node(2)}))
        return false;
    run.sector.packet->position = node(1);
    const auto delivered = project_action(run, WaitCommand{});
    if (!check(run, WaitCommand{}, true, {node(1), node(2)}) ||
        delivered.result.state.sector.packet ||
        !delivered.result.state.sector.powered_nodes[2])
        return false;
    run.sector.packet.reset();
    if (!check(run, WaitCommand{}, true, {node(0), node(1), node(2)}))
        return false;
    run.sector.powered_nodes[2] = true;
    run.sector.powered_nodes[48] = true;
    if (!check(run, WaitCommand{}, true, {}))
        return false;
    for (int phase = 0; phase < 5; ++phase)
    {
        auto terminal = run;
        if (phase < 2)
        {
            if (phase == 0)
                terminal.sector.player_integrity = 0;
            else
                terminal.sector.turns_taken = TurnIndex{48};
        }
        else
        {
            terminal.sector.sector_exit = terminal.sector.player_position;
            terminal.reward_claimed = phase == 3;
            if (phase == 4)
                terminal.sector_number = *SectorNumber::from_value(3);
        }
        for (const auto &command : std::array<Command, 4>{
                 WaitCommand{}, RotateCommand{node(1)},
                 OverdriveCommand{node(1)}, MoveCommand{node(2)}})
            if (!check(terminal, command, false, {}))
                return false;
    }
    return true;
}
} // namespace
int main()
{
    if (scenarios())
        return 0;
    std::cerr << "Action preview result, route or isolation mismatch\n";
    return 1;
}
