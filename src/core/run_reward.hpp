#pragma once

#include "core/module_choice.hpp"
#include "core/run_state.hpp"

namespace closed_circuit
{
[[nodiscard]] constexpr bool choose_run_module(RunState &run,
                                               ModuleId id) noexcept
{
    if (run_phase(run) != RunPhase::choosing_reward ||
        !choose_module(run.sector, id))
        return false;
    run.reward_claimed = true;
    return true;
}
} // namespace closed_circuit
