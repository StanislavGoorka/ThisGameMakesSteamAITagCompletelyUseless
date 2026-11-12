#include "core/packet_delivery.hpp"
#include "core/packet_movement.hpp"
#include "core/packet_target.hpp"

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <optional>

using closed_circuit::advance_packet;
using closed_circuit::deliver_packet;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::next_packet_target;
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

// Only 24-25-26 are connected. The board has no packet; receivers are set in
// the listed order.
GameState board_with_receivers(std::initializer_list<int> receivers)
{
    GameState state{.player_position = node(32), .turns_taken = TurnIndex{7}};
    state.ports[24] = ports(PortMask::east_bit);
    state.ports[25] = ports(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = ports(PortMask::west_bit);
    state.rotatable_nodes[25] = true;
    state.blocked_nodes[0] = true;
    state.sector_entry = node(24);
    state.sector_exit = *NodeId::from_value(48);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
    for (const auto receiver : receivers)
    {
        state.receiver_nodes[node(receiver).value()] = true;
    }
    state.reserve = 0;
    state.overload_debt = 17;
    return state;
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

// Repeated calls must report the same expected target and leave the state
// untouched.
bool target_is(const GameState &state, std::optional<NodeId> expected)
{
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (next_packet_target(state) != expected ||
            !same_state(state, original))
        {
            return false;
        }
    }
    return true;
}

bool lowest_receiver_is_the_target_in_any_arrangement()
{
    // Node 6 ends the first row and node 7 starts the second one.
    if (!target_is(board_with_receivers({6, 7}), node(6)) ||
        !target_is(board_with_receivers({7, 6}), node(6)) ||
        !target_is(board_with_receivers({20, 21}), node(20)) ||
        !target_is(board_with_receivers({41, 3}), node(3)) ||
        !target_is(board_with_receivers({30, 12, 44}), node(12)) ||
        !target_is(board_with_receivers({33}), node(33)))
    {
        std::cerr << "The target must be the receiver with the lowest NodeId, "
                     "whatever the layout and the order of the receivers\n";
        return false;
    }
    return true;
}

bool powered_receivers_are_skipped()
{
    auto state = board_with_receivers({6, 7, 30});
    state.powered_nodes[7] = true;
    if (!target_is(state, node(6)))
    {
        std::cerr << "A powered higher receiver must not hide a lower "
                     "unpowered one\n";
        return false;
    }

    state.powered_nodes[7] = false;
    state.powered_nodes[6] = true;
    if (!target_is(state, node(7)))
    {
        std::cerr << "A powered lowest receiver must be skipped\n";
        return false;
    }

    state.powered_nodes[7] = true;
    if (!target_is(state, node(30)))
    {
        std::cerr << "Every powered receiver must be skipped\n";
        return false;
    }
    return true;
}

bool no_target_without_receivers_or_with_all_powered()
{
    const auto empty = board_with_receivers({});
    auto empty_with_record = empty;
    empty_with_record.powered_nodes[6] = true;
    auto all_powered = board_with_receivers({6, 7});
    all_powered.powered_nodes[6] = true;
    all_powered.powered_nodes[7] = true;
    if (!target_is(empty, std::nullopt) ||
        !target_is(empty_with_record, std::nullopt) ||
        !target_is(all_powered, std::nullopt))
    {
        std::cerr << "There must be no target without receivers and once "
                     "every receiver is powered\n";
        return false;
    }
    return true;
}

bool target_depends_on_receivers_and_their_power_only()
{
    auto state = board_with_receivers({24, 26});
    // Every node that is not a receiver is powered.
    state.powered_nodes.fill(true);
    state.powered_nodes[24] = false;
    state.powered_nodes[26] = false;
    if (!target_is(state, node(24)))
    {
        std::cerr << "A powered node that is not a receiver must not change "
                     "the target\n";
        return false;
    }

    state.packet = Packet{.position = node(26), .target = node(26)};
    if (!target_is(state, node(24)))
    {
        std::cerr << "An active packet must not change the target\n";
        return false;
    }

    // An undelivered packet standing on the target does not power it.
    state.packet = Packet{.position = node(24), .target = node(24)};
    state.player_position = node(24);
    state.player_integrity = 0;
    state.blocked_nodes[24] = true;
    state.ports.fill(ports(0));
    state.rotatable_nodes.fill(false);
    state.turns_taken = TurnIndex{0};
    if (!target_is(state, node(24)))
    {
        std::cerr << "The target must not depend on the packet, the player, "
                     "blocking units, ports or the turn count\n";
        return false;
    }
    return true;
}

bool deliveries_move_the_target_until_no_receiver_is_left()
{
    auto state = board_with_receivers({24, 26});
    const auto first = next_packet_target(state);
    if (first != node(24))
    {
        std::cerr << "The first target must be the lower receiver\n";
        return false;
    }

    state.packet = Packet{.position = node(26), .target = *first};
    if (!advance_packet(state) || !target_is(state, node(24)) ||
        !advance_packet(state) || !target_is(state, node(24)) ||
        !deliver_packet(state) || state.packet)
    {
        std::cerr << "A packet sent to the first target must be delivered "
                     "there, and the target must stay until the delivery\n";
        return false;
    }

    const auto second = next_packet_target(state);
    if (second != node(26) || !target_is(state, node(26)))
    {
        std::cerr << "After the first delivery the target must be the second "
                     "receiver\n";
        return false;
    }

    state.packet = Packet{.position = node(24), .target = *second};
    if (!advance_packet(state) || !advance_packet(state) ||
        !deliver_packet(state) || state.packet)
    {
        std::cerr << "A packet sent to the second target must be delivered "
                     "there\n";
        return false;
    }
    if (!target_is(state, std::nullopt) ||
        std::ranges::count(state.powered_nodes, true) != 2)
    {
        std::cerr << "After both deliveries there must be no target and "
                     "exactly two powered nodes\n";
        return false;
    }
    return true;
}

bool extreme_identifiers_are_valid_targets()
{
    auto both = board_with_receivers({48, 0});
    auto lowest_powered = both;
    lowest_powered.powered_nodes[0] = true;
    auto both_powered = lowest_powered;
    both_powered.powered_nodes[48] = true;
    // Node 0 is powered without being a receiver.
    auto only_highest = board_with_receivers({48});
    only_highest.powered_nodes[0] = true;
    if (!target_is(board_with_receivers({0}), node(0)) ||
        !target_is(only_highest, node(48)) || !target_is(both, node(0)) ||
        !target_is(lowest_powered, node(48)) ||
        !target_is(both_powered, std::nullopt))
    {
        std::cerr << "Receivers on nodes 0 and 48 must be targeted in order "
                     "like any other receiver\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        lowest_receiver_is_the_target_in_any_arrangement() &&
        powered_receivers_are_skipped() &&
        no_target_without_receivers_or_with_all_powered() &&
        target_depends_on_receivers_and_their_power_only() &&
        deliveries_move_the_target_until_no_receiver_is_left() &&
        extreme_identifiers_are_valid_targets();
    return passed ? 0 : 1;
}
