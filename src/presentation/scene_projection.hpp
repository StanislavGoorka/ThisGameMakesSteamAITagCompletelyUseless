#pragma once
#include "presentation/unit_projection.hpp"
#include <span>

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::optional<std::vector<TileDraw>>
scene_draws(const GameState &state, float width, float height,
            std::span<const NodeId> route = {})
{
    const auto board = board_draws(state, width, height);
    const auto units = unit_draws(state, width, height);
    if (!board || !units || route.size() > GameState::node_count)
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
    const auto layout = *fit_board(width, height);
    // Reuse the inspected packet glyph in the free left-middle quarter.
    for (const auto node : route)
        result.push_back({*tile_for_symbol(SymbolKind::packet),
                          layout.left + (node.value() % 7) * layout.cell,
                          layout.top + (node.value() / 7 + .375f) * layout.cell,
                          layout.cell * .25f});
    return result;
}
} // namespace closed_circuit::presentation
