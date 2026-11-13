#pragma once
#include "core/game_state.hpp"
#include "presentation/atlas_tiles.hpp"
#include <cmath>

namespace closed_circuit::presentation
{
struct BoardLayout
{
    float left, top, cell;
    bool operator==(const BoardLayout &) const = default;
};
struct TileDraw
{
    AtlasTile tile;
    float x, y, size;
    bool operator==(const TileDraw &) const = default;
};

[[nodiscard]] inline std::optional<BoardLayout> fit_board(float width,
                                                          float height) noexcept
{
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0 ||
        height <= 0 || width > 1000000 || height > 1000000)
        return std::nullopt;
    const auto side = std::min(width, height);
    return BoardLayout{(width - side) / 2, (height - side) / 2, side / 7};
}

[[nodiscard]] inline std::optional<std::array<TileDraw, GameState::node_count>>
board_draws(const GameState &state, float width, float height) noexcept
{
    const auto layout = fit_board(width, height);
    if (!layout)
        return std::nullopt;
    std::array<TileDraw, GameState::node_count> result{};
    for (unsigned i = 0; i < result.size(); ++i)
        result[i] = {tile_for_ports(state.ports[i]),
                     layout->left + (i % 7) * layout->cell,
                     layout->top + (i / 7) * layout->cell, layout->cell};
    return result;
}
} // namespace closed_circuit::presentation
