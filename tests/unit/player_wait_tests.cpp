#include "core/player_wait.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

using closed_circuit::charge_turn;
using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::TurnIndex;
using closed_circuit::wait_player;

namespace
{

constexpr auto center = *NodeId::from_value(24);
constexpr auto all_ports = *PortMask::from_bits(15);
constexpr auto north_port = *PortMask::from_bits(PortMask::north_bit);

// The player stands on a fixed node without ports and every other node is
// occupied, so neither movement nor rotation of the own node is available.
GameState enclosed_board(NodeId player_position, std::uint8_t integrity)
{
    GameState state{.player_position = player_position,
                    .player_integrity = integrity,
                    .turns_taken = TurnIndex{7}};
    state.ports.fill(all_ports);
    state.ports[player_position.value()] = PortMask{};
    state.rotatable_nodes.fill(true);
    state.rotatable_nodes[player_position.value()] = false;
    state.blocked_nodes.fill(true);
    state.blocked_nodes[player_position.value()] = false;
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
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

bool living_player_wait_costs_one_turn_and_changes_nothing_else()
{
    constexpr std::array positions{0, 6, 24, 42, 48};
    constexpr std::array<std::uint8_t, 4> integrities{1, 2, 3, 255};
    for (const auto position : positions)
    {
        for (const auto integrity : integrities)
        {
            auto state =
                enclosed_board(*NodeId::from_value(position), integrity);
            auto expected = state;
            expected.turns_taken = TurnIndex{8};

            if (!wait_player(state) || !same_state(state, expected))
            {
                std::cerr << "A living player must always be able to wait; "
                             "waiting must cost exactly one turn and keep the "
                             "board, position and integrity\n";
                return false;
            }
        }
    }
    return true;
}

bool repeated_waits_cost_one_turn_each_without_regeneration()
{
    auto state = enclosed_board(center, 1);
    auto expected = state;
    for (std::uint32_t waits = 1; waits <= 100; ++waits)
    {
        expected.turns_taken = TurnIndex{7 + waits};
        if (!wait_player(state) || !same_state(state, expected))
        {
            std::cerr << "Every wait must add exactly one turn and must "
                         "never restore lost integrity\n";
            return false;
        }
    }
    return true;
}

bool dead_player_rejects_wait_without_changing_state()
{
    auto state = enclosed_board(center, 0);
    const auto original = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (wait_player(state) || !same_state(state, original))
        {
            std::cerr << "A dead player must reject every wait without "
                         "changing the turn count or any other state\n";
            return false;
        }
    }

    // Integrity alone decides the rejection.
    state.player_integrity = 1;
    auto expected = state;
    expected.turns_taken = TurnIndex{8};
    if (!wait_player(state) || !same_state(state, expected))
    {
        std::cerr << "The same state with a living player must accept the "
                     "wait and charge one turn\n";
        return false;
    }
    return true;
}

bool charging_a_turn_adds_one_and_changes_nothing_else()
{
    GameState state{.player_position = center};
    state.ports[center.value()] = north_port;
    state.rotatable_nodes[0] = true;
    state.blocked_nodes[48] = true;
    auto expected = state;

    charge_turn(state);
    expected.turns_taken = TurnIndex{1};
    if (!same_state(state, expected))
    {
        std::cerr << "The first charged turn must move the counter from "
                     "zero to one and change nothing else\n";
        return false;
    }

    charge_turn(state);
    expected.turns_taken = TurnIndex{2};
    if (!same_state(state, expected))
    {
        std::cerr << "Each further charge must add exactly one turn\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        living_player_wait_costs_one_turn_and_changes_nothing_else() &&
        repeated_waits_cost_one_turn_each_without_regeneration() &&
        dead_player_rejects_wait_without_changing_state() &&
        charging_a_turn_adds_one_and_changes_nothing_else();
    return passed ? 0 : 1;
}
