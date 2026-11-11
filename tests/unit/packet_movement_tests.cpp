#include "core/packet_movement.hpp"
#include "core/player_movement.hpp"
#include "core/player_rotation.hpp"

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iostream>

using closed_circuit::advance_packet;
using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::move_player;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::rotate_from_player;
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

// Only 24-25-26 are connected. The player stands below the rotatable node 25
// on a node without ports.
GameState corridor_board()
{
    GameState state{.player_position = node(32),
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(24), .target = node(26)}};
    state.ports[24] = ports(PortMask::east_bit);
    state.ports[25] = ports(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = ports(PortMask::west_bit);
    state.rotatable_nodes[25] = true;
    state.blocked_nodes[0] = true;
    state.sector_entry = node(24);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
    return state;
}

// Adds the longer route 24-17-18-19-26 around node 25.
GameState detour_board()
{
    auto state = corridor_board();
    state.ports[24] = ports(PortMask::north_bit | PortMask::east_bit);
    state.ports[17] = ports(PortMask::east_bit | PortMask::south_bit);
    state.ports[18] = ports(PortMask::east_bit | PortMask::west_bit);
    state.ports[19] = ports(PortMask::south_bit | PortMask::west_bit);
    state.ports[26] = ports(PortMask::north_bit | PortMask::west_bit);
    return state;
}

GameState open_board(int player, int packet, int target)
{
    GameState state{
        .player_position = node(player),
        .turns_taken = TurnIndex{7},
        .packet = Packet{.position = node(packet), .target = node(target)}};
    state.ports.fill(ports(15));
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[0] = true;
    state.sector_entry = node(24);
    state.pulses = {{*EntityId::from_value(11), *NodeId::from_value(48)}};
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.powered_nodes == right.powered_nodes &&
           left.receiver_nodes == right.receiver_nodes &&
           left.sector_entry == right.sector_entry &&
           left.pulses == right.pulses &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

// Every step must put the packet on the next listed node and change nothing
// else, including the turn count and the target.
bool packet_follows(GameState &state, std::initializer_list<int> steps)
{
    for (const auto step : steps)
    {
        auto expected = state;
        expected.packet->position = node(step);
        if (!advance_packet(state) || !same_state(state, expected))
        {
            return false;
        }
    }
    return true;
}

bool packet_waits(GameState &state)
{
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (advance_packet(state) || !same_state(state, original))
        {
            return false;
        }
    }
    return true;
}

bool packet_advances_one_edge_per_step_and_stays_on_target()
{
    auto state = corridor_board();
    if (!packet_follows(state, {25, 26}))
    {
        std::cerr << "Each step must move the packet exactly one edge along "
                     "the shortest route and change nothing else\n";
        return false;
    }
    if (!packet_waits(state))
    {
        std::cerr << "A packet on its target must stay there with its target "
                     "and leave the state untouched\n";
        return false;
    }
    return true;
}

bool equal_routes_follow_direction_order()
{
    struct Case
    {
        int target;
        int first_step;
    };
    constexpr std::array cases{
        Case{16, 17}, Case{18, 17}, // North before west and east.
        Case{32, 25},               // East before south.
        Case{30, 31},               // South before west.
        Case{10, 17}, Case{26, 25}, Case{38, 31}, Case{22, 23}};
    for (const auto item : cases)
    {
        auto state = open_board(0, 24, item.target);
        if (!packet_follows(state, {item.first_step}))
        {
            std::cerr << "Equal routes to " << item.target
                      << " must resolve in N, E, S, W order\n";
            return false;
        }
    }
    return true;
}

bool rotation_changes_the_chosen_route()
{
    auto direct = detour_board();
    auto rotated = direct;
    if (!rotate_from_player(rotated, node(25)))
    {
        std::cerr << "The player must be able to rotate the junction\n";
        return false;
    }

    if (!packet_follows(direct, {25, 26}))
    {
        std::cerr << "Before the rotation the packet must take the short "
                     "route through the junction\n";
        return false;
    }
    if (!packet_follows(rotated, {17, 18, 19, 26}))
    {
        std::cerr << "After the rotation the packet must take the remaining "
                     "route around the junction\n";
        return false;
    }
    return true;
}

bool packet_without_route_waits_and_keeps_target()
{
    auto disconnected = corridor_board();
    disconnected.ports.fill(PortMask{});
    auto one_way = corridor_board();
    one_way.ports[25] = ports(PortMask::east_bit);
    auto partial = corridor_board();
    partial.ports[26] = PortMask{};
    if (!packet_waits(disconnected) || !packet_waits(one_way) ||
        !packet_waits(partial))
    {
        std::cerr << "A packet without a complete route must wait: it must "
                     "not vanish, move, retarget or change any other state\n";
        return false;
    }

    // The junction first cuts the corridor ahead of the packet and later
    // under it; each time one more rotation reopens the route.
    auto state = corridor_board();
    state.ports[25] = ports(PortMask::north_bit | PortMask::south_bit);
    if (!packet_waits(state) || !rotate_from_player(state, node(25)) ||
        !packet_follows(state, {25}) || !rotate_from_player(state, node(25)) ||
        !packet_waits(state) || !rotate_from_player(state, node(25)) ||
        !packet_follows(state, {26}))
    {
        std::cerr << "A waiting packet must resume towards the same target "
                     "once a rotation restores the route\n";
        return false;
    }
    return true;
}

bool state_without_packet_stays_untouched()
{
    auto state = open_board(0, 24, 26);
    state.packet.reset();
    const auto original = state;
    if (advance_packet(state) || !same_state(state, original))
    {
        std::cerr << "A state without a packet must stay untouched\n";
        return false;
    }
    return true;
}

bool packet_and_player_share_nodes_without_blocking()
{
    auto state = open_board(24, 25, 23);
    auto moved = state;
    auto expected = state;
    expected.player_position = node(25);
    if (!move_player(moved, node(25)) || !same_state(moved, expected))
    {
        std::cerr << "The player must be able to enter the packet node and "
                     "change only the player position\n";
        return false;
    }

    // The shortest route leads through the player node.
    if (!packet_follows(state, {24, 23}))
    {
        std::cerr << "The packet must pass through the player node without "
                     "marking any node as blocked\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        packet_advances_one_edge_per_step_and_stays_on_target() &&
        equal_routes_follow_direction_order() &&
        rotation_changes_the_chosen_route() &&
        packet_without_route_waits_and_keeps_target() &&
        state_without_packet_stays_untouched() &&
        packet_and_player_share_nodes_without_blocking();
    return passed ? 0 : 1;
}
