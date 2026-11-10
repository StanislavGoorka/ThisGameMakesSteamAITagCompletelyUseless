#pragma once

#include "core/identifiers.hpp"

#include <cstdint>
#include <optional>

namespace closed_circuit
{

// Fixed-increment SplitMix64. The seed is the initial state, before any draw.
class SplitMix64
{
  public:
    explicit constexpr SplitMix64(RunSeed seed) noexcept
        : state_{static_cast<std::uint64_t>(seed)}
    {
    }

    [[nodiscard]] constexpr std::uint64_t next_u64() noexcept
    {
        // Advance the state and mix its new value using arithmetic modulo 2^64.
        state_ += 0x9e3779b97f4a7c15ULL;
        auto mixed = state_;
        mixed = (mixed ^ (mixed >> 30)) * 0xbf58476d1ce4e5b9ULL;
        mixed = (mixed ^ (mixed >> 27)) * 0x94d049bb133111ebULL;
        return mixed ^ (mixed >> 31);
    }

    // Both endpoints are inclusive. Invalid and singleton ranges consume no
    // draws.
    [[nodiscard]] constexpr std::optional<std::uint64_t>
    next_in_range(std::uint64_t minimum, std::uint64_t maximum) noexcept
    {
        if (minimum > maximum)
        {
            return std::nullopt;
        }
        if (minimum == maximum)
        {
            return minimum;
        }

        const std::uint64_t count = maximum - minimum + 1;
        if (count == 0)
        {
            // Only the full 64-bit range has a count of 2^64, which wraps to
            // zero.
            return next_u64();
        }

        // Unsigned wrap computes 2^64 % count. Removing that many low values
        // leaves a whole number of residue cycles for the modulo operation.
        const auto threshold = (std::uint64_t{0} - count) % count;
        auto draw = next_u64();
        while (draw < threshold)
        {
            draw = next_u64();
        }
        return minimum + draw % count;
    }

  private:
    std::uint64_t state_;
};

} // namespace closed_circuit
