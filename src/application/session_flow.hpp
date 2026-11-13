#pragma once
#include "core/run_start.hpp"

namespace closed_circuit
{
enum class SessionScreen
{
    start,
    run,
    result
};

// Owns only navigation. Run rules and profile lifetime remain separate.
class SessionFlow
{
  public:
    explicit SessionFlow(bool started = false) noexcept : started_(started)
    {
    }
    [[nodiscard]] SessionScreen screen(const RunState &run) const noexcept
    {
        if (!started_)
            return SessionScreen::start;
        const auto phase = run_phase(run);
        return phase == RunPhase::won || phase == RunPhase::lost
                   ? SessionScreen::result
                   : SessionScreen::run;
    }
    [[nodiscard]] bool confirm(RunState &run)
    {
        if (screen(run) == SessionScreen::run)
            return false;
        auto fresh = start_run(run.seed);
        run = std::move(fresh);
        started_ = true;
        return true;
    }

  private:
    bool started_;
};
} // namespace closed_circuit
