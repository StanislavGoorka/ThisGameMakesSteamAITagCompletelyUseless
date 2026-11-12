#include "core/player_overdrive.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
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

GameState board(int source)
{
    GameState s{.player_position = node(source)};
    s.rotatable_nodes.fill(true);
    s.blocked_nodes.fill(true);
    s.packet = Packet{node(0), node(48)};
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.receiver_nodes[48] = true;
    s.powered_nodes[1] = true;
    s.pulses = {{*EntityId::from_value(7), node(24)}};
    s.turns_taken = TurnIndex{48};
    s.player_integrity = 0;
    return s;
}
} // namespace
int main()
{
    // Exhaust every source/target pair, including row wrapping, diagonals and
    // edges.
    for (int source = 0; source < 49; ++source)
        for (int target = 0; target < 49; ++target)
        {
            auto s = board(source);
            s.overload_debt = 19;
            const int distance = std::abs(source / 7 - target / 7) +
                                 std::abs(source % 7 - target % 7);
            auto expected = s;
            if (distance <= 1)
            {
                --expected.reserve;
                ++expected.overload_debt;
            }
            if (overdrive_from_player(s, node(target)) != (distance <= 1) ||
                !same_state(s, expected))
            {
                std::cerr << "Range/atomic state mismatch\n";
                return 1;
            }
        }
    for (unsigned mask = 0; mask < 16; ++mask)
        for (int reserve : {0, 1, 3})
            for (std::uint32_t debt : {0U, UINT32_MAX - 1, UINT32_MAX})
                for (bool rotatable : {false, true})
                {
                    auto s = board(24);
                    s.ports[25] = *PortMask::from_bits(mask);
                    s.reserve = static_cast<std::uint8_t>(reserve);
                    s.overload_debt = debt;
                    s.rotatable_nodes[25] = rotatable;
                    const bool accepted =
                        reserve > 0 && debt < UINT32_MAX && rotatable;
                    auto expected = s;
                    if (accepted)
                    {
                        expected.ports[25] = *PortMask::from_bits(
                            ((mask << 2) | (mask >> 2)) & 15);
                        --expected.reserve;
                        ++expected.overload_debt;
                    }
                    if (overdrive_from_player(s, node(25)) != accepted ||
                        !same_state(s, expected))
                    {
                        std::cerr
                            << "Mask/resources/immutable target mismatch\n";
                        return 1;
                    }
                }
    return 0;
}
