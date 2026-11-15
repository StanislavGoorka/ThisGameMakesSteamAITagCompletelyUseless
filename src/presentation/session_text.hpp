#pragma once
#include "application/session_flow.hpp"
#include "application/text_line.hpp"
#include "core/start_config.hpp"

#include <string_view>
#include <vector>

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

// Catalog projection: one registered working text ID per implemented start
// configuration. Unknown values have no text; nothing shows a raw ID.
[[nodiscard]] constexpr std::optional<std::string_view>
start_config_text_id(StartConfigId id) noexcept
{
    switch (id)
    {
    case StartConfigId::baseline:
        return "start-config-baseline";
    case StartConfigId::preinstalled_range:
        return "start-config-range";
    case StartConfigId::charged_debt:
        return "start-config-charged";
    }
    return std::nullopt;
}

// The start and result screens name the configuration the next confirm
// starts; the run screen shows none.
[[nodiscard]] inline std::vector<TextLine> session_lines(SessionScreen screen,
                                                         StartConfigId selected)
{
    const auto id = start_config_text_id(selected);
    if (screen == SessionScreen::run || !id)
        return {};
    return {TextLine{*id, std::nullopt}};
}
} // namespace closed_circuit::presentation
