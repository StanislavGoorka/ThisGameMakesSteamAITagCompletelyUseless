#pragma once

#include "core/debt_spawn.hpp"
#include "corpus/baseline_witness.hpp"
#include "corpus/spawn_check.hpp"

namespace closed_circuit::developer
{
// Reasons 1..6 are SpawnIssue; 7 unsupported witness, 8 invalid certificate,
// 9 debt scenario mismatch. Zero means the individual check passed.
struct CorpusCheck
{
    unsigned baseline_reason{};
    std::size_t turns{};
    std::array<unsigned, 7> debt_reasons{};
};

[[nodiscard]] inline CorpusCheck check_sector_seed(RunSeed seed,
                                                   const GameState &base)
{
    CorpusCheck check;
    check.baseline_reason = static_cast<unsigned>(initial_spawn_issue(base));
    if (check.baseline_reason == 0)
    {
        const auto witness = build_baseline_witness(seed, base);
        if (!witness)
            check.baseline_reason = 7;
        else
        {
            check.turns = witness->size();
            if (!validate_solution(base, *witness))
                check.baseline_reason = 8;
        }
    }
    constexpr std::array<std::uint32_t, 7> debts{0, 2, 3, 6, 6, 6, 6};
    for (std::size_t scenario = 0; scenario < debts.size(); ++scenario)
    {
        auto state = base;
        state.overload_debt = debts[scenario];
        if (scenario == 4)
            state.sector_entry.reset();
        if (scenario == 5 && state.sector_entry)
            state.blocked_nodes[state.sector_entry->value()] = true;
        if (scenario == 6 && state.sector_entry)
            state.pulses.push_back(
                {*EntityId::from_value(9), *state.sector_entry});
        const auto before_count = state.pulses.size();
        const bool expected_spawn = scenario == 2 || scenario == 3;
        const auto expected_debt = debts[scenario] - (expected_spawn ? 3 : 0);
        const bool spawned = spawn_debt_pulse(state);
        unsigned reason = 0;
        if (spawned != expected_spawn || state.overload_debt != expected_debt ||
            state.pulses.size() != before_count + (expected_spawn ? 1 : 0))
            reason = 9;
        if (reason == 0 && expected_spawn &&
            (state.pulses.back().id != *EntityId::from_value(0) ||
             state.pulses.back().position != state.sector_entry))
            reason = 9;
        if (reason == 0 && scenario < 4)
            reason = static_cast<unsigned>(initial_spawn_issue(state));
        check.debt_reasons[scenario] = reason;
    }
    return check;
}
} // namespace closed_circuit::developer
