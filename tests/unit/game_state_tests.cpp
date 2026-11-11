#include "core/game_state.hpp"
#include "core/node_rotation.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <span>

using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::Packet;
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
        state->player_integrity != 3 ||
        state->player_integrity != GameState::initial_player_integrity ||
        !state->player_alive() || state->turns_taken != TurnIndex{0} ||
        state->packet ||
        std::ranges::any_of(state->powered_nodes,
                            [](bool powered) { return powered; }) ||
        std::ranges::any_of(state->receiver_nodes,
                            [](bool receiver) { return receiver; }) ||
        !std::ranges::equal(state->ports, ports, {}, &PortMask::bits,
                            &PortMask::bits) ||
        state->rotatable_nodes != rotatable_nodes ||
        std::ranges::any_of(state->blocked_nodes,
                            [](bool blocked) { return blocked; }) ||
        state->ports[1].bits() != 0)
    {
        std::cerr
            << "Initial state must preserve board and a living player with "
               "full integrity at turn zero, without a packet, a powered "
               "target or a receiver\n";
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
    copy.blocked_nodes.back() = true;
    copy.player_position = next_position;
    copy.player_integrity = 0;
    copy.turns_taken = TurnIndex{1};
    copy.packet = Packet{.position = start, .target = next_position};
    copy.powered_nodes.back() = true;
    copy.receiver_nodes.front() = true;
    auto second_copy = copy;
    second_copy.packet->position = next_position;
    second_copy.packet->target = start;
    second_copy.powered_nodes.back() = false;
    second_copy.powered_nodes.front() = true;
    second_copy.receiver_nodes.front() = false;
    second_copy.receiver_nodes.back() = true;

    if (!rotated || copy.ports.back().bits() != PortMask::east_bit ||
        copy.player_alive() || original->packet ||
        std::ranges::any_of(original->powered_nodes,
                            [](bool powered) { return powered; }) ||
        !copy.powered_nodes.back() || copy.powered_nodes.front() ||
        second_copy.powered_nodes.back() ||
        !second_copy.powered_nodes.front() ||
        std::ranges::any_of(original->receiver_nodes,
                            [](bool receiver) { return receiver; }) ||
        !copy.receiver_nodes.front() || copy.receiver_nodes.back() ||
        second_copy.receiver_nodes.front() ||
        !second_copy.receiver_nodes.back() ||
        copy.packet != Packet{.position = start, .target = next_position} ||
        second_copy.packet !=
            Packet{.position = next_position, .target = start} ||
        original->ports.back().bits() != PortMask::north_bit ||
        !original->rotatable_nodes.back() || original->blocked_nodes.back() ||
        original->player_position != start ||
        original->player_integrity != GameState::initial_player_integrity ||
        !original->player_alive() || original->turns_taken != TurnIndex{0})
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
