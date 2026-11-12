#include "core/splitmix64.hpp"
#include "core/turn_preview.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>

using namespace closed_circuit;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr auto spare_id = *EntityId::from_value(7);
constexpr auto high_id = *EntityId::from_value(EntityId::max_value);

GameState board()
{
    GameState state{.player_position = node(24),
                    .player_integrity = 2,
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{node(8), node(1)},
                    .sector_entry = node(8),
                    .sector_exit = node(48),
                    .pulses = {{spare_id, node(48)}, {high_id, node(0)}}};
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
    return left.terminal_rotation_used == right.terminal_rotation_used &&
           left.modules == right.modules && left.reserve == right.reserve &&
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

bool same_result(const TurnResult &left, const TurnResult &right)
{
    return left.accepted == right.accepted && left.outcome == right.outcome &&
           same_state(left.state, right.state);
}

bool expect_preview(const GameState &state, const Command &command,
                    bool accepted, const GameState &expected,
                    std::string_view context,
                    SectorOutcome outcome = SectorOutcome::ongoing)
{
    const auto before = state;
    const auto command_before = command;
    const TurnResult expected_result{expected, accepted, outcome};
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_turn(state, command);
        if (!same_state(state, before) || command != command_before)
        {
            std::cerr << context
                      << ": preview modified live state or command\n";
            return false;
        }
        const auto committed = resolve_turn(state, command);
        if (!same_state(state, before) || command != command_before ||
            !same_result(preview, committed) ||
            !same_result(preview, expected_result))
        {
            std::cerr
                << context
                << ": preview must match commit and explicit expectation\n";
            return false;
        }
    }
    return true;
}

bool basic_commands_and_rejections()
{
    const auto state = board();
    auto expected = state;
    expected.turns_taken = TurnIndex{8};
    if (!expect_preview(state, WaitCommand{}, true, expected, "wait"))
    {
        return false;
    }
    expected.player_position = node(25);
    if (!expect_preview(state, MoveCommand{node(25)}, true, expected, "move"))
    {
        return false;
    }
    expected = state;
    expected.turns_taken = TurnIndex{8};
    expected.ports[24] = *PortMask::from_bits(PortMask::south_bit);
    if (!expect_preview(state, RotateCommand{node(24)}, true, expected,
                        "rotate"))
    {
        return false;
    }
    auto ready = state;
    ready.packet.reset();
    ready.sector_entry = node(24);
    ready.pulses = {{high_id, node(24)}, {spare_id, node(25)}};
    for (const auto &command :
         std::array<Command, 2>{MoveCommand{node(26)}, RotateCommand{node(26)}})
    {
        if (!expect_preview(ready, command, false, ready,
                            "invalid command runs no ready phase"))
        {
            return false;
        }
    }
    return true;
}

bool introduction_delivery_and_movement()
{
    auto state = board();
    state.packet.reset();
    state.sector_entry = node(0);
    state.ports[0] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[1] = *PortMask::from_bits(PortMask::west_bit);
    auto expected = state;
    expected.powered_nodes[1] = true;
    ++expected.reserve;
    expected.turns_taken = TurnIndex{8};
    if (!expect_preview(state, WaitCommand{}, true, expected,
                        "packet introduced and delivered"))
    {
        return false;
    }

    state = board();
    state.player_position = node(25);
    state.ports[22] = *PortMask::from_bits(PortMask::east_bit);
    for (int index = 23; index < 26; ++index)
    {
        state.ports[index] =
            *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    }
    state.ports[26] = *PortMask::from_bits(PortMask::west_bit);
    state.packet = Packet{node(22), node(26)};
    state.receiver_nodes[26] = true;
    state.pulses = {{high_id, node(22)}, {spare_id, node(48)}};
    expected = state;
    expected.packet->position = node(23);
    expected.pulses.front().position = node(23);
    expected.turns_taken = TurnIndex{8};
    return expect_preview(state, WaitCommand{}, true, expected,
                          "packet and pulse move once");
}

bool rotation_changes_routes_and_contacts()
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
    if (!expect_preview(state, RotateCommand{node(24)}, true, expected,
                        "rotation closes delivery route and admits pulse"))
    {
        return false;
    }
    state = board();
    state.pulses = {{spare_id, node(48)}, {high_id, node(25)}};
    expected = state;
    expected.player_position = node(25);
    expected.player_integrity = 1;
    expected.pulses = {{spare_id, node(48)}};
    expected.turns_taken = TurnIndex{8};
    return expect_preview(state, MoveCommand{node(25)}, true, expected,
                          "player enters pulse");
}

bool last_turn_completion_death_and_overload()
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
    ++expected.reserve;
    expected.turns_taken = TurnIndex{48};
    if (!expect_preview(state, MoveCommand{node(25)}, true, expected,
                        "last turn completes after delivery",
                        SectorOutcome::completed))
    {
        return false;
    }
    state.player_integrity = 1;
    state.pulses = {{high_id, node(25)}, {spare_id, node(48)}};
    expected.player_integrity = 0;
    expected.pulses = {{spare_id, node(48)}};
    if (!expect_preview(state, MoveCommand{node(25)}, true, expected,
                        "death precedes completion despite delivery and cost",
                        SectorOutcome::death))
    {
        return false;
    }
    state = board();
    state.turns_taken = TurnIndex{47};
    expected = state;
    expected.turns_taken = TurnIndex{48};
    return expect_preview(state, WaitCommand{}, true, expected,
                          "last turn overload", SectorOutcome::overload);
}

bool terminal_states_reject_every_command()
{
    for (const auto outcome : {SectorOutcome::completed, SectorOutcome::death,
                               SectorOutcome::overload})
    {
        auto state = board();
        state.packet.reset();
        state.sector_entry = node(24);
        state.pulses = {{high_id, node(24)}, {spare_id, node(25)}};
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
        else
        {
            state.turns_taken = TurnIndex{48};
        }
        for (const auto &command : std::array<Command, 3>{
                 MoveCommand{node(25)}, RotateCommand{node(24)}, WaitCommand{}})
        {
            if (!expect_preview(state, command, false, state,
                                "terminal input preserves ready phases",
                                outcome))
            {
                return false;
            }
        }
    }
    return true;
}

bool preview_results_own_their_state_and_leave_rng_outside_api()
{
    SplitMix64 random{RunSeed{0x12345678}};
    static_cast<void>(random.next_u64());
    auto reference = random;
    const auto state = board();
    const auto before = state;
    for (const auto &command :
         std::array<Command, 2>{WaitCommand{}, MoveCommand{node(26)}})
    {
        const auto command_before = command;
        auto changed = preview_turn(state, command);
        const auto untouched = preview_turn(state, command);
        changed.state.pulses.front().id = high_id;
        changed.state.pulses.front().position = node(47);
        changed.state.pulses.push_back({*EntityId::from_value(99), node(46)});
        changed.state.packet->position = node(9);
        changed.state.packet->target = node(10);
        changed.state.sector_exit.reset();
        changed.state.powered_nodes[3] = false;
        const auto next = preview_turn(state, command);
        const auto committed = resolve_turn(state, command);
        if (!same_state(state, before) || command != command_before ||
            !same_result(untouched, next) || !same_result(next, committed))
        {
            std::cerr << "Preview results must own independent state\n";
            return false;
        }
    }
    for (int draw = 0; draw < 8; ++draw)
    {
        if (random.next_u64() != reference.next_u64())
        {
            std::cerr << "Preview must not consume external RNG draws\n";
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    return basic_commands_and_rejections() &&
                   introduction_delivery_and_movement() &&
                   rotation_changes_routes_and_contacts() &&
                   last_turn_completion_death_and_overload() &&
                   terminal_states_reject_every_command() &&
                   preview_results_own_their_state_and_leave_rng_outside_api()
               ? 0
               : 1;
}
