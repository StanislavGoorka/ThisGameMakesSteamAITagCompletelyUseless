#include "core/game_state.hpp"
#include "core/node_rotation.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <span>

using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::rotate_node;
using closed_circuit::TurnIndex;

namespace
{

constexpr auto start = *NodeId::from_value(48);
constexpr auto next_position = *NodeId::from_value(47);
constexpr auto north_port = *PortMask::from_bits(PortMask::north_bit);
constexpr auto west_port = *PortMask::from_bits(PortMask::west_bit);

bool initial_state_preserves_input_and_starts_at_zero()
{
    std::array<PortMask, GameState::node_count> ports{};
    ports.front() = west_port;
    ports.back() = north_port;
    std::array<bool, GameState::node_count> rotatable_nodes{};
    rotatable_nodes.back() = true;

    const auto state = GameState::initial(ports, rotatable_nodes, start);
    if (!state || state->player_position != start ||
        state->turns_taken != TurnIndex{0} ||
        !std::ranges::equal(state->ports, ports, {}, &PortMask::bits,
                            &PortMask::bits) ||
        state->rotatable_nodes != rotatable_nodes ||
        state->ports[1].bits() != 0)
    {
        std::cerr
            << "Initial state must preserve board and player at turn zero\n";
        return false;
    }
    return true;
}

bool invalid_initial_inputs_are_rejected()
{
    std::array<PortMask, GameState::node_count + 1> ports{};
    std::array<bool, GameState::node_count + 1> rotatable_nodes{};
    const auto valid_ports = std::span{ports}.first(GameState::node_count);
    const auto valid_flags =
        std::span{rotatable_nodes}.first(GameState::node_count);

    if (GameState::initial(valid_ports.first(valid_ports.size() - 1),
                           valid_flags, start) ||
        GameState::initial(ports, valid_flags, start) ||
        GameState::initial(valid_ports,
                           valid_flags.first(valid_flags.size() - 1), start) ||
        GameState::initial(valid_ports, rotatable_nodes, start) ||
        GameState::initial(valid_ports, valid_flags, NodeId::from_value(-1)) ||
        GameState::initial(valid_ports, valid_flags, NodeId::from_value(49)))
    {
        std::cerr
            << "Initial state must reject invalid board sizes and nodes\n";
        return false;
    }
    return true;
}

bool changing_inputs_and_copy_preserves_original_state()
{
    std::array<PortMask, GameState::node_count> ports{};
    ports.back() = north_port;
    std::array<bool, GameState::node_count> rotatable_nodes{};
    rotatable_nodes.back() = true;
    const auto original = GameState::initial(ports, rotatable_nodes, start);
    if (!original)
    {
        std::cerr << "Could not create a valid state to copy\n";
        return false;
    }

    auto copy = *original;
    ports.back() = west_port;
    rotatable_nodes.back() = false;
    const bool rotated = rotate_node(copy.ports, copy.rotatable_nodes, start);
    copy.rotatable_nodes.back() = false;
    copy.player_position = next_position;
    copy.turns_taken = TurnIndex{1};

    if (!rotated || copy.ports.back().bits() != PortMask::east_bit ||
        original->ports.back().bits() != PortMask::north_bit ||
        !original->rotatable_nodes.back() ||
        original->player_position != start ||
        original->turns_taken != TurnIndex{0})
    {
        std::cerr
            << "Initial state must own its board and copy independently\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed = initial_state_preserves_input_and_starts_at_zero() &&
                        invalid_initial_inputs_are_rejected() &&
                        changing_inputs_and_copy_preserves_original_state();
    return passed ? 0 : 1;
}
