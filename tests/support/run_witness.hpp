#pragma once

#include "application/reward_selection.hpp"
#include "core/run_reward.hpp"
#include "core/run_sector_variant.hpp"
#include "core/run_transition.hpp"
#include "core/run_turn.hpp"
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

inline std::optional<ModuleId> witness_choice(const RunState &run);

// True when the certified module-free commands complete this sector and all
// later ones, with every reward taken by witness_choice.
inline bool witness_completes(RunState run)
{
    for (;;)
    {
        const auto commands = sector_commands(run);
        if (!commands)
            return false;
        for (const auto &command : *commands)
        {
            auto turn = resolve_run_turn(run, command);
            if (!turn.accepted)
                return false;
            run = std::move(turn.state);
        }
        const auto phase = run_phase(run);
        if (phase == RunPhase::won)
            return true;
        if (phase == RunPhase::choosing_reward)
        {
            const auto choice = witness_choice(run);
            if (!choice || !choose_run_module(run, *choice))
                return false;
        }
        auto next = advance_run(run);
        if (!next.accepted)
            return false;
        run = std::move(next.state);
    }
}

// The certificates do not cover every module's dynamics, so a deterministic
// test run takes the first entry of the live offer that keeps them valid.
inline std::optional<ModuleId> witness_choice(const RunState &run)
{
    for (const auto candidate : reward_offer(run))
    {
        auto copy = run;
        if (!candidate || !choose_run_module(copy, *candidate))
            continue;
        const auto next = advance_run(copy);
        if (next.accepted && witness_completes(next.state))
            return candidate;
    }
    return std::nullopt;
}
} // namespace closed_circuit::test
