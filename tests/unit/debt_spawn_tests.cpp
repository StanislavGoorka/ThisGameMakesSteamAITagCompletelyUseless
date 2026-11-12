#include "core/debt_spawn.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <new>
using namespace closed_circuit;
bool fail_next_allocation = false;
void *operator new(std::size_t size)
{
    if (fail_next_allocation)
    {
        fail_next_allocation = false;
        throw std::bad_alloc{};
    }
    if (auto p = std::malloc(size ? size : 1))
        return p;
    throw std::bad_alloc{};
}
void operator delete(void *p) noexcept
{
    std::free(p);
}
void operator delete(void *p, std::size_t) noexcept
{
    std::free(p);
}
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

GameState board(int entry, std::uint32_t debt)
{
    GameState s{.player_position = node(entry)};
    s.sector_entry = node(entry);
    s.sector_exit = node(24);
    s.packet = Packet{node(entry), node(25)};
    s.reserve = 1;
    s.overload_debt = debt;
    s.player_integrity = 1;
    s.turns_taken = TurnIndex{17};
    s.ports[entry] = *PortMask::from_bits(3);
    s.rotatable_nodes[entry] = true;
    s.powered_nodes[1] = true;
    s.receiver_nodes[25] = true;
    s.pulses = {{*EntityId::from_value(UINT32_MAX), node(7)},
                {*EntityId::from_value(0), node(8)}};
    return s;
}
bool allocation_failure_preserves_state_and_allows_retry()
{
    auto s = board(0, 6);
    std::vector<Pulse>{}.swap(s.pulses);
    const auto before = s;
    fail_next_allocation = true;
    bool threw = false;
    try
    {
        static_cast<void>(spawn_debt_pulse(s));
    }
    catch (const std::bad_alloc &)
    {
        threw = true;
    }
    fail_next_allocation = false;
    if (!threw || !same_state(s, before))
    {
        std::cerr << "Allocation failure must preserve full state\n";
        return false;
    }
    auto expected = before;
    expected.pulses.push_back({*EntityId::from_value(0), node(0)});
    expected.overload_debt = 3;
    if (!spawn_debt_pulse(s) || !same_state(s, expected) ||
        spawn_debt_pulse(s) || !same_state(s, expected))
        return false;
    return true;
}
} // namespace
int main()
{
    for (int entry : {0, 48})
        for (std::uint32_t debt : {0U, 1U, 2U, 3U, 4U, 5U, 6U, UINT32_MAX})
            for (int mode = 0; mode < 4; ++mode)
                for (bool reverse : {false, true})
                {
                    auto s = board(entry, debt);
                    if (reverse)
                        std::ranges::reverse(s.pulses);
                    if (mode == 1)
                        s.sector_entry.reset();
                    if (mode == 2)
                        s.blocked_nodes[entry] = true;
                    if (mode == 3)
                        s.pulses.push_back(
                            {*EntityId::from_value(7), node(entry)});
                    const auto before = s;
                    const bool accepted = mode == 0 && debt >= 3;
                    auto expected = s;
                    if (accepted)
                    {
                        expected.pulses.push_back(
                            {*EntityId::from_value(1), node(entry)});
                        expected.overload_debt = debt - 3;
                    }
                    if (spawn_debt_pulse(s) != accepted ||
                        !same_state(s, expected))
                    {
                        std::cerr << "Debt spawn full state mismatch\n";
                        return 1;
                    }
                    auto copy = s;
                    copy.reserve = 3;
                    copy.overload_debt = 0;
                    copy.pulses.clear();
                    if (!same_state(s, expected) || spawn_debt_pulse(s) ||
                        !same_state(s, expected))
                    {
                        std::cerr << "Repeated spawn or copies changed state\n";
                        return 1;
                    }
                }
    return allocation_failure_preserves_state_and_allows_retry() ? 0 : 1;
}
