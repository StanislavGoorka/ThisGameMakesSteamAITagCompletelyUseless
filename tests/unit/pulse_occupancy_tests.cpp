#include "core/pulse_occupancy.hpp"

#include <algorithm>
#include <iostream>

using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::pulse_occupies;
using closed_circuit::TurnIndex;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

GameState board()
{
    GameState state{.player_position = node(24),
                    .turns_taken = TurnIndex{7},
                    .packet = Packet{.position = node(0), .target = node(48)},
                    .sector_entry = node(0),
                    .sector_exit = *NodeId::from_value(48)};
    state.ports[24] = *PortMask::from_bits(15);
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[1] = true;
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

bool occupancy_matches_without_changing_state(const GameState &state,
                                              bool populated)
{
    const auto original = state;
    for (int value = 0; value <= NodeId::max_value; ++value)
    {
        const bool expected =
            populated && (value == 0 || value == 24 || value == 48);
        for (int attempt = 0; attempt < 3; ++attempt)
        {
            if (pulse_occupies(state, node(value)) != expected ||
                !same_state(state, original))
            {
                std::cerr << "Pulse occupancy must match the collection "
                             "and preserve the entire state at node "
                          << value << '\n';
                return false;
            }
        }
    }
    return true;
}

bool empty_collection_never_occupies_a_node()
{
    return occupancy_matches_without_changing_state(board(), false);
}

bool occupancy_uses_positions_independently_of_storage_and_other_fields()
{
    auto state = board();
    state.pulses = {{*EntityId::from_value(12), node(48)},
                    {*EntityId::from_value(0), node(0)},
                    {*EntityId::from_value(EntityId::max_value), node(24)}};
    if (!occupancy_matches_without_changing_state(state, true))
    {
        return false;
    }
    std::ranges::reverse(state.pulses);
    state.ports.fill(PortMask{});
    state.blocked_nodes.fill(true);
    state.player_integrity = 0;
    state.turns_taken = TurnIndex{0};
    return occupancy_matches_without_changing_state(state, true);
}

} // namespace

int main()
{
    const bool passed =
        empty_collection_never_occupies_a_node() &&
        occupancy_uses_positions_independently_of_storage_and_other_fields();
    return passed ? 0 : 1;
}
