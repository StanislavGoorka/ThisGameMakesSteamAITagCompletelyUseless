#include "../support/state_checks.hpp"
#include "core/turn_preview.hpp"
#include "replay/state_hash.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(unsigned n)
{
    return *NodeId::from_value(n);
}
EntityId id(unsigned n)
{
    return *EntityId::from_value(n);
}
DelayedCapacitor cap(unsigned n, unsigned pos, unsigned delay, unsigned range,
                     unsigned remaining)
{
    auto value = *DelayedCapacitor::from_values(id(n), node(pos), delay, range);
    for (unsigned i = remaining; i < delay; ++i)
        static_cast<void>(value.advance_fuse());
    return value;
}
GameState fixture()
{
    GameState s{.player_position = node(0)};
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(10);
    s.ports[2] = *PortMask::from_bits(8);
    s.rotatable_nodes[1] = true;
    s.capacitors = {cap(0, 2, 2, 2, 2)};
    return s;
}
bool matches(const GameState &s, const Command &c, const GameState &expected,
             bool accepted, SectorOutcome outcome)
{
    const auto before = s;
    const auto p = preview_turn(s, c);
    const auto r = resolve_turn(s, c);
    return r.accepted == accepted && p.accepted == accepted &&
           r.outcome == outcome && p.outcome == outcome &&
           test::same_state(s, before) && test::same_state(p.state, expected) &&
           test::same_state(r.state, expected);
}
bool trace()
{
    // Independent byte snapshots: tools/replay/hash_vectors.cjs.
    constexpr std::array<std::uint64_t, 7> hashes{
        3864398672937786353ULL,  16175879701998237795ULL,
        16175879701998237795ULL, 6388936452983494942ULL,
        9980121365215646104ULL,  1165736276133461215ULL,
        14374591989062936971ULL};
    const std::array<Command, 6> commands{
        WaitCommand{},          MoveCommand{node(48)},
        RotateCommand{node(1)}, RotateCommand{node(1)},
        WaitCommand{},          OverdriveCommand{node(1)}};
    const auto before_commands = commands;
    const auto initial = fixture();
    if (developer::state_hash(initial) != hashes[0])
        return false;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        auto s = initial;
        for (unsigned i = 0; i < commands.size(); ++i)
        {
            auto e = initial;
            const unsigned turns = i == 0 || i == 1 ? 1 : i;
            e.turns_taken = TurnIndex{turns};
            e.capacitors = {cap(0, 2, 2, 2, i < 2 ? 1 : 0)};
            e.ports[1] = *PortMask::from_bits(i == 2 ? 5 : 10);
            if (i == 5)
            {
                e.reserve = 1;
                e.overload_debt = 1;
                e.first_overdrive_used = true;
            }
            if (!matches(s, commands[i], e, i != 1, SectorOutcome::ongoing) ||
                developer::state_hash(e) != hashes[i + 1])
                return false;
            s = resolve_turn(s, commands[i]).state;
            if (developer::state_hash(s) != hashes[i + 1])
                return false;
        }
    }
    return commands == before_commands;
}
bool connect_and_move()
{
    // Reconnect on the detonating turn; earlier rotation also changes the
    // graph used two turns later, rather than storing an armed footprint.
    auto s = fixture();
    s.ports[1] = *PortMask::from_bits(5);
    auto e = s;
    e.turns_taken = TurnIndex{1};
    e.capacitors = {cap(0, 2, 2, 2, 1)};
    if (!matches(s, WaitCommand{}, e, true, SectorOutcome::ongoing))
        return false;
    s = e;
    e.ports[1] = *PortMask::from_bits(10);
    e.player_integrity = 2;
    e.turns_taken = TurnIndex{2};
    e.capacitors = {cap(0, 2, 2, 2, 0)};
    if (!matches(s, RotateCommand{node(1)}, e, true, SectorOutcome::ongoing))
        return false;
    s = fixture();
    s.capacitors = {cap(0, 0, 1, 1, 1)};
    e = s;
    e.player_position = node(1);
    e.player_integrity = 2;
    e.turns_taken = TurnIndex{1};
    e.capacitors = {cap(0, 0, 1, 1, 0)};
    if (!matches(s, MoveCommand{node(1)}, e, true, SectorOutcome::ongoing))
        return false;
    // A rotation in the first pending turn already disconnects the blast.
    s = fixture();
    e = s;
    e.ports[1] = *PortMask::from_bits(5);
    e.turns_taken = TurnIndex{1};
    e.capacitors = {cap(0, 2, 2, 2, 1)};
    if (!matches(s, RotateCommand{node(1)}, e, true, SectorOutcome::ongoing))
        return false;
    s = e;
    e.turns_taken = TurnIndex{2};
    e.capacitors = {cap(0, 2, 2, 2, 0)};
    return matches(s, WaitCommand{}, e, true, SectorOutcome::ongoing);
}
bool shared_and_terminal()
{
    for (bool pulse : {false, true})
        for (bool emitter : {false, true})
        {
            auto s = fixture();
            s.capacitors = {cap(0, 0, 1, 1, 1), cap(1, 2, 1, 2, 1)};
            if (pulse)
                s.pulses = {{id(0), node(0)}, {id(1), node(1)}};
            if (emitter)
                s.emitters = {
                    *PeriodicEmitter::from_values(id(0), node(0), 2, 1, 1)};
            auto e = s;
            e.capacitors = {cap(0, 0, 1, 1, 0), cap(1, 2, 1, 2, 0)};
            e.pulses.clear();
            e.player_integrity = 2;
            e.turns_taken = TurnIndex{1};
            if (!matches(s, WaitCommand{}, e, true, SectorOutcome::ongoing))
                return false;
        }
    auto s = fixture();
    s.player_integrity = 1;
    s.capacitors = {cap(0, 0, 1, 1, 1)};
    s.sector_exit = node(0);
    s.receiver_nodes[0] = s.receiver_nodes[1] = true;
    s.powered_nodes[1] = true;
    s.packet = Packet{node(0), node(0)};
    auto e = s;
    e.capacitors = {cap(0, 0, 1, 1, 0)};
    e.packet.reset();
    e.powered_nodes[0] = true;
    e.reserve = 3;
    e.player_integrity = 0;
    e.turns_taken = TurnIndex{1};
    if (!matches(s, WaitCommand{}, e, true, SectorOutcome::death))
        return false;
    s = fixture();
    for (const auto c :
         {Command{MoveCommand{node(48)}}, Command{RotateCommand{node(48)}},
          Command{OverdriveCommand{node(48)}}})
        if (!matches(s, c, s, false, SectorOutcome::ongoing))
            return false;
    for (const auto outcome : {SectorOutcome::death, SectorOutcome::overload,
                               SectorOutcome::completed})
    {
        auto terminal = s;
        if (outcome == SectorOutcome::death)
            terminal.player_integrity = 0;
        if (outcome == SectorOutcome::overload)
            terminal.turns_taken = TurnIndex{48};
        if (outcome == SectorOutcome::completed)
        {
            terminal.sector_exit = node(0);
            terminal.receiver_nodes[0] = terminal.receiver_nodes[1] = true;
            terminal.powered_nodes = terminal.receiver_nodes;
        }
        for (const auto c :
             {Command{WaitCommand{}}, Command{RotateCommand{node(1)}},
              Command{OverdriveCommand{node(1)}}})
            if (!matches(terminal, c, terminal, false, outcome))
                return false;
    }
    // Fuse expiry and the single cost also occur on the final legal turn.
    s = fixture();
    s.turns_taken = TurnIndex{47};
    s.capacitors = {cap(0, 0, 1, 1, 1)};
    e = s;
    e.turns_taken = TurnIndex{48};
    e.player_integrity = 2;
    e.capacitors = {cap(0, 0, 1, 1, 0)};
    return matches(s, WaitCommand{}, e, true, SectorOutcome::overload);
}
} // namespace
int main()
{
    if (!trace() || !connect_and_move() || !shared_and_terminal())
    {
        std::cerr << "Capacitor turn/preview mismatch\n";
        return 1;
    }
    return 0;
}
