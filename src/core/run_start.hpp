#pragma once

#include "core/run_state.hpp"
#include "core/sector_boundary.hpp"

#include <utility>

namespace closed_circuit
{
// A fresh run has no inherited resources, rewards or per-sector usage.
// Sector N will use the Nth local SplitMix64 draw; this creates only sector 1.
[[nodiscard]] inline RunState start_run(RunSeed seed)
{
    SplitMix64 rng{seed};
    auto state =
        *build_sector_state(RunSeed{rng.next_u64()}, SectorVariant::normal);
    return RunState{.seed = seed, .sector = std::move(state)};
}
} // namespace closed_circuit
