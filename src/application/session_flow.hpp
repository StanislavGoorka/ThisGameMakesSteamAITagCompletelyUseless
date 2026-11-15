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
        // Restart keeps the run's configuration and modifiers unless others
        // were selected; an unknown ID changes nothing.
        auto fresh =
            start_run(run.seed, selected(run), selected_modifiers(run));
        if (!fresh)
            return false;
        run = std::move(*fresh);
        choice_.reset();
        modifier_choice_.reset();
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
    // The modifiers the next confirm starts, independent of the configuration:
    // without a selection the run's own, so a fresh session starts with none.
    [[nodiscard]] DifficultyModifiers
    selected_modifiers(const RunState &run) const noexcept
    {
        return modifier_choice_.value_or(run.sector.difficulty);
    }
    // Cycle through every mask of implemented bits in ascending order, only on
    // the start and result screens; a refusal changes nothing.
    [[nodiscard]] bool select_next_modifiers(const RunState &run) noexcept
    {
        return step_modifiers(run, true);
    }
    [[nodiscard]] bool select_previous_modifiers(const RunState &run) noexcept
    {
        return step_modifiers(run, false);
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

    [[nodiscard]] bool step_modifiers(const RunState &run,
                                      bool forward) noexcept
    {
        if (screen(run) == SessionScreen::run)
            return false;
        // The empty mask is always implemented, so the search terminates.
        auto bits = selected_modifiers(run).bits();
        std::optional<DifficultyModifiers> next;
        while (!next)
        {
            bits = static_cast<std::uint8_t>(bits + (forward ? 1 : 255));
            next = DifficultyModifiers::from_value(bits);
        }
        modifier_choice_ = next;
        return true;
    }

    bool started_;
    std::optional<StartConfigId> choice_;
    std::optional<DifficultyModifiers> modifier_choice_;
};
} // namespace closed_circuit
