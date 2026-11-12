#include "../support/state_checks.hpp"
#include "core/sector_module_usage.hpp"
#include "core/turn_preview.hpp"
#include <iostream>
#include <limits>
using namespace closed_circuit;
namespace
{
NodeId node(int value)
{
    return *NodeId::from_value(value);
}
bool check_turn(const GameState &state, const Command &command, bool accepted,
                const GameState &expected)
{
    const auto before = state;
    const auto preview = preview_turn(state, command);
    const auto turn = resolve_turn(state, command);
    return is_command_legal(state, command) == accepted &&
           preview.accepted == accepted && turn.accepted == accepted &&
           preview.outcome == turn.outcome &&
           test::same_state(preview.state, expected) &&
           test::same_state(turn.state, expected) &&
           test::same_state(state, before);
}
bool cases()
{
    for (const bool used : {false, true})
        for (unsigned modules = 0; modules < 4; ++modules)
        {
            GameState state{.player_position = node(0)};
            if (modules & 1)
                static_cast<void>(state.modules.add(ModuleId::rotation_range));
            if (modules & 2)
                static_cast<void>(
                    state.modules.add(ModuleId::terminal_rotation));
            state.terminal_rotation_used = used;
            state.player_integrity = 0;
            state.turns_taken = TurnIndex{48};
            state.ports[0] = *PortMask::from_bits(1);
            state.packet = Packet{node(1), node(48)};
            state.pulses = {{*EntityId::from_value(8), node(7)}};
            state.sector_entry = node(48);
            state.sector_exit = node(0);
            state.receiver_nodes[48] = state.powered_nodes[48] = true;
            state.blocked_nodes[8] = state.rotatable_nodes[9] = true;
            state.reserve = 1;
            state.overload_debt = 6;
            auto expected = state;
            expected.terminal_rotation_used = false;
            reset_sector_module_usage(state);
            if (!test::same_state(state, expected))
                return false;
            reset_sector_module_usage(state);
            if (!test::same_state(state, expected))
                return false;
        }
    GameState renewal{.player_position = node(0)};
    static_cast<void>(renewal.modules.add(ModuleId::terminal_rotation));
    renewal.ports[0] = *PortMask::from_bits(1);
    auto first = resolve_turn(renewal, RotateCommand{node(0)});
    if (!first.accepted || !first.state.terminal_rotation_used)
        return false;
    const auto waited = resolve_turn(first.state, WaitCommand{});
    if (!waited.accepted || !waited.state.terminal_rotation_used ||
        !check_turn(waited.state, RotateCommand{node(0)}, false, waited.state))
        return false;
    auto renewed = waited.state;
    reset_sector_module_usage(renewed);
    auto expected_renewed = renewed;
    expected_renewed.ports[0] = *PortMask::from_bits(8);
    expected_renewed.turns_taken = TurnIndex{3};
    expected_renewed.reserve = 1;
    expected_renewed.overload_debt = 1;
    expected_renewed.terminal_rotation_used = true;
    if (!check_turn(renewed, OverdriveCommand{node(0)}, true, expected_renewed))
        return false;
    GameState route{.player_position = node(1)};
    static_cast<void>(route.modules.add(ModuleId::terminal_rotation));
    route.ports[1] = *PortMask::from_bits(1);
    route.ports[2] = *PortMask::from_bits(8);
    route.packet = Packet{node(1), node(2)};
    route.pulses = {{*EntityId::from_value(9), node(2)}};
    auto routed = route;
    routed.ports[1] = *PortMask::from_bits(2);
    routed.packet.reset();
    routed.powered_nodes[2] = true;
    routed.reserve = 3;
    routed.pulses.clear();
    routed.player_integrity = 2;
    routed.turns_taken = TurnIndex{1};
    routed.terminal_rotation_used = true;
    if (!check_turn(route, RotateCommand{node(1)}, true, routed))
        return false;
    // Every mask, module pair, allowance and action, including an ordinary
    // rotatable node after the allowance was already consumed.
    for (unsigned modules = 0; modules < 4; ++modules)
        for (const bool used : {false, true})
            for (const bool rotatable : {false, true})
                for (unsigned bits = 0; bits < 16; ++bits)
                    for (const bool overdrive : {false, true})
                        for (const int target : {0, 1, 2, 3, 7, 8, 48})
                        {
                            GameState state{.player_position = node(0)};
                            if (modules & 1)
                                static_cast<void>(state.modules.add(
                                    ModuleId::rotation_range));
                            if (modules & 2)
                                static_cast<void>(state.modules.add(
                                    ModuleId::terminal_rotation));
                            state.terminal_rotation_used = used;
                            state.ports[target] = *PortMask::from_bits(bits);
                            state.rotatable_nodes[target] = rotatable;
                            state.blocked_nodes[target] = true;
                            const int distance = target / 7 + target % 7;
                            const bool terminal = bits == 1 || bits == 2 ||
                                                  bits == 4 || bits == 8;
                            const bool allowed =
                                distance <= ((modules & 1) ? 2 : 1) &&
                                (rotatable ||
                                 ((modules & 2) && !used && terminal));
                            const Command command =
                                overdrive
                                    ? Command{OverdriveCommand{node(target)}}
                                    : Command{RotateCommand{node(target)}};
                            auto expected = state;
                            if (allowed)
                            {
                                unsigned rotated =
                                    ((bits << 1) & 15) | (bits >> 3);
                                if (overdrive)
                                    rotated =
                                        ((rotated << 1) & 15) | (rotated >> 3);
                                expected.ports[target] =
                                    *PortMask::from_bits(rotated);
                                expected.terminal_rotation_used =
                                    used || !rotatable;
                                expected.turns_taken = TurnIndex{1};
                                if (overdrive)
                                {
                                    expected.reserve = 1;
                                    expected.overload_debt = 1;
                                }
                            }
                            if (!check_turn(state, command, allowed, expected))
                                return false;
                            if (allowed && !rotatable)
                            {
                                // Shared allowance, including another fixed
                                // terminal target.
                                for (const Command &again :
                                     {Command{RotateCommand{node(target)}},
                                      Command{OverdriveCommand{node(target)}}})
                                    if (!check_turn(expected, again, false,
                                                    expected))
                                        return false;
                                expected.ports[1] = *PortMask::from_bits(1);
                                if (!check_turn(expected,
                                                RotateCommand{node(1)}, false,
                                                expected))
                                    return false;
                            }
                        }
    for (const bool overflow : {false, true})
    {
        GameState state{.player_position = node(0)};
        static_cast<void>(state.modules.add(ModuleId::terminal_rotation));
        state.ports[0] = *PortMask::from_bits(1);
        if (overflow)
            state.overload_debt = std::numeric_limits<std::uint32_t>::max();
        else
            state.reserve = 0;
        if (!check_turn(state, OverdriveCommand{node(0)}, false, state))
            return false;
    }
    GameState ended{.player_position = node(0), .player_integrity = 0};
    static_cast<void>(ended.modules.add(ModuleId::terminal_rotation));
    ended.ports[0] = *PortMask::from_bits(1);
    return check_turn(ended, RotateCommand{node(0)}, false, ended);
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Terminal module full-state contract mismatch\n";
        return 1;
    }
    return 0;
}
