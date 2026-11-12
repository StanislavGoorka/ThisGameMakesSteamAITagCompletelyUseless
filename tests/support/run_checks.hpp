#pragma once
#include "core/run_state.hpp"
#include "state_checks.hpp"
namespace closed_circuit::test
{
[[nodiscard]] inline bool same_run(const RunState &a, const RunState &b)
{
    return a.seed == b.seed && a.sector_number == b.sector_number &&
           a.reward_claimed == b.reward_claimed &&
           same_state(a.sector, b.sector);
}
} // namespace closed_circuit::test
