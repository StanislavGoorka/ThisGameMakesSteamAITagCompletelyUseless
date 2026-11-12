#include "core/player_movement.hpp"
#include "core/pulse_contact.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>

using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::move_player;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::resolve_pulse_contact;
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
    GameState state{.player_position = node(24),
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(24), .target = node(48)},
                    .sector_entry = node(0),
                    .sector_exit = *NodeId::from_value(48),
                    .pulses = {{low_id, node(0)},
                               {middle_id, node(24)},
                               {high_id, node(48)}}};
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[24] = true;
    state.powered_nodes[3] = true;
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

bool expect_contact(GameState &state, EntityId id, bool &damage_applied,
                    bool expected_contact, const GameState &expected,
                    bool expected_damage_applied, std::string_view context)
{
    const auto contacted = resolve_pulse_contact(state, id, damage_applied);
    if (contacted != expected_contact || !same_state(state, expected) ||
        damage_applied != expected_damage_applied)
    {
        std::cerr << context
                  << ": unexpected contact, full state or damage "
                     "limit\n";
        return false;
    }
    return true;
}

bool absent_id_no_contact_and_empty_collection_preserve_state_and_limit()
{
    for (const bool already_applied : {false, true})
    {
        auto state = board();
        bool damage_applied = already_applied;
        const auto original = state;
        if (!expect_contact(state, *EntityId::from_value(8), damage_applied,
                            false, original, already_applied, "absent ID") ||
            !expect_contact(state, low_id, damage_applied, false, original,
                            already_applied, "pulse away from player"))
        {
            return false;
        }
        state.pulses.clear();
        const auto empty = state;
        if (!expect_contact(state, high_id, damage_applied, false, empty,
                            already_applied, "empty collection"))
        {
            return false;
        }
    }
    return true;
}

bool contact_preserves_survivor_order_and_saturates_damage_at_zero()
{
    const auto initial = board();
    for (const auto target : initial.pulses)
    {
        for (const auto integrity :
             {std::uint8_t{3}, std::uint8_t{1}, std::uint8_t{0}})
        {
            for (const bool already_applied : {false, true})
            {
                std::array order{0, 1, 2};
                do
                {
                    auto state = initial;
                    state.player_position = target.position;
                    state.player_integrity = integrity;
                    for (std::size_t index = 0; index < order.size(); ++index)
                    {
                        state.pulses[index] = initial.pulses[order[index]];
                    }
                    auto expected = state;
                    std::erase_if(expected.pulses, [target](const auto &pulse)
                                  { return pulse.id == target.id; });
                    const bool can_damage = !already_applied && integrity > 0;
                    expected.player_integrity = static_cast<std::uint8_t>(
                        integrity - (can_damage ? 1 : 0));
                    bool damage_applied = already_applied;
                    if (!expect_contact(state, target.id, damage_applied, true,
                                        expected, already_applied || can_damage,
                                        "contact and damage boundary") ||
                        !expect_contact(state, target.id, damage_applied, false,
                                        expected, already_applied || can_damage,
                                        "retry after removal"))
                    {
                        return false;
                    }
                } while (std::ranges::next_permutation(order).found);
            }
        }
    }
    return true;
}

bool shared_limit_removes_every_contact_and_fresh_limit_allows_next_damage()
{
    auto state = board();
    bool damage_applied = false;
    for (const auto id : {middle_id, low_id, high_id})
    {
        std::ranges::find(state.pulses, id, &closed_circuit::Pulse::id)
            ->position = state.player_position;
        auto expected = state;
        std::erase_if(expected.pulses,
                      [id](const auto &pulse) { return pulse.id == id; });
        expected.player_integrity = 2;
        if (!expect_contact(state, id, damage_applied, true, expected, true,
                            "multiple contacts share one damage limit"))
        {
            return false;
        }
    }

    state.pulses.push_back({low_id, state.player_position});
    auto expected = state;
    expected.pulses.clear();
    expected.player_integrity = 1;
    bool next_turn_damage_applied = false;
    return expect_contact(state, low_id, next_turn_damage_applied, true,
                          expected, true, "fresh damage limit");
}

bool player_entry_then_contact_changes_only_position_integrity_and_pulses()
{
    auto state = board();
    state.player_position = node(23);
    state.blocked_nodes[24] = false;
    state.ports[23] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[24] = *PortMask::from_bits(PortMask::west_bit);
    auto moved = state;
    moved.player_position = node(24);
    if (!move_player(state, node(24)) || !same_state(state, moved))
    {
        std::cerr << "Player entry on a pulse must change only position\n";
        return false;
    }
    auto expected = moved;
    expected.pulses = {{low_id, node(0)}, {high_id, node(48)}};
    expected.player_integrity = 2;
    bool damage_applied = false;
    return expect_contact(state, middle_id, damage_applied, true, expected,
                          true, "player entry then contact");
}

} // namespace

int main()
{
    const bool passed =
        absent_id_no_contact_and_empty_collection_preserve_state_and_limit() &&
        contact_preserves_survivor_order_and_saturates_damage_at_zero() &&
        shared_limit_removes_every_contact_and_fresh_limit_allows_next_damage() &&
        player_entry_then_contact_changes_only_position_integrity_and_pulses();
    return passed ? 0 : 1;
}
