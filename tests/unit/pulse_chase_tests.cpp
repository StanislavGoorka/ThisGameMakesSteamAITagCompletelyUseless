#include "core/pulse_chase.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>

using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::next_pulse_step;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::Pulse;
using closed_circuit::TurnIndex;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr auto chaser_id = *EntityId::from_value(17);
constexpr auto other_id = *EntityId::from_value(0);
constexpr auto all_ports = *PortMask::from_bits(15);

GameState board()
{
    GameState state{.player_position = node(26),
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(25), .target = node(48)},
                    .sector_entry = node(0),
                    .sector_exit = *NodeId::from_value(48),
                    .pulses = {{other_id, node(0)}, {chaser_id, node(24)}}};
    state.ports.fill(all_ports);
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[25] = true;
    state.powered_nodes[3] = true;
    state.receiver_nodes[48] = true;
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
           left.sector_exit == right.sector_exit &&
           left.pulses == right.pulses &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

bool expect_step(const GameState &state, EntityId id,
                 std::optional<NodeId> expected, std::string_view context)
{
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (next_pulse_step(state, id) != expected ||
            !same_state(state, original))
        {
            std::cerr << context << ": unexpected step or changed full state\n";
            return false;
        }
    }
    return true;
}

bool selects_one_edge_and_allows_a_player_at_the_next_node()
{
    auto state = board();
    if (!expect_step(state, chaser_id, node(25), "one edge, not the goal"))
    {
        return false;
    }
    state.player_position = node(25);
    if (!expect_step(state, chaser_id, node(25), "adjacent player"))
    {
        return false;
    }
    state.player_integrity = 0;
    state.blocked_nodes.fill(true);
    return expect_step(state, chaser_id, node(25),
                       "integrity and blocking flags do not select a route");
}

bool equal_routes_follow_north_east_south_west_order()
{
    auto state = board();
    for (const auto [goal, step] :
         {std::pair{18, 17}, std::pair{32, 25}, std::pair{30, 31},
          std::pair{16, 17}, std::pair{22, 23}})
    {
        state.player_position = node(goal);
        if (!expect_step(state, chaser_id, node(step), "direction order"))
        {
            return false;
        }
    }
    return true;
}

bool missing_pulse_unreachable_player_and_contact_mean_waiting()
{
    auto state = board();
    if (!expect_step(state, *EntityId::from_value(EntityId::max_value),
                     std::nullopt, "missing ID"))
    {
        return false;
    }
    state.pulses.clear();
    if (!expect_step(state, chaser_id, std::nullopt, "empty collection"))
    {
        return false;
    }
    state.pulses = {{chaser_id, node(0)}};
    state.player_position = node(48);
    state.ports.fill(PortMask{});
    if (!expect_step(state, chaser_id, std::nullopt, "no route"))
    {
        return false;
    }
    state.pulses.front().position = node(48);
    return expect_step(state, chaser_id, std::nullopt, "already on player");
}

bool choice_reads_current_topology_and_current_player_position()
{
    auto state = board();
    state.player_position = node(25);
    if (!expect_step(state, chaser_id, node(25), "direct route"))
    {
        return false;
    }
    state.ports[24] = *PortMask::from_bits(PortMask::north_bit);
    if (!expect_step(state, chaser_id, node(17), "changed topology"))
    {
        return false;
    }
    state.ports[24] = all_ports;
    state.player_position = node(31);
    return expect_step(state, chaser_id, node(31), "changed player position");
}

bool occupied_first_step_waits_despite_a_longer_free_route()
{
    auto state = board();
    state.ports.fill(PortMask{});
    state.ports[24] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    state.ports[25] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
    state.ports[17] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
    state.ports[18] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[19] =
        *PortMask::from_bits(PortMask::south_bit | PortMask::west_bit);
    state.pulses = {{other_id, node(25)}, {chaser_id, node(24)}};
    if (!expect_step(state, chaser_id, std::nullopt, "occupied first step"))
    {
        return false;
    }
    std::ranges::reverse(state.pulses);
    if (!expect_step(state, chaser_id, std::nullopt,
                     "storage order does not change blocking"))
    {
        return false;
    }
    std::erase_if(state.pulses,
                  [](const Pulse &pulse) { return pulse.id == other_id; });
    if (!expect_step(state, chaser_id, node(25), "blocker removed"))
    {
        return false;
    }
    state.ports[24] = *PortMask::from_bits(PortMask::north_bit);
    return expect_step(state, chaser_id, node(17),
                       "the longer free route exists");
}

} // namespace

int main()
{
    const bool passed =
        selects_one_edge_and_allows_a_player_at_the_next_node() &&
        equal_routes_follow_north_east_south_west_order() &&
        missing_pulse_unreachable_player_and_contact_mean_waiting() &&
        choice_reads_current_topology_and_current_player_position() &&
        occupied_first_step_waits_despite_a_longer_free_route();
    return passed ? 0 : 1;
}
