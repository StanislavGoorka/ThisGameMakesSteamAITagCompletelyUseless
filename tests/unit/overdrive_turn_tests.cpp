#include "core/splitmix64.hpp"
#include "core/turn_preview.hpp"
#include <algorithm>
#include <array>
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
bool same_state(const GameState &left, const GameState &right)
{
    return left.modules == right.modules && left.reserve == right.reserve &&
           left.overload_debt == right.overload_debt &&
           left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.powered_nodes == right.powered_nodes &&
           left.receiver_nodes == right.receiver_nodes &&
           left.sector_entry == right.sector_entry &&
           left.sector_exit == right.sector_exit &&
           left.pulses == right.pulses &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

GameState board()
{
    GameState s{.player_position = node(24)};
    s.ports[24] = *PortMask::from_bits(2);
    s.ports[25] = *PortMask::from_bits(6);
    s.ports[18] = *PortMask::from_bits(4);
    s.rotatable_nodes[25] = true;
    s.blocked_nodes[25] = true;
    s.sector_entry = node(18);
    s.sector_exit = node(48);
    s.receiver_nodes[1] = s.receiver_nodes[25] = true;
    s.powered_nodes[1] = true;
    s.pulses = {{*EntityId::from_value(9), node(18)}};
    s.reserve = 3;
    s.overload_debt = 7;
    return s;
}
bool expect(const GameState &s, const Command &command, bool accepted,
            const GameState &expected, SectorOutcome outcome)
{
    const auto before = s;
    const auto command_before = command;
    SplitMix64 rng{RunSeed{5}};
    auto reference = rng;
    for (int i = 0; i < 3; ++i)
    {
        const auto p = preview_turn(s, command);
        auto r = resolve_turn(s, command);
        if (is_command_legal(s, command) != accepted ||
            p.accepted != accepted || r.accepted != accepted ||
            p.outcome != outcome || r.outcome != outcome ||
            !same_state(p.state, expected) || !same_state(r.state, expected) ||
            !same_state(s, before) || command != command_before ||
            rng.next_u64() != reference.next_u64())
        {
            std::cerr << "Overdrive turn mismatch\n";
            return false;
        }
        r.state.reserve = 255;
        r.state.overload_debt = 0;
        r.state.pulses.clear();
        if (!same_state(p.state, expected) || !same_state(s, before))
            return false;
    }
    return true;
}
} // namespace
int main()
{
    const Command command = OverdriveCommand{node(25)};
    for (int mode = 0; mode < 4; ++mode)
    {
        auto s = board();
        if (mode > 0)
            s.turns_taken = TurnIndex{47};
        if (mode == 2)
        {
            s.player_integrity = 1;
            s.pulses.push_back({*EntityId::from_value(10), node(24)});
        }
        if (mode == 3)
            s.sector_exit = node(24);
        auto expected = s;
        expected.ports[25] = *PortMask::from_bits(9);
        expected.powered_nodes[25] = true;
        expected.reserve = 3;
        expected.overload_debt = 8;
        expected.turns_taken = TurnIndex{mode == 0 ? 1U : 48U};
        expected.pulses = {{*EntityId::from_value(9), node(25)}};
        if (mode == 2)
            expected.player_integrity = 0;
        const auto outcome = mode == 0   ? SectorOutcome::ongoing
                             : mode == 1 ? SectorOutcome::overload
                             : mode == 2 ? SectorOutcome::death
                                         : SectorOutcome::completed;
        if (!expect(s, command, true, expected, outcome))
            return 1;
    }
    auto s = board();
    s.sector_entry.reset();
    s.receiver_nodes[25] = false;
    s.pulses.clear();
    auto expected = s;
    expected.reserve = 2;
    expected.overload_debt = 8;
    expected.ports[25] = *PortMask::from_bits(9);
    expected.turns_taken = TurnIndex{1};
    if (!expect(s, command, true, expected, SectorOutcome::ongoing))
        return 1;
    for (int mode = 0; mode < 7; ++mode)
    {
        auto s = board();
        if (mode == 0)
            s.reserve = 0;
        if (mode == 1)
            s.overload_debt = UINT32_MAX;
        if (mode == 2)
            s.rotatable_nodes[25] = false;
        if (mode == 3)
            s.player_integrity = 0;
        if (mode == 4)
            s.turns_taken = TurnIndex{48};
        if (mode == 5)
        {
            s.powered_nodes[25] = true;
            s.sector_exit = node(24);
        }
        const Command c =
            mode == 6 ? Command{OverdriveCommand{node(26)}} : command;
        if (!expect(s, c, false, s, sector_outcome(s)))
            return 1;
    }
    return 0;
}
