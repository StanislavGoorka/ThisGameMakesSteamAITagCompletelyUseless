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

bool invalid_and_singleton_ranges_preserve_state()
{
    SplitMix64 rng{RunSeed{0}};
    if (rng.next_in_range(1, 0).has_value() ||
        rng.next_in_range(UINT64_MAX, UINT64_MAX) != UINT64_MAX ||
        rng.next_u64() != 0xe220a8397b1dcdafULL)
    {
        std::cerr << "Invalid or singleton range returned the wrong result or "
                     "advanced the RNG\n";
        return false;
    }
    return true;
}

bool full_range_matches_raw_draw()
{
    SplitMix64 bounded{RunSeed{0}};
    SplitMix64 raw{RunSeed{0}};
    if (bounded.next_in_range(0, UINT64_MAX) != raw.next_u64() ||
        bounded.next_u64() != raw.next_u64())
    {
        std::cerr << "Full range did not consume and return one raw draw\n";
        return false;
    }
    return true;
}

bool bounded_sequence_reaches_both_endpoints_and_repeats()
{
    constexpr std::uint64_t minimum = UINT64_MAX - 5;
    constexpr std::array<std::uint64_t, 8> offsets{1, 0, 1, 4, 1, 0, 5, 2};
    SplitMix64 first{RunSeed{0}};
    SplitMix64 second{RunSeed{0}};
    for (std::size_t index = 0; index < offsets.size(); ++index)
    {
        const auto expected = minimum + offsets[index];
        if (first.next_in_range(minimum, UINT64_MAX) != expected ||
            second.next_in_range(minimum, UINT64_MAX) != expected)
        {
            std::cerr << "Unexpected bounded result at draw " << index + 1
                      << '\n';
            return false;
        }
    }
    return true;
}

bool wide_range_retries_after_two_rejections()
{
    constexpr auto maximum = std::uint64_t{1} << 63;
    SplitMix64 rng{RunSeed{0}};
    // The second and third raw draws are below the threshold 2^63 - 1.
    if (rng.next_in_range(0, maximum) != 0x6220a8397b1dcdaeULL ||
        rng.next_in_range(0, maximum) != 0x788bb8a8724c81ebULL ||
        rng.next_u64() != 0x1b39896a51a8749bULL)
    {
        std::cerr << "Wide range did not skip exactly the two rejected draws\n";
        return false;
    }
    return true;
}

bool draw_equal_to_threshold_is_accepted()
{
    // This seed starts with a raw draw of 4, the threshold for a range of
    // size 6.
    SplitMix64 rng{RunSeed{0xbb7f855425049b55ULL}};
    if (rng.next_in_range(10, 15) != 14)
    {
        std::cerr << "Bounded draw rejected the threshold itself\n";
        return false;
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
        equal_seeds_repeat_across_interleaved_draws() &&
        invalid_and_singleton_ranges_preserve_state() &&
        full_range_matches_raw_draw() &&
        bounded_sequence_reaches_both_endpoints_and_repeats() &&
        wide_range_retries_after_two_rejections() &&
        draw_equal_to_threshold_is_accepted();
    return passed ? 0 : 1;
}
