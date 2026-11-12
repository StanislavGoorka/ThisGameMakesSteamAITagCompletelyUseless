#include "core/grid_adjacency.hpp"
#include "core/sector_topology.hpp"
#include <algorithm>
#include <bit>
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
bool same(const SectorTopology &a, const SectorTopology &b)
{
    return a.entry == b.entry && a.exit == b.exit &&
           a.receivers == b.receivers &&
           a.rotatable_nodes == b.rotatable_nodes &&
           std::ranges::equal(a.ports, b.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}
struct Vector
{
    RunSeed seed;
    bool top;
    std::array<std::uint8_t, 13> masks;
};
const Vector vectors[]{
    {RunSeed{0ULL}, false, {4, 5, 10, 5, 5, 10, 12, 5, 10, 10, 5, 10, 8}},
    {RunSeed{1ULL}, false, {4, 10, 5, 10, 5, 10, 3, 5, 10, 5, 10, 10, 8}},
    {RunSeed{2ULL}, true, {2, 10, 5, 10, 10, 5, 6, 5, 5, 10, 10, 5, 1}},
    {RunSeed{18446744073709551615ULL},
     true,
     {2, 5, 5, 10, 10, 10, 6, 10, 5, 5, 5, 5, 1}}};
bool valid(const SectorTopology &s)
{
    if (s.entry != node(0) || s.exit != node(48) ||
        s.receivers[0] == s.receivers[1])
        return false;
    const bool top = s.receivers[0] == node(4);
    if (s.receivers[1] != node(top ? 20 : 44) ||
        s.receivers[0] != node(top ? 4 : 28))
        return false;
    for (int n = 0; n < 49; ++n)
    {
        const bool active =
            top ? (n < 7 || n % 7 == 6) : (n % 7 == 0 || n >= 42);
        const bool terminal = n == 0 || n == 48;
        const bool fixed = terminal || n == s.receivers[0].value() ||
                           n == s.receivers[1].value();
        if (s.ports[n].bits() > 15 ||
            std::popcount(static_cast<unsigned>(s.ports[n].bits())) !=
                (active ? (terminal ? 1 : 2) : 0) ||
            s.rotatable_nodes[n] != (active && !fixed))
            return false;
    }
    if (s.ports[0].bits() != (top ? 2 : 4) ||
        s.ports[48].bits() != (top ? 1 : 8) ||
        s.ports[s.receivers[0].value()].bits() != (top ? 10 : 5) ||
        s.ports[s.receivers[1].value()].bits() != (top ? 5 : 10))
        return false;
    return true;
}
} // namespace
int main()
{
    SplitMix64 external{RunSeed{8}};
    auto reference = external;
    for (const auto &v : vectors)
    {
        const auto s = generate_sector_topology(v.seed);
        if (!valid(s))
        {
            std::cerr << "Topology invariant mismatch\n";
            return 1;
        }
        for (int i = 0; i < 13; ++i)
        {
            int n = v.top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                          : (i <= 6 ? i * 7 : 42 + i - 6);
            if (s.ports[n].bits() != v.masks[i])
            {
                std::cerr << "Independent topology vector mismatch\n";
                return 1;
            }
        }
        for (int repeat = 0; repeat < 3; ++repeat)
            if (!same(s, generate_sector_topology(v.seed)) ||
                external.next_u64() != reference.next_u64())
                return 1;
        auto changed = s;
        auto second = changed;
        changed.ports[0] = *PortMask::from_bits(0);
        changed.receivers[0] = node(48);
        changed.rotatable_nodes[1] = !changed.rotatable_nodes[1];
        second.exit = node(0);
        if (!same(s, generate_sector_topology(v.seed)) || same(changed, s) ||
            same(second, s))
            return 1;
    }
    for (std::uint64_t seed = 0; seed < 32; ++seed)
        if (!valid(generate_sector_topology(RunSeed{seed})))
            return 1;
    return 0;
}
