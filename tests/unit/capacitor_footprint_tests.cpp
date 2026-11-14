#include "core/capacitor_footprint.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
using namespace closed_circuit;
namespace
{
using Ports = std::array<PortMask, 49>;
DelayedCapacitor cap(unsigned pos, unsigned range)
{
    return *DelayedCapacitor::from_values(*EntityId::from_value(0),
                                          *NodeId::from_value(pos), 2, range);
}
bool exact(const Ports &ports, const DelayedCapacitor &value,
           std::initializer_list<unsigned> nodes)
{
    std::array<bool, 49> expected{};
    for (const auto n : nodes)
        expected[n] = true;
    const auto before = ports;
    const auto copy = value;
    return capacitor_footprint(ports, value) == expected && value == copy &&
           std::ranges::equal(ports, before, {}, &PortMask::bits,
                              &PortMask::bits);
}
bool literals()
{
    Ports ports{};
    if (!exact(ports, cap(0, 48), {0}) || !exact(ports, cap(48, 1), {48}))
        return false;
    for (const auto [n, bits] :
         {std::pair{0, 2}, {1, 10}, {2, 12}, {9, 9}, {8, 10}, {7, 2}})
        ports[n] = *PortMask::from_bits(bits);
    auto value = cap(0, 2);
    if (!exact(ports, value, {0, 1, 2}) || !exact(ports, cap(0, 1), {0, 1}) ||
        !exact(ports, cap(0, 4), {0, 1, 2, 9, 8}) ||
        !exact(ports, cap(0, 48), {0, 1, 2, 9, 8, 7}))
        return false;
    if (value.advance_fuse())
        return false;
    ports[1] = ports[1].rotate_clockwise();
    if (!exact(ports, value, {0}) || !value.advance_fuse() ||
        !exact(ports, value, {0}))
        return false;
    ports = {};
    for (const auto [n, bits] : {std::pair{0, 6}, {1, 12}, {8, 9}, {7, 3}})
        ports[n] = *PortMask::from_bits(bits);
    if (!exact(ports, cap(0, 1), {0, 1, 7}) ||
        !exact(ports, cap(0, 2), {0, 1, 7, 8}) ||
        !exact(ports, cap(0, 48), {0, 1, 7, 8}))
        return false;
    ports = {};
    for (const auto [n, bits] :
         {std::pair{24, 15}, {17, 4}, {25, 8}, {31, 1}, {23, 2}})
        ports[n] = *PortMask::from_bits(bits);
    if (!exact(ports, cap(24, 1), {24, 17, 25, 31, 23}))
        return false;
    ports = {};
    ports[0] = *PortMask::from_bits(2);
    ports[6] = *PortMask::from_bits(2);
    ports[7] = *PortMask::from_bits(8);
    return exact(ports, cap(0, 48), {0}) && exact(ports, cap(6, 48), {6});
}
bool full_grid()
{
    Ports ports{};
    ports.fill(*PortMask::from_bits(15));
    for (int source = 0; source < 49; ++source)
        for (unsigned range = 1; range <= 48; ++range)
        {
            auto value = cap(source, range);
            const auto before = value;
            const auto reached = capacitor_footprint(ports, value);
            if (value != before)
                return false;
            for (int target = 0; target < 49; ++target)
                if (reached[target] != (std::abs(source / 7 - target / 7) +
                                            std::abs(source % 7 - target % 7) <=
                                        static_cast<int>(range)))
                    return false;
            static_cast<void>(value.advance_fuse());
            static_cast<void>(value.advance_fuse());
            if (capacitor_footprint(ports, value) != reached)
                return false;
        }
    return true;
}
} // namespace
int main()
{
    if (!literals() || !full_grid())
    {
        std::cerr << "Capacitor graph footprint mismatch\n";
        return 1;
    }
    return 0;
}
