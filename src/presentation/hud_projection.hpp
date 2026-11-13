#pragma once
#include "core/game_state.hpp"

namespace closed_circuit::presentation
{
struct HudValues
{
    std::uint8_t integrity;
    TurnIndex turns;
    std::size_t deliveries;
    bool operator==(const HudValues &) const = default;
};

[[nodiscard]] constexpr HudValues hud_values(const GameState &state) noexcept
{
    std::size_t deliveries = 0;
    for (std::size_t i = 0; i < GameState::node_count; ++i)
        if (state.receiver_nodes[i] && state.powered_nodes[i])
            ++deliveries;
    return {state.player_integrity, state.turns_taken, deliveries};
}
} // namespace closed_circuit::presentation
