#include "core/pulse_movement.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>

using closed_circuit::advance_pulses;
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

constexpr auto low_id = *EntityId::from_value(0);
constexpr auto middle_id = *EntityId::from_value(17);
constexpr auto high_id = *EntityId::from_value(EntityId::max_value);

GameState board()
{
    GameState state{.player_position = node(26),
                    .player_integrity = 2,
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(25), .target = node(48)},
                    .sector_entry = node(0),
                    .sector_exit = *NodeId::from_value(48)};
    state.ports[24] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[25] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[26] = *PortMask::from_bits(PortMask::west_bit);
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[25] = true;
    state.powered_nodes[3] = true;
    state.receiver_nodes[48] = true;
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

bool expect_state(GameState state, const GameState &expected,
                  std::string_view context)
{
    advance_pulses(state);
    if (!same_state(state, expected))
    {
        std::cerr << context
                  << ": unexpected full state after pulse movement\n";
        return false;
    }
    return true;
}

bool expect_all_storage_orders(const GameState &before, const GameState &after,
                               std::string_view context)
{
    std::array order{0, 1, 2};
    do
    {
        auto state = before;
        auto expected = after;
        for (std::size_t index = 0; index < order.size(); ++index)
        {
            state.pulses[index] = before.pulses[order[index]];
            expected.pulses[index] = after.pulses[order[index]];
        }
        if (!expect_state(state, expected, context))
        {
            return false;
        }
    } while (std::ranges::next_permutation(order).found);
    return true;
}

bool one_pulse_moves_one_edge_and_contact_does_not_remove_or_damage()
{
    auto state = board();
    state.pulses = {{low_id, node(24)}};
    auto expected = state;
    expected.pulses.front().position = node(25);
    if (!expect_state(state, expected, "one edge instead of jumping to player"))
    {
        return false;
    }
    state = expected;
    expected.pulses.front().position = node(26);
    return expect_state(state, expected,
                        "contact preserves pulse and integrity");
}

bool empty_collection_no_route_and_existing_contact_preserve_state()
{
    auto state = board();
    if (!expect_state(state, state, "empty collection"))
    {
        return false;
    }
    state.pulses = {{low_id, node(0)}, {high_id, node(48)}};
    if (!expect_state(state, state, "no route from either boundary node"))
    {
        return false;
    }
    state.pulses = {{low_id, state.player_position}};
    return expect_state(state, state, "already on player");
}

bool lower_id_frees_a_node_for_a_later_pulse()
{
    auto state = board();
    state.pulses = {
        {low_id, node(25)}, {middle_id, node(0)}, {high_id, node(24)}};
    auto expected = state;
    expected.pulses[0].position = node(26);
    expected.pulses[2].position = node(25);
    return expect_all_storage_orders(state, expected,
                                     "lower ID frees the next node");
}

bool blocked_lower_id_does_not_retry_after_a_later_pulse_moves()
{
    auto state = board();
    state.pulses = {
        {low_id, node(24)}, {middle_id, node(0)}, {high_id, node(25)}};
    auto expected = state;
    expected.pulses[2].position = node(26);
    return expect_all_storage_orders(state, expected,
                                     "blocked lower ID acts only once");
}

bool lower_id_claims_a_shared_next_node()
{
    auto state = board();
    state.player_position = node(32);
    state.ports.fill(PortMask{});
    state.ports[24] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[18] = *PortMask::from_bits(PortMask::south_bit);
    state.ports[25] = *PortMask::from_bits(
        PortMask::north_bit | PortMask::south_bit | PortMask::west_bit);
    state.ports[32] = *PortMask::from_bits(PortMask::north_bit);
    state.pulses = {
        {low_id, node(24)}, {middle_id, node(0)}, {high_id, node(18)}};
    auto expected = state;
    expected.pulses[0].position = node(25);
    return expect_all_storage_orders(state, expected,
                                     "lower ID claims shared next node");
}

bool occupied_next_node_does_not_cause_a_longer_detour()
{
    auto state = board();
    state.ports[24] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::east_bit);
    state.ports[26] =
        *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
    state.ports[17] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::south_bit);
    state.ports[18] =
        *PortMask::from_bits(PortMask::east_bit | PortMask::west_bit);
    state.ports[19] =
        *PortMask::from_bits(PortMask::south_bit | PortMask::west_bit);
    state.pulses = {
        {low_id, node(26)}, {middle_id, node(25)}, {high_id, node(24)}};
    return expect_all_storage_orders(state, state,
                                     "occupied first step prevents a detour");
}

} // namespace

int main()
{
    const bool passed =
        one_pulse_moves_one_edge_and_contact_does_not_remove_or_damage() &&
        empty_collection_no_route_and_existing_contact_preserve_state() &&
        lower_id_frees_a_node_for_a_later_pulse() &&
        blocked_lower_id_does_not_retry_after_a_later_pulse_moves() &&
        lower_id_claims_a_shared_next_node() &&
        occupied_next_node_does_not_cause_a_longer_detour();
    return passed ? 0 : 1;
}
