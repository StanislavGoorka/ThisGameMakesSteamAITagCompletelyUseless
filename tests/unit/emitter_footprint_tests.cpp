#include "core/emitter_footprint.hpp"
#include <algorithm>
#include <iostream>
using namespace closed_circuit;
namespace
{
using Ports = std::array<PortMask, 49>;
PeriodicEmitter emitter(unsigned node, unsigned range, unsigned phase = 0)
{
    return *PeriodicEmitter::from_values(
        *EntityId::from_value(7), *NodeId::from_value(node), 3, phase, range);
}
bool exact(const Ports &ports, unsigned node, unsigned range,
           std::initializer_list<unsigned> nodes)
{
    std::array<bool, 49> expected{};
    for (const auto n : nodes)
        expected[n] = true;
    const auto before = ports;
    const auto value = emitter(node, range);
    const auto copy = value;
    return emitter_footprint(ports, value) == expected && value == copy &&
           emitter_footprint(ports, emitter(node, range, 1)) == expected &&
           std::ranges::equal(ports, before, {}, &PortMask::bits,
                              &PortMask::bits);
}
bool literals()
{
    Ports ports{};
    if (!exact(ports, 0, 48, {0}) || !exact(ports, 48, 1, {48}))
        return false;
    for (const auto [node, bits] :
         {std::pair{0, 2}, {1, 10}, {2, 12}, {9, 9}, {8, 10}, {7, 2}})
        ports[node] = *PortMask::from_bits(bits);
    // Node 7 is adjacent on the grid, but five mutual edges away.
    if (!exact(ports, 0, 1, {0, 1}) || !exact(ports, 0, 2, {0, 1, 2}) ||
        !exact(ports, 0, 4, {0, 1, 2, 9, 8}) ||
        !exact(ports, 0, 5, {0, 1, 2, 9, 8, 7}) ||
        !exact(ports, 0, 48, {0, 1, 2, 9, 8, 7}))
        return false;
    ports[1] = ports[1].rotate_clockwise();
    if (!exact(ports, 0, 48, {0}))
        return false;
    ports = {};
    for (const auto [node, bits] : {std::pair{0, 6}, {1, 12}, {8, 9}, {7, 3}})
        ports[node] = *PortMask::from_bits(bits);
    if (!exact(ports, 0, 1, {0, 1, 7}) || !exact(ports, 0, 2, {0, 1, 7, 8}) ||
        !exact(ports, 0, 48, {0, 1, 7, 8}))
        return false;
    ports = {};
    for (const auto [node, bits] :
         {std::pair{24, 15}, {17, 4}, {25, 8}, {31, 1}, {23, 2}})
        ports[node] = *PortMask::from_bits(bits);
    if (!exact(ports, 24, 1, {24, 17, 25, 31, 23}))
        return false;
    // A one-sided link is not a connection. Nor may rows wrap 6 -> 7.
    ports = {};
    ports[0] = *PortMask::from_bits(2);
    ports[6] = *PortMask::from_bits(2);
    ports[7] = *PortMask::from_bits(8);
    return exact(ports, 0, 48, {0}) && exact(ports, 6, 48, {6});
}
bool full_grid()
{
    Ports ports{};
    ports.fill(*PortMask::from_bits(15));
    // Independent geometric distance for a fully connected grid, all sources
    // and radii, including the four edges and all 49 queue entries.
    for (int source = 0; source < 49; ++source)
        for (unsigned range = 1; range <= 48; ++range)
        {
            const auto reached =
                emitter_footprint(ports, emitter(source, range));
            for (int target = 0; target < 49; ++target)
            {
                const auto row = std::abs(source / 7 - target / 7);
                const auto column = std::abs(source % 7 - target % 7);
                if (reached[target] !=
                    (row + column <= static_cast<int>(range)))
                    return false;
            }
        }
    return true;
}
} // namespace
int main()
{
    if (!literals() || !full_grid())
    {
        std::cerr << "Emitter graph footprint mismatch\n";
        return 1;
    }
    return 0;
}
