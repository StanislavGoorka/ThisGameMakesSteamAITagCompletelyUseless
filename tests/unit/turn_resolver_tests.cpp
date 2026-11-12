#include "core/splitmix64.hpp"
#include "core/turn_resolver.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>

using closed_circuit::Command;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::MoveCommand;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::resolve_turn;
using closed_circuit::RotateCommand;
using closed_circuit::RunSeed;
using closed_circuit::SectorOutcome;
using closed_circuit::SplitMix64;
using closed_circuit::TurnIndex;
using closed_circuit::WaitCommand;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr auto low_id = *EntityId::from_value(0);
constexpr auto spare_id = *EntityId::from_value(7);
constexpr auto high_id = *EntityId::from_value(EntityId::max_value);

GameState board()
{
    GameState state{.player_position = node(24),
                    .player_integrity = 2,
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{node(8), node(1)},
                    .sector_entry = node(8),
                    .sector_exit = *NodeId::from_value(48),
                    .pulses = {{spare_id, node(48)}, {low_id, node(0)}}};
    state.ports[24] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[25] = *PortMask::from_bits(PortMask::west_bit);
    state.rotatable_nodes[24] = true;
    state.rotatable_nodes[25] = true;
    state.blocked_nodes[12] = true;
    state.powered_nodes[3] = true;
    state.receiver_nodes[1] = true;
    state.receiver_nodes[48] = true;
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

bool expect_turn(const GameState &state, const Command &command, bool accepted,
                 const GameState &expected, std::string_view context,
                 SectorOutcome outcome = SectorOutcome::ongoing)
{
    const auto before = state;
    const auto command_before = command;
    const auto result = resolve_turn(state, command);
    if (result.accepted != accepted || result.outcome != outcome ||
        !same_state(result.state, expected) || !same_state(state, before) ||
        command != command_before)
    {
        std::cerr << context
                  << ": unexpected result or modified input state/command\n";
        return false;
    }
    return true;
}

bool illegal_commands_run_no_phase_or_cost()
{
    auto state = board();
    state.packet.reset();
    state.sector_entry = node(24);
    state.pulses = {{high_id, node(24)}, {low_id, node(25)}};
    state.blocked_nodes[25] = true;
    state.rotatable_nodes[24] = false;
    const std::array<Command, 5> commands{
        MoveCommand{node(25)}, MoveCommand{node(17)}, MoveCommand{node(26)},
        RotateCommand{node(24)}, RotateCommand{node(26)}};
    for (const auto &command : commands)
    {
        if (!expect_turn(state, command, false, state,
                         "illegal command with ready packet and contacts"))
        {
            return false;
        }
    }

    state.blocked_nodes[25] = false;
    state.rotatable_nodes[24] = true;
    state.player_integrity = 0;
    for (const auto &command : std::array<Command, 3>{
             MoveCommand{node(25)}, RotateCommand{node(24)}, WaitCommand{}})
    {
        if (!expect_turn(state, command, false, state,
                         "dead player runs no phase", SectorOutcome::death))
        {
            return false;
        }
    }
    return true;
}

bool each_legal_command_pays_one_turn()
{
    const auto state = board();
    auto expected = state;
    expected.turns_taken = TurnIndex{8};
    if (!expect_turn(state, WaitCommand{}, true, expected,
                     "wait pays one turn"))
    {
        return false;
    }
    expected.player_position = node(25);
    if (!expect_turn(state, MoveCommand{node(25)}, true, expected,
                     "move pays one turn"))
    {
        return false;
    }
    expected.player_position = state.player_position;
    expected.ports[24] = *PortMask::from_bits(PortMask::south_bit);
    return expect_turn(state, RotateCommand{node(24)}, true, expected,
                       "rotate pays one turn");
}

bool packet_and_pulse_move_exactly_one_edge()
{
    auto state = board();
    state.player_position = node(26);
    state.ports.fill(*PortMask::from_bits(0));
    state.ports[22] = *PortMask::from_bits(PortMask::east_bit);
    for (int value = 23; value <= 25; ++value)
    {
        state.ports[value] =
            *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    }
    state.ports[26] = *PortMask::from_bits(PortMask::west_bit);
    state.packet = Packet{node(22), node(26)};
    state.receiver_nodes[26] = true;
    state.pulses = {{high_id, node(22)}, {spare_id, node(48)}};
    auto expected = state;
    expected.packet->position = node(23);
    expected.pulses.front().position = node(23);
    expected.turns_taken = TurnIndex{8};
    return expect_turn(state, WaitCommand{}, true, expected,
                       "one packet step and one pulse phase");
}

bool rotation_precedes_packet_and_pulse_routes()
{
    auto state = board();
    state.ports[24] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    state.ports[17] = *PortMask::from_bits(PortMask::south_bit);
    state.ports[31] = *PortMask::from_bits(PortMask::north_bit);
    state.packet = Packet{node(17), node(25)};
    state.receiver_nodes[25] = true;
    state.pulses = {{high_id, node(31)}, {spare_id, node(48)}};
    auto expected = state;
    expected.ports[24] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
    expected.pulses = {{spare_id, node(48)}};
    expected.player_integrity = 1;
    expected.turns_taken = TurnIndex{8};
    return expect_turn(state, RotateCommand{node(24)}, true, expected,
                       "new topology closes packet route and opens pursuit");
}

bool player_entry_is_contact_before_pulse_movement()
{
    auto state = board();
    state.pulses = {{spare_id, node(48)}, {high_id, node(25)}};
    auto expected = state;
    expected.player_position = node(25);
    expected.player_integrity = 1;
    expected.pulses = {{spare_id, node(48)}};
    expected.turns_taken = TurnIndex{8};
    return expect_turn(state, MoveCommand{node(25)}, true, expected,
                       "player enters pulse before threat phase");
}

bool delivery_and_turn_cost_survive_death_in_threat_phase()
{
    auto state = board();
    state.player_position = node(26);
    state.player_integrity = 1;
    state.ports[25] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = *PortMask::from_bits(PortMask::west_bit);
    state.packet = Packet{node(24), node(25)};
    state.receiver_nodes[25] = true;
    state.pulses = {{high_id, node(25)}, {spare_id, node(48)}};
    auto expected = state;
    expected.packet.reset();
    expected.powered_nodes[25] = true;
    expected.pulses = {{spare_id, node(48)}};
    expected.player_integrity = 0;
    expected.turns_taken = TurnIndex{8};
    return expect_turn(state, WaitCommand{}, true, expected,
                       "delivery precedes death and accepted turn is charged",
                       SectorOutcome::death);
}

bool next_packet_waits_for_the_next_accepted_turn()
{
    auto state = board();
    state.player_position = node(48);
    state.sector_exit = node(47);
    state.packet.reset();
    state.sector_entry = node(0);
    state.receiver_nodes.fill(false);
    state.receiver_nodes[1] = true;
    state.receiver_nodes[2] = true;
    state.ports[0] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[1] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[2] = *PortMask::from_bits(PortMask::west_bit);
    state.pulses = {{spare_id, node(40)}};
    auto first = state;
    first.powered_nodes[1] = true;
    first.turns_taken = TurnIndex{8};
    if (!expect_turn(state, WaitCommand{}, true, first,
                     "first packet introduced then delivered") ||
        !expect_turn(first, MoveCommand{node(47)}, false, first,
                     "rejection does not introduce second packet"))
    {
        return false;
    }
    auto second = first;
    second.packet = Packet{node(1), node(2)};
    second.turns_taken = TurnIndex{9};
    if (!expect_turn(first, WaitCommand{}, true, second,
                     "next packet appears and moves once next turn"))
    {
        return false;
    }
    auto third = second;
    third.packet.reset();
    third.powered_nodes[2] = true;
    third.turns_taken = TurnIndex{10};
    if (!expect_turn(second, WaitCommand{}, true, third,
                     "active second packet is delivered"))
    {
        return false;
    }
    auto fourth = third;
    fourth.turns_taken = TurnIndex{11};
    return expect_turn(third, WaitCommand{}, true, fourth,
                       "no packet after all receivers are powered");
}

bool accepted_and_rejected_results_own_independent_state()
{
    const auto state = board();
    const auto before = state;
    for (const auto &command :
         std::array<Command, 2>{WaitCommand{}, MoveCommand{node(26)}})
    {
        auto result = resolve_turn(state, command);
        const auto untouched_result = resolve_turn(state, command);
        result.state.pulses.front().id = high_id;
        result.state.pulses.front().position = node(47);
        result.state.pulses.push_back({*EntityId::from_value(99), node(46)});
        result.state.packet->position = node(9);
        result.state.powered_nodes[3] = false;
        result.state.sector_exit = node(0);
        auto expected = state;
        if (untouched_result.accepted)
        {
            expected.turns_taken = TurnIndex{8};
        }
        if (!same_state(state, before) ||
            !same_state(untouched_result.state, expected))
        {
            std::cerr << "Every result must independently own its state\n";
            return false;
        }
    }
    return true;
}

bool last_turn_classifies_after_delivery_threats_and_cost()
{
    auto state = board();
    state.turns_taken = TurnIndex{47};
    state.sector_exit = node(25);
    state.packet = Packet{node(24), node(25)};
    state.receiver_nodes[25] = true;
    state.powered_nodes[1] = true;
    auto expected = state;
    expected.player_position = node(25);
    expected.packet.reset();
    expected.powered_nodes[25] = true;
    expected.turns_taken = TurnIndex{48};
    if (!expect_turn(state, MoveCommand{node(25)}, true, expected,
                     "last turn completes after second delivery",
                     SectorOutcome::completed))
    {
        return false;
    }

    state.player_position = node(25);
    state.player_integrity = 1;
    state.pulses = {{high_id, node(25)}, {low_id, node(24)}};
    expected.player_integrity = 0;
    expected.pulses.clear();
    if (!expect_turn(state, WaitCommand{}, true, expected,
                     "last turn death precedes exit despite delivery and cost",
                     SectorOutcome::death))
    {
        return false;
    }

    state = board();
    state.turns_taken = TurnIndex{47};
    if (!expect_turn(state, MoveCommand{node(26)}, false, state,
                     "invalid move preserves the last available turn"))
    {
        return false;
    }
    expected = state;
    expected.turns_taken = TurnIndex{48};
    return expect_turn(state, WaitCommand{}, true, expected,
                       "last turn overload is read after the turn cost",
                       SectorOutcome::overload);
}

bool terminal_sectors_reject_all_commands_and_own_the_returned_state()
{
    SplitMix64 random{RunSeed{0xfedcba98}};
    static_cast<void>(random.next_u64());
    auto reference = random;
    for (const auto outcome : {SectorOutcome::completed, SectorOutcome::death,
                               SectorOutcome::overload})
    {
        for (const auto turn : {TurnIndex{12}, TurnIndex{48}, TurnIndex{49}})
        {
            if (outcome == SectorOutcome::completed && turn == TurnIndex{49})
            {
                continue;
            }
            auto state = board();
            state.turns_taken = turn;
            state.packet.reset();
            state.sector_entry = node(24);
            state.pulses = {{high_id, node(24)}, {low_id, node(25)}};
            state.receiver_nodes[25] = true;
            if (outcome == SectorOutcome::completed)
            {
                state.sector_exit = node(24);
                state.powered_nodes[1] = true;
                state.powered_nodes[48] = true;
            }
            else if (outcome == SectorOutcome::death)
            {
                state.player_integrity = 0;
            }
            else if (turn == TurnIndex{12})
            {
                continue;
            }
            for (const auto &command :
                 std::array<Command, 3>{MoveCommand{node(25)},
                                        RotateCommand{node(24)}, WaitCommand{}})
            {
                if (closed_circuit::is_command_legal(state, command) ||
                    !expect_turn(state, command, false, state,
                                 "terminal input runs no phase or turn 49",
                                 outcome))
                {
                    return false;
                }
                const auto before = state;
                auto result = resolve_turn(state, command);
                const auto untouched = resolve_turn(state, command);
                result.state.sector_exit.reset();
                result.state.pulses.front().position = node(47);
                result.state.pulses.push_back({spare_id, node(46)});
                result.state.packet = Packet{node(0), node(1)};
                if (!same_state(state, before) ||
                    !same_state(untouched.state, before) ||
                    untouched.outcome != outcome || untouched.accepted)
                {
                    std::cerr
                        << "Terminal result must own an independent state\n";
                    return false;
                }
            }
        }
    }
    for (int draw = 0; draw < 8; ++draw)
    {
        if (random.next_u64() != reference.next_u64())
        {
            std::cerr << "Terminal rejection must not consume external RNG\n";
            return false;
        }
    }
    return true;
}

bool resolver_leaves_external_rng_sequence_untouched()
{
    SplitMix64 random{RunSeed{0x12345678}};
    static_cast<void>(random.next_u64());
    auto reference = random;
    const auto state = board();
    auto expected = state;
    expected.turns_taken = TurnIndex{8};
    if (!expect_turn(state, MoveCommand{node(26)}, false, state,
                     "rejection without access to RNG") ||
        !expect_turn(state, WaitCommand{}, true, expected,
                     "acceptance without access to RNG"))
    {
        return false;
    }
    for (int draw = 0; draw < 8; ++draw)
    {
        if (random.next_u64() != reference.next_u64())
        {
            std::cerr << "Resolver must not consume external RNG draws\n";
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        illegal_commands_run_no_phase_or_cost() &&
        each_legal_command_pays_one_turn() &&
        packet_and_pulse_move_exactly_one_edge() &&
        rotation_precedes_packet_and_pulse_routes() &&
        player_entry_is_contact_before_pulse_movement() &&
        delivery_and_turn_cost_survive_death_in_threat_phase() &&
        next_packet_waits_for_the_next_accepted_turn() &&
        accepted_and_rejected_results_own_independent_state() &&
        last_turn_classifies_after_delivery_threats_and_cost() &&
        terminal_sectors_reject_all_commands_and_own_the_returned_state() &&
        resolver_leaves_external_rng_sequence_untouched();
    return passed ? 0 : 1;
}
