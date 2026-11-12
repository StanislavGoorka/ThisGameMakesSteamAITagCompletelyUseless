#pragma once

#include "core/game_state.hpp"
#include "core/module_offer.hpp"

namespace closed_circuit
{
// Low-level installation only. The run owns the post-sector reward window.
// Always recompute the offer from the live loadout, never accept stale offers.
[[nodiscard]] constexpr bool choose_module(GameState &state,
                                           ModuleId id) noexcept
{
    for (const auto candidate : module_offer(state.modules))
        if (candidate == id)
            return state.modules.add(id);
    return false;
}
} // namespace closed_circuit
