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

// Catalog projection: one registered working text ID per difficulty modifier
// bit. Unknown values have no text; nothing shows a raw bit.
[[nodiscard]] constexpr std::optional<std::string_view>
difficulty_modifier_text_id(DifficultyModifier modifier) noexcept
{
    switch (modifier)
    {
    case DifficultyModifier::short_turn_limit:
        return "difficulty-short-turns";
    case DifficultyModifier::lean_delivery:
        return "difficulty-lean-delivery";
    case DifficultyModifier::heavy_overdrive_debt:
        return "difficulty-heavy-debt";
    }
    return std::nullopt;
}

// The start and result screens name the configuration the next confirm
// starts, then each active modifier in catalog order; the run screen shows
// none. The empty mask adds no line.
[[nodiscard]] inline std::vector<TextLine>
session_lines(SessionScreen screen, StartConfigId selected,
              DifficultyModifiers modifiers)
{
    const auto id = start_config_text_id(selected);
    if (screen == SessionScreen::run || !id)
        return {};
    std::vector<TextLine> lines{TextLine{*id, std::nullopt}};
    for (const auto modifier : difficulty_modifier_catalog)
        if (const auto text = difficulty_modifier_text_id(modifier);
            text && modifiers.contains(modifier))
            lines.push_back(TextLine{*text, std::nullopt});
    return lines;
}
} // namespace closed_circuit::presentation
