#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_start.hpp"
using namespace closed_circuit;
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
void ports(RunState &run, int n, int bits)
{
    run.sector.ports[n] = *PortMask::from_bits(bits);
    run.sector.rotatable_nodes[n] = true;
}
RunState fixture()
{
    RunState run{.seed = RunSeed{12},
                 .sector_number = *SectorNumber::from_value(1),
                 .sector = GameState{.player_position = node(7)}};
    run.sector.packet = Packet{node(7), node(8)};
    ports(run, 7, 1);
    ports(run, 8, 8);
    return run;
}
bool check(const RunState &run, const Command &command,
           std::optional<NodeId> expected)
{
    const auto original = run;
    const auto core = resolve_run_turn(run, command);
    const auto preview = preview_run_turn(run, command);
    const auto first = resolve_run_turn_events(run, command);
    const auto second = resolve_run_turn_events(run, command);
    return first.turn.accepted && first.events == second.events &&
           test::same_run(first.turn.state, core.state) &&
           test::same_run(preview.state, core.state) &&
           test::same_run(run, original) &&
           std::get<ActionCommitted>(first.events.front().payload)
                   .reconfigured_packet_target == expected;
}
int main()
{
    auto run = fixture();
    // No route -> [7,8], and delivery in the same turn.
    if (!check(run, RotateCommand{node(7)}, node(8)))
        return 1;
    // Same rule for a packet introduced at the beginning of this turn.
    run.sector.packet.reset();
    run.sector.sector_entry = node(7);
    run.sector.receiver_nodes[8] = true;
    if (!check(run, RotateCommand{node(7)}, node(8)))
        return 1;
    run = fixture();
    ports(run, 7, 8);
    if (!check(run, OverdriveCommand{node(7)}, node(8)))
        return 1;
    run = fixture();
    ports(run, 7, 2);
    if (!check(run, RotateCommand{node(7)}, {}) ||
        !check(run, WaitCommand{}, {}) || !check(run, MoveCommand{node(8)}, {}))
        return 1;
    // Symmetric mask and unrelated rotation leave the selected route unchanged.
    ports(run, 7, 15);
    if (!check(run, RotateCommand{node(7)}, {}))
        return 1;
    ports(run, 0, 1);
    if (!check(run, RotateCommand{node(0)}, {}))
        return 1;
    run.sector.packet.reset();
    if (!check(run, RotateCommand{node(7)}, {}))
        return 1;
    run.sector.packet = Packet{node(8), node(8)};
    if (!check(run, RotateCommand{node(7)}, {}))
        return 1;
    // Existing BFS route [0,1,8] changes to [0,7,8].
    run = fixture();
    run.sector.player_position = node(1);
    run.sector.packet = Packet{node(0), node(8)};
    ports(run, 0, 6);
    ports(run, 1, 12);
    ports(run, 7, 3);
    ports(run, 8, 9);
    if (!check(run, RotateCommand{node(1)}, node(8)))
        return 1;
    // A rejected rotation never emits evidence.
    if (!resolve_run_turn_events(run, RotateCommand{node(48)}).events.empty())
        return 1;
    // Replay after a real checkpoint round-trip reproduces full payloads.
    const auto fresh = start_run(RunSeed{12});
    const auto checkpoint = capture_checkpoint(fresh);
    if (!checkpoint)
        return 1;
    const auto decoded =
        decode_checkpoint(encode_checkpoint(*checkpoint).value());
    const auto restored = decoded ? restore_checkpoint(*decoded) : std::nullopt;
    if (!restored)
        return 1;
    for (const Command &command :
         {Command{WaitCommand{}}, Command{RotateCommand{node(1)}}})
        if (resolve_run_turn_events(fresh, command).events !=
            resolve_run_turn_events(*restored, command).events)
            return 1;
    return 0;
}
