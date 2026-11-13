#pragma once
#include "application/session_flow.hpp"
#include "application/text_line.hpp"

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::optional<TextLine>
session_message(SessionScreen screen, const RunState &run) noexcept
{
    if (screen == SessionScreen::start)
        return TextLine{"session-start", std::nullopt};
    if (screen != SessionScreen::result)
        return std::nullopt;
    if (run_phase(run) == RunPhase::won)
        return TextLine{"session-won", std::nullopt};
    if (run_phase(run) != RunPhase::lost)
        return std::nullopt;
    return TextLine{sector_outcome(run.sector) == SectorOutcome::death
                        ? "session-death"
                        : "session-overload",
                    std::nullopt};
}
} // namespace closed_circuit::presentation
