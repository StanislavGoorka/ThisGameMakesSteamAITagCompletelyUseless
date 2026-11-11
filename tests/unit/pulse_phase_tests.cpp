#include "core/player_movement.hpp"
#include "core/pulse_phase.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string_view>
#include <vector>

using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::move_player;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::resolve_pulse_phase;
using closed_circuit::TurnIndex;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

constexpr auto low_id = *EntityId::from_value(0);
constexpr auto spare_id = *EntityId::from_value(8);
constexpr auto middle_id = *EntityId::from_value(17);
constexpr auto high_id = *EntityId::from_value(EntityId::max_value);

GameState board()
{
    GameState state{.player_position = node(26),
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(26), .target = node(48)},
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

bool expect_phase(GameState state, const GameState &expected,
                  std::string_view context)
{
    resolve_pulse_phase(state);
    if (!same_state(state, expected))
    {
        std::cerr << context << ": unexpected full state after pulse phase\n";
        return false;
    }
    return true;
}

bool expect_all_storage_orders(const GameState &before, const GameState &after,
                               std::string_view context)
{
    std::vector<std::size_t> order(before.pulses.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    do
    {
        auto state = before;
        auto expected = after;
        expected.pulses.clear();
        for (std::size_t index = 0; index < order.size(); ++index)
        {
            const auto pulse = before.pulses[order[index]];
            state.pulses[index] = pulse;
            const auto survivor = std::ranges::find(after.pulses, pulse.id,
                                                    &closed_circuit::Pulse::id);
            if (survivor != after.pulses.end())
            {
                expected.pulses.push_back(*survivor);
            }
        }
        if (!expect_phase(state, expected, context))
        {
            return false;
        }
    } while (std::ranges::next_permutation(order).found);
    return true;
}

bool empty_and_unreachable_pulses_preserve_state()
{
    auto state = board();
    if (!expect_phase(state, state, "empty collection"))
    {
        return false;
    }
    state.pulses = {{low_id, node(0)}, {high_id, node(48)}};
    return expect_all_storage_orders(state, state, "no route or contact");
}

bool single_pulse_moves_one_edge_then_contacts_on_a_later_phase()
{
    auto state = board();
    state.pulses = {{low_id, node(24)}};
    auto expected = state;
    expected.pulses.front().position = node(25);
    if (!expect_phase(state, expected, "one edge only"))
    {
        return false;
    }
    state = expected;
    expected.pulses.clear();
    expected.player_integrity = 2;
    return expect_phase(state, expected, "arrival removes the pulse");
}

bool high_id_contact_disappears_before_any_movement_and_shares_damage_limit()
{
    auto state = board();
    state.pulses = {{low_id, node(25)},
                    {spare_id, node(48)},
                    {middle_id, node(24)},
                    {high_id, node(26)}};
    auto expected = state;
    expected.pulses = {{spare_id, node(48)}, {middle_id, node(25)}};
    expected.player_integrity = 2;
    return expect_all_storage_orders(state, expected,
                                     "high ID contact precedes every move");
}

bool successive_arrivals_disappear_immediately_even_after_death()
{
    for (const auto integrity :
         {std::uint8_t{3}, std::uint8_t{1}, std::uint8_t{0}})
    {
        auto state = board();
        state.player_integrity = integrity;
        state.ports[19] = *PortMask::from_bits(PortMask::south_bit);
        state.ports[26] =
            *PortMask::from_bits(PortMask::north_bit | PortMask::west_bit);
        state.pulses = {{low_id, node(25)},
                        {spare_id, node(0)},
                        {middle_id, node(19)},
                        {high_id, node(48)}};
        auto expected = state;
        expected.pulses = {{spare_id, node(0)}, {high_id, node(48)}};
        expected.player_integrity =
            integrity > 0 ? static_cast<std::uint8_t>(integrity - 1) : 0;
        if (!expect_all_storage_orders(state, expected,
                                       "successive arrivals free player node"))
        {
            return false;
        }
    }
    return true;
}

bool legal_player_entry_contacts_before_phase_movement()
{
    auto state = board();
    state.player_position = node(25);
    state.pulses = {{high_id, node(26)}, {low_id, node(0)}};
    auto expected = state;
    expected.player_position = node(26);
    expected.player_integrity = 2;
    expected.pulses = {{low_id, node(0)}};
    if (!move_player(state, node(26)))
    {
        std::cerr << "Player entry onto a pulse must remain legal\n";
        return false;
    }
    return expect_phase(state, expected, "contact after player entry");
}

bool each_phase_has_a_fresh_damage_limit()
{
    auto state = board();
    state.pulses = {{low_id, node(25)}, {high_id, node(24)}};
    auto expected = state;
    expected.player_integrity = 2;
    expected.pulses = {{high_id, node(25)}};
    resolve_pulse_phase(state);
    if (!same_state(state, expected))
    {
        std::cerr << "First phase must remove contact and advance later ID\n";
        return false;
    }
    expected.player_integrity = 1;
    expected.pulses.clear();
    return expect_phase(state, expected, "fresh limit on next phase");
}

bool blocked_lower_id_does_not_retry_after_later_contact()
{
    auto state = board();
    state.pulses = {
        {low_id, node(24)}, {middle_id, node(0)}, {high_id, node(25)}};
    auto expected = state;
    expected.player_integrity = 2;
    expected.pulses = {{low_id, node(24)}, {middle_id, node(0)}};
    return expect_all_storage_orders(state, expected,
                                     "waiting pulse does not retry");
}

bool occupied_first_step_waits_despite_a_longer_detour()
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
        {low_id, node(24)}, {middle_id, node(0)}, {high_id, node(25)}};
    auto expected = state;
    expected.player_integrity = 2;
    expected.pulses = {{low_id, node(24)}, {middle_id, node(0)}};
    return expect_all_storage_orders(state, expected,
                                     "occupied first step prevents detour");
}

} // namespace

int main()
{
    const bool passed =
        empty_and_unreachable_pulses_preserve_state() &&
        single_pulse_moves_one_edge_then_contacts_on_a_later_phase() &&
        high_id_contact_disappears_before_any_movement_and_shares_damage_limit() &&
        successive_arrivals_disappear_immediately_even_after_death() &&
        legal_player_entry_contacts_before_phase_movement() &&
        each_phase_has_a_fresh_damage_limit() &&
        blocked_lower_id_does_not_retry_after_later_contact() &&
        occupied_first_step_waits_despite_a_longer_detour();
    return passed ? 0 : 1;
}
