#include "core/sector_outcome.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>

using closed_circuit::EntityId;
using closed_circuit::GameState;
using closed_circuit::NodeId;
using closed_circuit::Packet;
using closed_circuit::PortMask;
using closed_circuit::sector_outcome;
using closed_circuit::SectorOutcome;
using closed_circuit::TurnIndex;

namespace
{

constexpr NodeId node(int value)
{
    return *NodeId::from_value(value);
}

GameState board()
{
    GameState state{.player_position = node(0),
                    .player_integrity = 2,
                    .packet = Packet{node(24), node(24)},
                    .sector_entry = node(8),
                    .sector_exit = node(0),
                    .pulses = {{*EntityId::from_value(7), node(0)},
                               {*EntityId::from_value(0), node(48)}}};
    state.ports[24] = *PortMask::from_bits(15);
    state.rotatable_nodes[24] = true;
    state.blocked_nodes[0] = true;
    state.receiver_nodes[0] = true;
    state.receiver_nodes[48] = true;
    state.reserve = 0;
    state.overload_debt = 17;
    return state;
}

bool same_state(const GameState &left, const GameState &right)
{
    return left.reserve == right.reserve &&
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

bool outcome_is(const GameState &state, SectorOutcome expected)
{
    const auto before = state;
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (sector_outcome(state) != expected || !same_state(state, before))
        {
            std::cerr << "Unexpected sector outcome or modified state\n";
            return false;
        }
    }
    return true;
}

bool outcomes_obey_delivery_exit_life_and_turn_boundaries()
{
    constexpr std::array turns{0U, 47U, 48U, 49U,
                               std::numeric_limits<unsigned int>::max()};
    for (const auto exit : {0, 48})
    {
        for (const auto turn : turns)
        {
            for (int deliveries = 0; deliveries <= 2; ++deliveries)
            {
                for (const auto hp : {0, 1, 3})
                {
                    // No exit, off the exit, on the exit.
                    for (int exit_case = 0; exit_case < 3; ++exit_case)
                    {
                        auto state = board();
                        state.turns_taken = TurnIndex{turn};
                        state.player_integrity = static_cast<std::uint8_t>(hp);
                        state.sector_exit = node(exit);
                        state.player_position =
                            node(exit_case == 1 ? 24 : exit);
                        if (exit_case == 0)
                        {
                            state.sector_exit.reset();
                        }
                        state.powered_nodes[0] = deliveries >= 1;
                        state.powered_nodes[48] = deliveries >= 2;
                        auto expected = SectorOutcome::ongoing;
                        if (hp == 0)
                        {
                            expected = SectorOutcome::death;
                        }
                        else if (deliveries == 2 && exit_case == 2 &&
                                 turn <= 48)
                        {
                            expected = SectorOutcome::completed;
                        }
                        else if (turn >= 48)
                        {
                            expected = SectorOutcome::overload;
                        }
                        if (!outcome_is(state, expected))
                        {
                            std::cerr << "exit=" << exit << " turn=" << turn
                                      << " deliveries=" << deliveries
                                      << " hp=" << hp
                                      << " exit_case=" << exit_case << '\n';
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool only_powered_receivers_count_and_pending_phases_stay_pending()
{
    for (const auto turn : {47, 48, 49})
    {
        const auto unfinished =
            turn == 47 ? SectorOutcome::ongoing : SectorOutcome::overload;
        auto state = board();
        state.turns_taken = TurnIndex{static_cast<std::uint32_t>(turn)};
        state.powered_nodes.fill(true);
        state.powered_nodes[0] = false;
        state.powered_nodes[48] = false;
        if (!outcome_is(state, unfinished))
        {
            std::cerr << "Powered nonreceivers must not count as deliveries\n";
            return false;
        }
        state.powered_nodes[0] = true;
        if (!outcome_is(state, unfinished))
        {
            return false;
        }
        state.receiver_nodes[48] = false;
        state.powered_nodes[48] = true;
        if (!outcome_is(state, unfinished))
        {
            std::cerr << "Powering every existing receiver still needs two\n";
            return false;
        }
        state.receiver_nodes.fill(false);
        if (!outcome_is(state, unfinished))
        {
            return false;
        }
        // Packet is already on this second receiver but not delivered; a
        // contact also remains on the living player. A query runs neither.
        state.receiver_nodes[0] = true;
        state.receiver_nodes[24] = true;
        state.powered_nodes[24] = false;
        if (!outcome_is(state, unfinished))
        {
            return false;
        }
        state.powered_nodes[24] = true;
        const auto completed =
            turn <= 48 ? SectorOutcome::completed : SectorOutcome::overload;
        if (!outcome_is(state, completed))
        {
            std::cerr << "A pending contact must not resolve during a query\n";
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    return outcomes_obey_delivery_exit_life_and_turn_boundaries() &&
                   only_powered_receivers_count_and_pending_phases_stay_pending()
               ? 0
               : 1;
}
