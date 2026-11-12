#include "core/packet_delivery.hpp"
#include "core/packet_introduction.hpp"
#include "core/packet_movement.hpp"

#include <algorithm>
#include <initializer_list>
#include <iostream>

using closed_circuit::advance_packet;
using closed_circuit::deliver_packet;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::introduce_packet;
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

// Only 24-25-26 are connected. The entry is 24; receivers are 25 and 26.
GameState corridor_board()
{
    GameState state{.player_position = node(32), .turns_taken = TurnIndex{7}};
    state.ports[24] = ports(PortMask::east_bit);
    state.ports[25] = ports(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = ports(PortMask::west_bit);
    state.rotatable_nodes[25] = true;
    state.blocked_nodes[0] = true;
    state.powered_nodes[3] = true;
    state.receiver_nodes[26] = true;
    state.receiver_nodes[25] = true;
    state.sector_entry = node(24);
    state.sector_exit = *NodeId::from_value(48);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
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

bool introduces_at(GameState &state, int entry, int target)
{
    auto expected = state;
    expected.packet = Packet{.position = node(entry), .target = node(target)};
    return introduce_packet(state) && same_state(state, expected);
}

bool introduction_is_refused(GameState &state)
{
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (introduce_packet(state) || !same_state(state, original))
        {
            return false;
        }
    }
    return true;
}

bool first_packet_starts_at_entry_with_lowest_unpowered_receiver()
{
    auto state = corridor_board();
    if (!introduces_at(state, 24, 25))
    {
        std::cerr << "Introduction must change only the packet, placing it "
                     "at the entry with the lowest unpowered receiver\n";
        return false;
    }

    auto lowest_powered = corridor_board();
    lowest_powered.powered_nodes[25] = true;
    if (!introduces_at(lowest_powered, 24, 26))
    {
        std::cerr << "Introduction must skip an already powered receiver\n";
        return false;
    }
    return true;
}

bool active_packet_prevents_introduction_and_repeated_step()
{
    auto state = corridor_board();
    if (!introduces_at(state, 24, 25) || !introduction_is_refused(state))
    {
        std::cerr << "A repeated introduction must preserve the active "
                     "packet and the entire state\n";
        return false;
    }

    // An active packet away from the entry, even standing on a powered
    // target, must not be replaced or retargeted.
    state.packet = Packet{.position = node(26), .target = node(26)};
    state.powered_nodes[26] = true;
    if (!introduction_is_refused(state))
    {
        std::cerr << "An active packet must prevent introduction regardless "
                     "of its position and target\n";
        return false;
    }
    return true;
}

bool missing_entry_or_unpowered_receiver_preserves_entire_state()
{
    auto no_entry = corridor_board();
    no_entry.sector_entry.reset();
    auto no_receivers = corridor_board();
    no_receivers.receiver_nodes.fill(false);
    auto all_powered = corridor_board();
    all_powered.powered_nodes[25] = true;
    all_powered.powered_nodes[26] = true;
    if (!introduction_is_refused(no_entry) ||
        !introduction_is_refused(no_receivers) ||
        !introduction_is_refused(all_powered))
    {
        std::cerr << "Without an entry or an unpowered receiver introduction "
                     "must leave the entire state untouched\n";
        return false;
    }
    return true;
}

bool introduction_ignores_player_blocking_units_and_topology()
{
    for (const auto player : {24, 32})
    {
        for (const auto alive : {true, false})
        {
            auto state = corridor_board();
            state.player_position = node(player);
            state.player_integrity = alive ? 3 : 0;
            state.blocked_nodes[24] = true;
            state.ports.fill(ports(0));
            state.rotatable_nodes.fill(false);
            state.turns_taken = TurnIndex{0};
            if (!introduces_at(state, 24, 25))
            {
                std::cerr << "Introduction must ignore the player, blocking "
                             "units, ports and the turn count\n";
                return false;
            }
        }
    }
    return true;
}

bool moves_to(GameState &state, int position)
{
    auto expected = state;
    expected.packet->position = node(position);
    return advance_packet(state) && same_state(state, expected);
}

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

bool movement_is_refused(GameState &state)
{
    const auto original = state;
    return !advance_packet(state) && same_state(state, original);
}

bool delivery_is_refused(GameState &state)
{
    const auto original = state;
    return !deliver_packet(state) && same_state(state, original);
}

bool next_packet_waits_until_next_turn_and_moves_once_per_turn()
{
    auto state = corridor_board();
    // Turn 1: introduction, one movement, delivery. No introduction after
    // delivery in this turn, and no packet left for another movement.
    if (!introduces_at(state, 24, 25) || !moves_to(state, 25) ||
        !delivers_to(state, 25) || !movement_is_refused(state))
    {
        std::cerr << "The first turn must deliver only the first packet "
                     "and leave no packet for further movement\n";
        return false;
    }

    // Turn 2: the new packet starts at the entry and moves just to 25,
    // keeping 26 as its target. It cannot be delivered yet.
    if (!introduces_at(state, 24, 26) || !moves_to(state, 25) ||
        !delivery_is_refused(state))
    {
        std::cerr << "On the next turn the second packet must move exactly "
                     "one edge from the entry and remain undelivered\n";
        return false;
    }

    // Turn 3: the active packet prevents introduction, reaches 26 and is
    // delivered. All receivers are powered; no more packets may enter.
    if (!introduction_is_refused(state) || !moves_to(state, 26) ||
        !delivers_to(state, 26) || !movement_is_refused(state) ||
        !introduction_is_refused(state))
    {
        std::cerr << "The third turn must deliver the second packet, keep "
                     "the first receiver powered and prevent more packets\n";
        return false;
    }
    return true;
}

bool entry_on_target_leaves_delivery_to_its_own_step()
{
    auto state = corridor_board();
    state.sector_entry = node(25);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
    if (!introduces_at(state, 25, 25) || !movement_is_refused(state) ||
        !delivers_to(state, 25) || !movement_is_refused(state))
    {
        std::cerr << "A packet introduced on its target must wait for the "
                     "separate delivery step without moving\n";
        return false;
    }
    return true;
}

bool extreme_identifiers_are_valid_entries()
{
    for (const auto entry : {0, 48})
    {
        auto state = corridor_board();
        state.sector_entry = node(entry);
        if (!introduces_at(state, entry, 25))
        {
            std::cerr << "Entries on nodes 0 and 48 must work like any "
                         "other entry\n";
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        first_packet_starts_at_entry_with_lowest_unpowered_receiver() &&
        active_packet_prevents_introduction_and_repeated_step() &&
        missing_entry_or_unpowered_receiver_preserves_entire_state() &&
        introduction_ignores_player_blocking_units_and_topology() &&
        next_packet_waits_until_next_turn_and_moves_once_per_turn() &&
        entry_on_target_leaves_delivery_to_its_own_step() &&
        extreme_identifiers_are_valid_entries();
    return passed ? 0 : 1;
}
