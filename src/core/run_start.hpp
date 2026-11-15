#pragma once

#include "core/run_sector_variant.hpp"
#include "core/run_state.hpp"
#include "core/start_config.hpp"

#include <optional>
#include <utility>

namespace closed_circuit
{
// A fresh run has no inherited rewards or per-sector usage. Sector N will use
// the Nth local SplitMix64 draw; this creates only sector 1. The sector seed
// never depends on the configuration. An unimplemented ID creates no run.
[[nodiscard]] inline std::optional<RunState> start_run(RunSeed seed,
                                                       StartConfigId config)
{
    if (!is_known_start_config(config))
        return std::nullopt;
    SplitMix64 rng{seed};
    const auto number = *SectorNumber::from_value(1);
    auto state = *build_sector_state(RunSeed{rng.next_u64()},
                                     sector_variant_for(number));
    // baseline keeps the GameState defaults: HP3, reserve2, debt0, no module.
    return RunState{.seed = seed,
                    .sector_number = number,
                    .sector = std::move(state),
                    .start_config = config};
}

[[nodiscard]] inline RunState start_run(RunSeed seed)
{
    return *start_run(seed, StartConfigId::baseline);
}
} // namespace closed_circuit
