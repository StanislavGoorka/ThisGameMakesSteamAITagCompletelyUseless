#include "../../tools/replay/state_hash.hpp"
#include "../support/state_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/packet_delivery.hpp"
#include "core/packet_movement.hpp"
#include "core/turn_preview.hpp"

#include <algorithm>
#include <iostream>

using closed_circuit::advance_packet;
using closed_circuit::deliver_packet;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::TurnIndex;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr PortMask ports(int bits)
{
    return *PortMask::from_bits(bits);
}

// Only 24-25-26 are connected. The player stands on a node without ports.
GameState corridor_board(int packet, int target)
{
    GameState state{
        .player_position = node(32),
        .turns_taken = TurnIndex{7},
        .packet = Packet{.position = node(packet), .target = node(target)}};
    state.ports[24] = ports(PortMask::east_bit);
    state.ports[25] = ports(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = ports(PortMask::west_bit);
    state.rotatable_nodes[25] = true;
    state.blocked_nodes[0] = true;
    state.sector_entry = node(24);
    state.sector_exit = *NodeId::from_value(48);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
    state.reserve = 0;
    state.overload_debt = 17;
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    if (left.first_overdrive_used != right.first_overdrive_used ||
        left.delivery_requirement != right.delivery_requirement)
        return false;
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

// The step must remove the packet, power exactly the given target and change
// nothing else, including the turn count.
bool delivers_to(GameState &state, int target)
{
    auto expected = state;
    if (expected.reserve < 3)
    {
        ++expected.reserve;
    }
    expected.packet.reset();
    expected.powered_nodes[node(target).value()] = true;
    return deliver_packet(state) && same_state(state, expected);
}

bool delivery_is_refused(GameState &state)
{
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (deliver_packet(state) || !same_state(state, original))
        {
            return false;
        }
    }
    return true;
}

bool packet_on_target_is_delivered_exactly_once()
{
    auto state = corridor_board(26, 26);
    if (!delivers_to(state, 26))
    {
        std::cerr << "A packet on its target must leave the board and power "
                     "only that target without changing anything else\n";
        return false;
    }
    if (!delivery_is_refused(state))
    {
        std::cerr << "A repeated step after the delivery must change "
                     "nothing\n";
        return false;
    }
    return true;
}

bool packet_off_target_is_not_delivered()
{
    auto distant = corridor_board(24, 26);
    auto adjacent = corridor_board(25, 26);
    // The packet stands on a powered node that is not its target.
    auto on_powered_node = corridor_board(25, 26);
    on_powered_node.powered_nodes[25] = true;
    if (!delivery_is_refused(distant) || !delivery_is_refused(adjacent) ||
        !delivery_is_refused(on_powered_node))
    {
        std::cerr << "A packet off its target must stay on the board and "
                     "leave the state untouched\n";
        return false;
    }
    return true;
}

bool state_without_packet_stays_untouched()
{
    auto empty = corridor_board(26, 26);
    empty.packet.reset();
    auto empty_with_record = empty;
    empty_with_record.powered_nodes[26] = true;
    if (!delivery_is_refused(empty) || !delivery_is_refused(empty_with_record))
    {
        std::cerr << "A state without a packet must stay untouched\n";
        return false;
    }
    return true;
}

bool packet_arriving_by_route_is_delivered_and_leaves_the_board()
{
    auto state = corridor_board(24, 26);
    if (!delivery_is_refused(state) || !advance_packet(state) ||
        !delivery_is_refused(state) || !advance_packet(state))
    {
        std::cerr << "A packet must reach its target by its own steps and "
                     "stay undelivered on the way\n";
        return false;
    }
    if (!delivers_to(state, 26))
    {
        std::cerr << "A packet that reached its target must be delivered\n";
        return false;
    }

    const auto delivered = state;
    if (advance_packet(state) || !same_state(state, delivered) || state.packet)
    {
        std::cerr << "After the delivery the board must stay without a "
                     "packet and the packet step must do nothing\n";
        return false;
    }
    return true;
}

bool second_delivery_keeps_the_first_target_powered()
{
    auto state = corridor_board(26, 26);
    if (!delivers_to(state, 26))
    {
        std::cerr << "The first packet must be delivered\n";
        return false;
    }

    state.packet = Packet{.position = node(26), .target = node(24)};
    if (!advance_packet(state) || !advance_packet(state) ||
        !delivers_to(state, 24))
    {
        std::cerr << "A second packet must be delivered to another target\n";
        return false;
    }
    if (!state.powered_nodes[26] || !state.powered_nodes[24] ||
        std::ranges::count(state.powered_nodes, true) != 2)
    {
        std::cerr << "Two deliveries must leave exactly the two targets "
                     "powered\n";
        return false;
    }
    return true;
}

bool packet_on_powered_target_is_not_delivered_again()
{
    auto state = corridor_board(26, 26);
    state.powered_nodes[26] = true;
    if (!delivery_is_refused(state))
    {
        std::cerr << "A target must be powered at most once: a packet on a "
                     "powered target must stay and leave the state "
                     "untouched\n";
        return false;
    }
    return true;
}

bool delivery_ignores_player_and_blocking_units()
{
    auto state = corridor_board(26, 26);
    state.player_position = node(26);
    state.player_integrity = 0;
    state.blocked_nodes[26] = true;
    if (!delivers_to(state, 26))
    {
        std::cerr << "Delivery must not depend on the player or on blocking "
                     "units and must not change them\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    for (const bool installed : {false, true})
        for (const bool paired : {false, true})
            for (const unsigned reserve : {0U, 1U, 2U, 3U})
            {
                auto state = corridor_board(26, 26);
                state.reserve = static_cast<std::uint8_t>(reserve);
                if (installed &&
                    !state.modules.add(
                        closed_circuit::ModuleId::delivery_reserve))
                    return 2;
                if (paired && !state.modules.add(
                                  closed_circuit::ModuleId::rotation_range))
                    return 2;
                const auto before = state;
                auto expected = state;
                expected.reserve = static_cast<std::uint8_t>(
                    std::min(3U, reserve + (installed ? 2U : 1U)));
                expected.packet.reset();
                expected.powered_nodes[26] = true;
                if (!deliver_packet(state) ||
                    !closed_circuit::test::same_state(state, expected))
                    return 3;
                const auto delivered = state;
                if (deliver_packet(state) ||
                    !closed_circuit::test::same_state(state, delivered))
                    return 4;
                state.packet = Packet{node(26), node(26)};
                const auto duplicate = state;
                if (deliver_packet(state) ||
                    !closed_circuit::test::same_state(state, duplicate))
                    return 5;
                auto turn_expected = expected;
                turn_expected.turns_taken = TurnIndex{8};
                const auto preview = closed_circuit::preview_turn(
                    before, closed_circuit::WaitCommand{});
                const auto commit = closed_circuit::resolve_turn(
                    before, closed_circuit::WaitCommand{});
                const auto refused = closed_circuit::resolve_turn(
                    before, closed_circuit::MoveCommand{node(48)});
                if (!preview.accepted || !commit.accepted || refused.accepted ||
                    !closed_circuit::test::same_state(refused.state, before) ||
                    !closed_circuit::test::same_state(preview.state,
                                                      turn_expected) ||
                    !closed_circuit::test::same_state(commit.state,
                                                      turn_expected))
                    return 6;
                closed_circuit::RunCheckpoint cp{
                    closed_circuit::RunSeed{42},
                    *closed_circuit::SectorNumber::from_value(1),
                    3,
                    2,
                    0,
                    before.modules,
                    false};
                const auto bytes = closed_circuit::encode_checkpoint(cp);
                if (!bytes || closed_circuit::decode_checkpoint(*bytes) != cp)
                    return 7;
                auto without = before;
                without.modules = {};
                if (installed &&
                    closed_circuit::developer::state_hash(without) ==
                        closed_circuit::developer::state_hash(before))
                    return 8;
            }
    for (const auto reserve : {0, 1, 2, 3})
    {
        auto state = corridor_board(26, 26);
        state.reserve = static_cast<std::uint8_t>(reserve);
        state.player_integrity = 1;
        if (!delivers_to(state, 26) ||
            state.reserve != (reserve < 3 ? reserve + 1 : 3) ||
            !delivery_is_refused(state))
        {
            return 1;
        }
        state.packet = Packet{node(26), node(26)};
        if (!delivery_is_refused(state))
        {
            return 1;
        }
    }
    const bool passed =
        packet_on_target_is_delivered_exactly_once() &&
        packet_off_target_is_not_delivered() &&
        state_without_packet_stays_untouched() &&
        packet_arriving_by_route_is_delivered_and_leaves_the_board() &&
        second_delivery_keeps_the_first_target_powered() &&
        packet_on_powered_target_is_not_delivered_again() &&
        delivery_ignores_player_and_blocking_units();
    return passed ? 0 : 1;
}
