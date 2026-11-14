#pragma once

#include "core/run_sector_variant.hpp"
#include "corpus/final_witness.hpp"

namespace closed_circuit::test
{
// Reuse certified commands, leaving the actual run's inherited resources
// intact.
inline std::optional<std::vector<Command>> sector_commands(const RunState &run)
{
    SplitMix64 rng(run.seed);
    RunSeed seed{0};
    for (unsigned i = 0; i < run.sector_number.value(); ++i)
        seed = RunSeed{rng.next_u64()};
    const auto variant = sector_variant_for(run.sector_number);
    const auto baseline = *build_sector_state(seed, variant);
    return variant == SectorVariant::final
               ? developer::build_final_witness(seed, baseline)
               : developer::build_baseline_witness(seed, baseline);
}
} // namespace closed_circuit::test
