#include "core/command.hpp"
#include "core/player_wait.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <variant>

using closed_circuit::Command;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::is_command_legal;
using closed_circuit::move_player;
using closed_circuit::MoveCommand;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::rotate_from_player;
using closed_circuit::RotateCommand;
using closed_circuit::sector_outcome;
using closed_circuit::SectorOutcome;
using closed_circuit::TurnIndex;
using closed_circuit::wait_player;
using closed_circuit::WaitCommand;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

static_assert(std::variant_size_v<Command> == 3);
static_assert(!std::default_initializable<Command>);
static_assert(!std::default_initializable<MoveCommand>);
static_assert(!std::default_initializable<RotateCommand>);
static_assert(std::get<MoveCommand>(Command{MoveCommand{node(48)}}).target ==
              node(48));

GameState rich_board()
{
    GameState state{.player_position = node(24),
                    .player_integrity = 2,
                    .turns_taken = TurnIndex{13},
                    .packet = Packet{node(8), node(1)},
                    .sector_entry = node(8),
                    .sector_exit = *NodeId::from_value(48),
                    .pulses = {{*EntityId::from_value(9), node(25)},
                               {*EntityId::from_value(3), node(40)}}};
    state.ports.fill(*PortMask::from_bits(15));
    state.rotatable_nodes.fill(true);
    state.blocked_nodes[12] = true;
    state.powered_nodes[2] = true;
    state.receiver_nodes[1] = true;
    state.receiver_nodes[2] = true;
    state.reserve = 0;
    state.overload_debt = 17;
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.reserve == right.reserve &&
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

bool expect_legality(const GameState &state, const Command &command,
                     bool expected, const char *description)
{
    const auto before = state;
    const auto command_before = command;
    const bool actual = is_command_legal(state, command);
    if (actual != expected || !same_state(state, before) ||
        command != command_before)
    {
        std::cerr << description
                  << ": wrong legality or modified input state/command\n";
        return false;
    }

    auto helper_state = state;
    const bool helper_result = std::visit(
        [&helper_state](const auto &action)
        {
            using Action = std::decay_t<decltype(action)>;
            if constexpr (std::is_same_v<Action, MoveCommand>)
            {
                return move_player(helper_state, action.target);
            }
            else if constexpr (std::is_same_v<Action, RotateCommand>)
            {
                return rotate_from_player(helper_state, action.target);
            }
            else
            {
                return wait_player(helper_state);
            }
        },
        command);
    if (actual !=
        (sector_outcome(state) == SectorOutcome::ongoing && helper_result))
    {
        std::cerr << description << ": legality disagrees with action rules\n";
        return false;
    }
    return true;
}

bool move_checks_edges_boundaries_and_blockers_but_allows_a_pulse()
{
    struct Case
    {
        int source;
        int target;
        bool legal;
    };
    constexpr std::array cases{
        Case{24, 17, true},  Case{24, 25, true},  Case{24, 31, true},
        Case{24, 23, true},  Case{0, 1, true},    Case{0, 7, true},
        Case{48, 41, true},  Case{41, 48, true},  Case{24, 24, false},
        Case{24, 26, false}, Case{24, 32, false}, Case{6, 7, false},
        Case{7, 6, false},   Case{0, 42, false},  Case{48, 0, false}};
    for (const auto test : cases)
    {
        auto state = rich_board();
        state.player_position = node(test.source);
        if (!expect_legality(state, MoveCommand{node(test.target)}, test.legal,
                             "move target"))
        {
            return false;
        }
    }

    auto state = rich_board();
    state.blocked_nodes[25] = true;
    if (!expect_legality(state, MoveCommand{node(25)}, false,
                         "separate blocking flag on pulse"))
    {
        return false;
    }
    state.blocked_nodes[25] = false;
    return expect_legality(state, MoveCommand{node(25)}, true,
                           "pulse alone does not block movement");
}

bool absent_or_one_way_edges_reject_move()
{
    constexpr std::array<std::array<unsigned char, 2>, 4> cases{
        {{0, 0},
         {PortMask::east_bit, 0},
         {0, PortMask::west_bit},
         {PortMask::east_bit, PortMask::east_bit}}};
    for (const auto ports : cases)
    {
        auto state = rich_board();
        state.ports[24] = *PortMask::from_bits(ports[0]);
        state.ports[25] = *PortMask::from_bits(ports[1]);
        if (!expect_legality(state, MoveCommand{node(25)}, false,
                             "missing reciprocal edge"))
        {
            return false;
        }
    }
    return true;
}

bool rotation_checks_range_and_rotatability_without_ports_or_occupancy()
{
    struct Case
    {
        int source;
        int target;
        bool legal;
    };
    constexpr std::array cases{
        Case{24, 24, true}, Case{24, 17, true},  Case{24, 25, true},
        Case{24, 31, true}, Case{24, 23, true},  Case{0, 7, true},
        Case{48, 41, true}, Case{24, 16, false}, Case{24, 26, false},
        Case{6, 7, false},  Case{7, 6, false},   Case{0, 42, false}};
    for (const auto test : cases)
    {
        auto state = rich_board();
        state.player_position = node(test.source);
        state.ports.fill(*PortMask::from_bits(0));
        state.ports[test.target] = *PortMask::from_bits(1);
        state.blocked_nodes[test.target] = true;
        if (!expect_legality(state, RotateCommand{node(test.target)},
                             test.legal,
                             "rotation range without connected edge"))
        {
            return false;
        }
    }

    auto state = rich_board();
    state.rotatable_nodes[24] = false;
    state.rotatable_nodes[25] = false;
    return expect_legality(state, RotateCommand{node(24)}, false,
                           "stationary player node") &&
           expect_legality(state, RotateCommand{node(25)}, false,
                           "stationary neighbor");
}

bool wait_needs_only_life_and_all_commands_reject_death()
{
    auto state = rich_board();
    state.ports.fill(*PortMask::from_bits(0));
    state.rotatable_nodes.fill(false);
    state.blocked_nodes.fill(true);
    if (!expect_legality(state, WaitCommand{}, true,
                         "isolated living player can wait without a cost"))
    {
        return false;
    }

    state = rich_board();
    state.player_integrity = 0;
    return expect_legality(state, MoveCommand{node(25)}, false,
                           "dead player cannot move") &&
           expect_legality(state, RotateCommand{node(24)}, false,
                           "dead player cannot rotate") &&
           expect_legality(state, WaitCommand{}, false,
                           "dead player cannot wait");
}

bool ready_packet_is_not_introduced_by_repeated_validation()
{
    auto state = rich_board();
    state.packet.reset();
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        if (!expect_legality(state, MoveCommand{node(25)}, true,
                             "move with packet ready") ||
            !expect_legality(state, RotateCommand{node(24)}, true,
                             "rotate with packet ready") ||
            !expect_legality(state, WaitCommand{}, true,
                             "wait with packet ready"))
        {
            return false;
        }
    }
    return true;
}

bool every_command_rejects_terminal_sectors_without_mutation()
{
    for (const auto outcome : {SectorOutcome::completed, SectorOutcome::death,
                               SectorOutcome::overload})
    {
        auto state = rich_board();
        state.packet.reset();
        state.receiver_nodes[30] = true;
        state.pulses.push_back({*EntityId::from_value(99), node(24)});
        if (outcome == SectorOutcome::completed)
        {
            state.sector_exit = node(24);
            state.powered_nodes[1] = true;
        }
        else if (outcome == SectorOutcome::death)
        {
            state.player_integrity = 0;
        }
        else
        {
            state.turns_taken = TurnIndex{48};
        }
        for (const auto &command : std::array<Command, 3>{
                 MoveCommand{node(25)}, RotateCommand{node(24)}, WaitCommand{}})
        {
            if (!expect_legality(state, command, false,
                                 "terminal sector rejects every command"))
            {
                return false;
            }
        }
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        move_checks_edges_boundaries_and_blockers_but_allows_a_pulse() &&
        absent_or_one_way_edges_reject_move() &&
        rotation_checks_range_and_rotatability_without_ports_or_occupancy() &&
        wait_needs_only_life_and_all_commands_reject_death() &&
        ready_packet_is_not_introduced_by_repeated_validation() &&
        every_command_rejects_terminal_sectors_without_mutation();
    return passed ? 0 : 1;
}
