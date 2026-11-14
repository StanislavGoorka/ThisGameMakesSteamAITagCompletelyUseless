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
PeriodicEmitter emitter(unsigned n, unsigned pos, unsigned phase = 1)
{
    return *PeriodicEmitter::from_values(id(n), node(pos), 2, phase, 2);
}
GameState fixture()
{
    GameState s{.player_position = node(0)};
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(9);
    s.ports[2] = *PortMask::from_bits(8);
    s.rotatable_nodes[1] = true;
    s.emitters = {emitter(0, 2), emitter(1, 0)};
    return s;
}
bool result_matches(const GameState &s, const Command &c,
                    const GameState &expected, bool accepted,
                    SectorOutcome outcome)
{
    const auto before = s;
    const auto p = preview_turn(s, c);
    const auto r = resolve_turn(s, c);
    return r.accepted == accepted && p.accepted == accepted &&
           r.outcome == outcome && p.outcome == outcome &&
           test::same_state(s, before) && test::same_state(p.state, expected) &&
           test::same_state(r.state, expected);
}
bool cases()
{
    auto s = fixture();
    // Only the remote emitter: rotation opens 0-1-2 in this very turn.
    s.emitters.erase(s.emitters.begin() + 1);
    auto expected = s;
    expected.ports[1] = *PortMask::from_bits(3);
    expected.turns_taken = TurnIndex{1};
    if (!result_matches(s, RotateCommand{node(1)}, expected, true,
                        SectorOutcome::ongoing))
        return false;
    s.ports[1] = *PortMask::from_bits(5);
    expected = s;
    expected.ports[1] = *PortMask::from_bits(10);
    expected.player_integrity = 2;
    expected.turns_taken = TurnIndex{1};
    if (!result_matches(s, RotateCommand{node(1)}, expected, true,
                        SectorOutcome::ongoing))
        return false;
    // All contacts are removed even when the first pulse consumed the limit.
    s = fixture();
    s.pulses = {{id(8), node(0)}, {id(7), node(1)}};
    expected = s;
    expected.pulses.clear();
    expected.player_integrity = 2;
    expected.turns_taken = TurnIndex{1};
    if (!result_matches(s, WaitCommand{}, expected, true,
                        SectorOutcome::ongoing))
        return false;
    auto shared = s;
    bool damage = true;
    resolve_pulse_phase(shared, damage);
    auto pulse_expected = s;
    pulse_expected.pulses.clear();
    if (!damage || !test::same_state(shared, pulse_expected))
        return false;
    // A delivery happens before emitter death, which precedes success.
    s = fixture();
    s.player_integrity = 1;
    s.sector_exit = node(0);
    s.receiver_nodes[0] = s.receiver_nodes[1] = true;
    s.powered_nodes[1] = true;
    s.packet = Packet{node(0), node(0)};
    expected = s;
    expected.packet.reset();
    expected.powered_nodes[0] = true;
    expected.reserve = 3;
    expected.player_integrity = 0;
    expected.turns_taken = TurnIndex{1};
    if (!result_matches(s, WaitCommand{}, expected, true, SectorOutcome::death))
        return false;
    s = fixture();
    for (const auto c :
         {Command{MoveCommand{node(48)}}, Command{RotateCommand{node(48)}},
          Command{OverdriveCommand{node(48)}}})
        if (!result_matches(s, c, s, false, SectorOutcome::ongoing))
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
        if (!result_matches(terminal, WaitCommand{}, terminal, false, outcome))
            return false;
    }
    // Explicit schedule and complete snapshots for repeated simulation traces.
    auto a = s, b = s;
    for (unsigned t = 1; t <= 6; ++t)
    {
        auto e = a;
        const bool accepted = t <= 5;
        if (accepted)
        {
            e.turns_taken = TurnIndex{t};
            if (t % 2)
                --e.player_integrity;
        }
        const auto outcome =
            t >= 5 ? SectorOutcome::death : SectorOutcome::ongoing;
        if (!result_matches(a, WaitCommand{}, e, accepted, outcome))
            return false;
        a = resolve_turn(a, WaitCommand{}).state;
        b = resolve_turn(b, WaitCommand{}).state;
        if (!test::same_state(a, b) ||
            developer::state_hash(a) != developer::state_hash(b))
            return false;
    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Emitter turn/preview contract failed\n";
        return 1;
    }
    return 0;
}
