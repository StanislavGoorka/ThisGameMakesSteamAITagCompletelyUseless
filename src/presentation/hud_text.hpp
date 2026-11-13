#pragma once
#include "application/text_line.hpp"
#include "presentation/hud_projection.hpp"

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::array<TextLine, 3> hud_lines(HudValues values)
{
    return {{{"hud-integrity", values.integrity},
             {"hud-turns", static_cast<std::uint32_t>(values.turns)},
             {"hud-deliveries", values.deliveries}}};
}
} // namespace closed_circuit::presentation
