#include "core/player_rotation.hpp"

#include <algorithm>
#include <array>
#include <iostream>

using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::rotate_from_player;
using closed_circuit::TurnIndex;

namespace
{

constexpr auto center = *NodeId::from_value(24);
constexpr auto north_port = *PortMask::from_bits(PortMask::north_bit);
constexpr auto east_port = *PortMask::from_bits(PortMask::east_bit);

struct Rotation
{
    int source;
    int target;
};

GameState disconnected_board(NodeId player_position, NodeId target)
{
    GameState state{.player_position = player_position,
                    .turns_taken = TurnIndex{7}};
    state.sector_entry = center;
    state.ports[target.value()] = north_port;
    state.rotatable_nodes.fill(true);
    state.rotatable_nodes[48] = false;
    state.rotatable_nodes[target.value()] = true;
    state.blocked_nodes[0] = true;
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
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

bool own_node_and_neighbors_rotate_without_connections()
{
    constexpr std::array cases{
        Rotation{24, 24}, Rotation{24, 17}, Rotation{24, 25}, Rotation{24, 31},
        Rotation{24, 23}, Rotation{0, 0},   Rotation{0, 1},   Rotation{0, 7},
        Rotation{6, 6},   Rotation{6, 5},   Rotation{6, 13},  Rotation{42, 42},
        Rotation{42, 35}, Rotation{42, 43}, Rotation{48, 48}, Rotation{48, 41},
        Rotation{48, 47}};
    for (const auto rotation : cases)
    {
        const auto target = *NodeId::from_value(rotation.target);
        auto state =
            disconnected_board(*NodeId::from_value(rotation.source), target);
        // Occupancy restricts movement, not rotation.
        state.blocked_nodes[target.value()] = true;
        auto expected = state;
        expected.ports[target.value()] = east_port;

        if (!rotate_from_player(state, target) || !same_state(state, expected))
        {
            std::cerr << "An own or orthogonally adjacent node must rotate "
                         "clockwise and change only its ports, even without "
                         "connections or when occupied\n";
            return false;
        }
    }
    return true;
}

bool distant_diagonal_and_wrapped_targets_preserve_state()
{
    constexpr std::array cases{
        Rotation{24, 10}, Rotation{24, 38}, Rotation{24, 22}, Rotation{24, 26},
        Rotation{24, 16}, Rotation{24, 18}, Rotation{24, 30}, Rotation{24, 32},
        Rotation{6, 7},   Rotation{7, 6},  // No row wrapping.
        Rotation{0, 42},  Rotation{42, 0}, // No vertical wrapping.
        Rotation{0, 6},   Rotation{6, 0},   Rotation{0, 48},  Rotation{48, 0}};
    for (const auto rotation : cases)
    {
        const auto target = *NodeId::from_value(rotation.target);
        auto state =
            disconnected_board(*NodeId::from_value(rotation.source), target);
        const auto original = state;

        if (rotate_from_player(state, target) || !same_state(state, original))
        {
            std::cerr << "An out-of-range target must reject rotation "
                         "without changing any state or turn count\n";
            return false;
        }
    }
    return true;
}

bool fixed_targets_preserve_state_until_made_rotatable()
{
    constexpr std::array targets{24, 17, 25, 31, 23};
    for (const auto value : targets)
    {
        const auto target = *NodeId::from_value(value);
        auto state = disconnected_board(center, target);
        state.rotatable_nodes[target.value()] = false;
        const auto original = state;
        if (rotate_from_player(state, target) || !same_state(state, original))
        {
            std::cerr << "A fixed own node or neighbor must reject rotation "
                         "without changing the state\n";
            return false;
        }

        state.rotatable_nodes[target.value()] = true;
        auto expected = state;
        expected.ports[target.value()] = east_port;
        if (!rotate_from_player(state, target) || !same_state(state, expected))
        {
            std::cerr << "A rejected rotation must succeed after its target "
                         "becomes rotatable\n";
            return false;
        }
    }
    return true;
}

bool connected_neighbor_rotates_and_four_rotations_restore_state()
{
    const auto target = *NodeId::from_value(17);
    auto state = disconnected_board(center, target);
    state.ports[center.value()] = north_port;
    state.ports[target.value()] = *PortMask::from_bits(PortMask::south_bit);
    const auto original = state;
    auto expected = state;
    expected.ports[target.value()] = *PortMask::from_bits(PortMask::west_bit);
    if (!rotate_from_player(state, target) || !same_state(state, expected))
    {
        std::cerr << "A connected neighbor must also accept rotation\n";
        return false;
    }
    for (int turn = 1; turn < 4; ++turn)
    {
        if (!rotate_from_player(state, target))
        {
            std::cerr << "Rotation must remain legal after disconnection\n";
            return false;
        }
    }
    if (!same_state(state, original))
    {
        std::cerr << "Four rotations must restore the complete state\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed =
        own_node_and_neighbors_rotate_without_connections() &&
        distant_diagonal_and_wrapped_targets_preserve_state() &&
        fixed_targets_preserve_state_until_made_rotatable() &&
        connected_neighbor_rotates_and_four_rotations_restore_state();
    return passed ? 0 : 1;
}
