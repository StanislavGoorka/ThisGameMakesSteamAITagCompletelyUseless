#pragma once

#include "core/splitmix64.hpp"

#include <array>
#include <utility>

namespace closed_circuit
{
using ModuleOfferIndices = std::array<std::optional<std::size_t>, 2>;

// Numeric pool indices only; no persistent RNG or module registration.
[[nodiscard]] constexpr ModuleOfferIndices
module_offer_indices(std::size_t count, RunSeed seed,
                     std::uint8_t completed_sector) noexcept
{
    if (count == 0 || count > 12)
        return {};
    if (count == 1)
        return {0, std::nullopt};
    if (count == 2)
        return {0, 1};
    SplitMix64 local{
        RunSeed{static_cast<std::uint64_t>(seed) ^
                (std::uint64_t{completed_sector} * 0x9e3779b97f4a7c15ULL)}};
    auto first = static_cast<std::size_t>(*local.next_in_range(0, count - 1));
    auto second = static_cast<std::size_t>(*local.next_in_range(0, count - 2));
    if (second >= first)
        ++second;
    if (second < first)
        std::swap(first, second);
    return {first, second};
}
} // namespace closed_circuit
