#include "core/port_mask.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <limits>

using closed_circuit::PortMask;

static_assert(PortMask::north_bit == 1);
static_assert(PortMask::east_bit == 2);
static_assert(PortMask::south_bit == 4);
static_assert(PortMask::west_bit == 8);

int main()
{
    constexpr std::array<std::uint8_t, 16> clockwise_bits{
        0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15};

    for (std::uint8_t bits = 0; bits < clockwise_bits.size(); ++bits)
    {
        const auto mask = PortMask::from_bits(bits);
        if (!mask || mask->bits() != bits)
        {
            std::cerr << "PortMask failed to preserve valid bits " << +bits
                      << '\n';
            return 1;
        }

        const auto rotated = mask->rotate_clockwise();
        if (rotated.bits() != clockwise_bits[bits])
        {
            std::cerr << "Incorrect clockwise rotation for mask " << +bits
                      << '\n';
            return 1;
        }
        if (std::popcount(rotated.bits()) != std::popcount(bits))
        {
            std::cerr << "Rotation changed the port count for mask " << +bits
                      << '\n';
            return 1;
        }

        auto full_turn = rotated;
        for (int turn = 1; turn < 4; ++turn)
        {
            full_turn = full_turn.rotate_clockwise();
        }
        if (full_turn.bits() != bits)
        {
            std::cerr << "Four rotations changed mask " << +bits << '\n';
            return 1;
        }
    }

    if (PortMask::from_bits(-1) || PortMask::from_bits(16) ||
        PortMask::from_bits(256) ||
        PortMask::from_bits(std::numeric_limits<std::uint64_t>::max()))
    {
        std::cerr << "PortMask accepted bits outside the four-bit range\n";
        return 1;
    }
    return 0;
}
