#include "core/pulse_identity.hpp"
#include "core/splitmix64.hpp"
#include <algorithm>
#include <array>
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

bool expect(const GameState &s, std::uint32_t id)
{
    const auto before = s;
    SplitMix64 rng{RunSeed{8}};
    auto reference = rng;
    for (int repeat = 0; repeat < 3; ++repeat)
        if (available_pulse_id(s) != EntityId::from_value(id) ||
            !same_state(s, before) || rng.next_u64() != reference.next_u64())
        {
            std::cerr << "Free ID or full state mismatch\n";
            return false;
        }
    return true;
}
} // namespace
int main()
{
    GameState s{.player_position = node(48)};
    s.reserve = 1;
    s.overload_debt = UINT32_MAX;
    s.packet = Packet{node(0), node(48)};
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.ports[0] = *PortMask::from_bits(15);
    s.blocked_nodes[0] = true;
    s.receiver_nodes[1] = true;
    s.powered_nodes[2] = true;
    s.rotatable_nodes[3] = true;
    s.player_integrity = 0;
    s.turns_taken = TurnIndex{48};
    if (!expect(s, 0))
        return 1;
    std::array<std::uint32_t, 4> ids{0, 1, 3, UINT32_MAX};
    do
    {
        s.pulses.clear();
        int position = 0;
        for (auto id : ids)
            s.pulses.push_back({*EntityId::from_value(id), node(position++)});
        if (!expect(s, 2))
            return 1;
    } while (std::next_permutation(ids.begin(), ids.end()));
    s.pulses = {{*EntityId::from_value(UINT32_MAX), node(48)}};
    if (!expect(s, 0))
        return 1;
    s.pulses.clear();
    for (int i = 0; i < 49; ++i)
    {
        s.pulses.push_back({*EntityId::from_value(i), node(i)});
        if (!expect(s, i + 1))
            return 1;
    }
    std::ranges::reverse(s.pulses);
    if (!expect(s, 49))
        return 1;
    s.pulses.erase(s.pulses.begin() + 24);
    if (!expect(s, 24))
        return 1;
    return 0;
}
