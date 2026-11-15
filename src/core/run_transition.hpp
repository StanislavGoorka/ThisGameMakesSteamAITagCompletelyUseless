#pragma once

#include "core/debt_spawn.hpp"
#include "core/run_sector_variant.hpp"
#include "core/run_turn.hpp"
#include "core/sector_module_usage.hpp"

namespace closed_circuit
{
[[nodiscard]] inline RunResult advance_run(const RunState &run)
{
    RunResult result{run, false, run_phase(run)};
    if (result.phase != RunPhase::ready_for_next)
        return result;
    const auto number =
        *SectorNumber::from_value(run.sector_number.value() + 1);
    SplitMix64 rng{run.seed};
    RunSeed sector_seed{0};
    for (unsigned i = 0; i < number.value(); ++i)
        sector_seed = RunSeed{rng.next_u64()};
    auto next = *build_sector_state(sector_seed, sector_variant_for(number));
    next.player_integrity = run.sector.player_integrity;
    next.reserve = run.sector.reserve;
    next.overload_debt = run.sector.overload_debt;
    next.modules = run.sector.modules;
    reset_sector_module_usage(next);
    static_cast<void>(spawn_debt_pulse(next));
    result.state = RunState{.seed = run.seed,
                            .sector_number = number,
                            .sector = std::move(next),
                            .reward_claimed = false,
                            .start_config = run.start_config};
    result.accepted = true;
    result.phase = run_phase(result.state);
    return result;
}
} // namespace closed_circuit
