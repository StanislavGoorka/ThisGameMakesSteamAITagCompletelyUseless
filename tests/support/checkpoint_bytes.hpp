#pragma once
#include "application/checkpoint_format.hpp"

namespace closed_circuit::test
{
// Test-side checksum calculation, independent of encoder/decoder helpers.
// Checked against all four literal vectors before using it to reseal mutations.
inline void reseal(CheckpointBytes &bytes)
{
    std::uint64_t hash = 0xcbf29ce484222325ull;
    for (unsigned i = 0; i < 32; ++i)
    {
        hash ^= bytes[i];
        hash *= 0x100000001b3ull;
    }
    for (unsigned i = 0; i < 8; ++i)
    {
        bytes[32 + i] = static_cast<std::uint8_t>(hash & 255);
        hash >>= 8;
    }
}
} // namespace closed_circuit::test
