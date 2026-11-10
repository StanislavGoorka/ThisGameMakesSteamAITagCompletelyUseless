#pragma once

#include "core/identifiers.hpp"

#include <cstdint>

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

  private:
    std::uint64_t state_;
};

} // namespace closed_circuit
