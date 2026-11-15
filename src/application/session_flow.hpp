#pragma once
#include "core/run_start.hpp"

#include <algorithm>
#include <optional>

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
        // Restart keeps the run's configuration unless another was selected,
        // and always its difficulty modifiers; an unknown ID changes nothing.
        auto fresh = start_run(run.seed, selected(run), run.sector.difficulty);
        if (!fresh)
            return false;
        run = std::move(*fresh);
        choice_.reset();
        started_ = true;
        return true;
    }
    // The configuration the next confirm starts. Without a selection this is
    // the run's own configuration, so a plain restart keeps it.
    [[nodiscard]] StartConfigId selected(const RunState &run) const noexcept
    {
        return choice_.value_or(run.start_config);
    }
    // Cycle through implemented configurations in canonical order, only on
    // the start and result screens; a refusal changes nothing.
    [[nodiscard]] bool select_next(const RunState &run) noexcept
    {
        return step(run, true);
    }
    [[nodiscard]] bool select_previous(const RunState &run) noexcept
    {
        return step(run, false);
    }

  private:
    [[nodiscard]] bool step(const RunState &run, bool forward) noexcept
    {
        if (screen(run) == SessionScreen::run)
            return false;
        const auto &catalog = start_config_catalog;
        const auto found = std::ranges::find(catalog, selected(run));
        if (found == catalog.end())
        {
            choice_ = forward ? catalog.front() : catalog.back();
            return true;
        }
        const auto index = static_cast<std::size_t>(found - catalog.begin());
        choice_ = catalog[(index + (forward ? 1 : catalog.size() - 1)) %
                          catalog.size()];
        return true;
    }

    bool started_;
    std::optional<StartConfigId> choice_;
};
} // namespace closed_circuit
