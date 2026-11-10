#include "core/splitmix64.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

using closed_circuit::RunSeed;
using closed_circuit::SplitMix64;

namespace
{

bool matches_reference_sequence(RunSeed seed,
                                const std::array<std::uint64_t, 4> &expected)
{
    SplitMix64 rng{seed};
    for (std::size_t index = 0; index < expected.size(); ++index)
    {
        const auto result = rng.next_u64();
        if (result != expected[index])
        {
            std::cerr << "Draw " << index + 1 << " for seed 0x" << std::hex
                      << static_cast<std::uint64_t>(seed) << ": expected 0x"
                      << expected[index] << ", got 0x" << result << '\n';
            return false;
        }
    }
    return true;
}

bool equal_seeds_repeat_across_interleaved_draws()
{
    constexpr RunSeed seed{0x0123456789abcdefULL};
    SplitMix64 first{seed};
    SplitMix64 second{seed};
    for (int index = 0; index < 64; ++index)
    {
        const auto expected = first.next_u64();
        if (second.next_u64() != expected)
        {
            std::cerr << "Equal seeds diverged at draw " << index + 1 << '\n';
            return false;
        }
    }
    return true;
}

} // namespace

int main()
{
    constexpr std::array<std::uint64_t, 4> zero_seed_values{
        0xe220a8397b1dcdafULL, 0x6e789e6aa1b965f4ULL, 0x06c45d188009454fULL,
        0xf88bb8a8724c81ecULL};
    constexpr std::array<std::uint64_t, 4> max_seed_values{
        0xe4d971771b652c20ULL, 0xe99ff867dbf682c9ULL, 0x382ff84cb27281e9ULL,
        0x6d1db36ccba982d2ULL};

    const bool passed =
        matches_reference_sequence(RunSeed{0}, zero_seed_values) &&
        matches_reference_sequence(RunSeed{UINT64_MAX}, max_seed_values) &&
        equal_seeds_repeat_across_interleaved_draws();
    return passed ? 0 : 1;
}
