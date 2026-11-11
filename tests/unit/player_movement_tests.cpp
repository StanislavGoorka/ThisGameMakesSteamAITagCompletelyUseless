#include "core/player_movement.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

using closed_circuit::GameState;
using closed_circuit::move_player;
using closed_circuit::NodeId;
using closed_circuit::PortMask;
using closed_circuit::TurnIndex;

namespace
{

constexpr auto center = *NodeId::from_value(24);
constexpr auto east = *NodeId::from_value(25);
constexpr auto all_ports = *PortMask::from_bits(15);

struct Move
{
    int source;
    int target;
};

GameState open_board(NodeId player_position)
{
    GameState state{.player_position = player_position,
                    .turns_taken = TurnIndex{7}};
    state.ports.fill(all_ports);
    state.rotatable_nodes[center.value()] = true;
    state.blocked_nodes[0] = true;
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           std::ranges::equal(left.ports, right.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}

bool legal_move_changes_only_position_in_each_direction()
{
    constexpr std::array cases{Move{24, 17}, Move{24, 25}, Move{24, 31},
                               Move{24, 23}, Move{0, 1},   Move{0, 7},
                               Move{48, 41}, Move{48, 47}};
    for (const auto move : cases)
    {
        auto state = open_board(*NodeId::from_value(move.source));
        const auto target = *NodeId::from_value(move.target);
        auto expected = state;
        expected.player_position = target;

        if (!move_player(state, target) || !same_state(state, expected))
        {
            std::cerr << "A connected free neighbor must accept movement "
                         "and change only the player position\n";
            return false;
        }
    }
    return true;
}

bool missing_reciprocal_edge_preserves_state()
{
    struct Ports
    {
        std::uint8_t source;
        std::uint8_t target;
    };
    constexpr std::array cases{Ports{0, 0}, Ports{PortMask::east_bit, 0},
                               Ports{0, PortMask::west_bit},
                               Ports{PortMask::east_bit, PortMask::east_bit}};
    for (const auto ports : cases)
    {
        auto state = open_board(center);
        state.ports[center.value()] = *PortMask::from_bits(ports.source);
        state.ports[east.value()] = *PortMask::from_bits(ports.target);
        const auto original = state;

        if (move_player(state, east) || !same_state(state, original))
        {
            std::cerr << "Missing or one-way edges must reject movement "
                         "without changing the state\n";
            return false;
        }
    }
    return true;
}

bool non_neighbor_and_wrapped_moves_preserve_state()
{
    constexpr std::array cases{
        Move{24, 24}, Move{24, 26},
        Move{24, 32},              // Self, distant, diagonal.
        Move{6, 7},   Move{7, 6},  // No row wrapping.
        Move{0, 42},  Move{42, 0}, // No vertical wrapping.
        Move{0, 6},   Move{6, 0},
        Move{48, 0}}; // No horizontal/corner wrapping.
    for (const auto move : cases)
    {
        auto state = open_board(*NodeId::from_value(move.source));
        state.blocked_nodes.fill(false);
        const auto original = state;

        if (move_player(state, *NodeId::from_value(move.target)) ||
            !same_state(state, original))
        {
            std::cerr
                << "Only an orthogonal neighbor inside the grid may "
                   "accept movement; rejected moves must preserve state\n";
            return false;
        }
    }
    return true;
}

bool blocking_unit_rejects_move_until_target_is_free()
{
    auto state = open_board(center);
    state.blocked_nodes[east.value()] = true;
    const auto original = state;
    if (move_player(state, east) || !same_state(state, original))
    {
        std::cerr << "A blocking unit must reject movement over an existing "
                     "edge without changing the state\n";
        return false;
    }

    state.blocked_nodes[east.value()] = false;
    auto expected = state;
    expected.player_position = east;
    if (!move_player(state, east) || !same_state(state, expected))
    {
        std::cerr << "A previously rejected move must succeed once its "
                     "target is free\n";
        return false;
    }
    return true;
}

} // namespace

int main()
{
    const bool passed = legal_move_changes_only_position_in_each_direction() &&
                        missing_reciprocal_edge_preserves_state() &&
                        non_neighbor_and_wrapped_moves_preserve_state() &&
                        blocking_unit_rejects_move_until_target_is_free();
    return passed ? 0 : 1;
}
