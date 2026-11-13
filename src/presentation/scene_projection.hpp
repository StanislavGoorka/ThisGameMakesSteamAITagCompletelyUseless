#pragma once
#include "presentation/unit_projection.hpp"

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::optional<std::vector<TileDraw>>
scene_draws(const GameState &state, float width, float height)
{
    const auto board = board_draws(state, width, height);
    const auto units = unit_draws(state, width, height);
    if (!board || !units)
        return std::nullopt;
    std::vector<TileDraw> result;
    result.reserve(board->size() + units->size());
    for (auto tile : *board)
    {
        // The middle half and the four quarter-size corners are disjoint.
        tile.x += tile.size * .25f;
        tile.y += tile.size * .25f;
        tile.size *= .5f;
        result.push_back(tile);
    }
    for (const auto &symbol : *units)
        result.push_back(symbol.draw);
    return result;
}
} // namespace closed_circuit::presentation
