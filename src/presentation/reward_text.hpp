#pragma once
#include "application/reward_selection.hpp"
#include "application/text_line.hpp"

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::optional<std::vector<TextLine>>
reward_lines(const ModuleOffer &offer, std::optional<ModuleId> candidate)
{
    std::vector<TextLine> lines;
    if (!offer[0])
        return !offer[1] && !candidate ? std::optional{lines} : std::nullopt;
    if (offer[1] && *offer[1] <= *offer[0])
        return std::nullopt;
    lines.push_back({"reward-instruction", std::nullopt});
    std::optional<std::uint64_t> selected;
    for (std::size_t i = 0; i < offer.size(); ++i)
    {
        if (!offer[i])
            continue;
        const auto id = *offer[i];
        if (id != ModuleId::rotation_range && id != ModuleId::terminal_rotation)
            return std::nullopt;
        lines.push_back({id == ModuleId::rotation_range ? "reward-range"
                                                        : "reward-terminal",
                         i + 1});
        if (candidate == id)
            selected = i + 1;
    }
    if (candidate && !selected)
        return std::nullopt;
    if (selected)
        lines.push_back({"reward-selected", selected});
    return lines;
}
} // namespace closed_circuit::presentation
