#pragma once

#include "core/player_rotation.hpp"

#include <limits>

namespace closed_circuit
{

// Action only, with the range of a normal rotation plus overdrive_range's
// extra step. A charged overdrive costs one debt, two with
// heavy_overdrive_debt; first_overdrive_debt waives the first. All rejection
// checks precede the first mutation, including a debt that would overflow.
// The second rotation cannot invalidate its target.
[[nodiscard]] constexpr bool overdrive_from_player(GameState &state,
                                                   NodeId target) noexcept
{
    const bool charge_debt =
        !state.modules.contains(ModuleId::first_overdrive_debt) ||
        state.first_overdrive_used;
    const std::uint32_t debt_cost =
        !charge_debt ? 0
        : state.difficulty.contains(DifficultyModifier::heavy_overdrive_debt)
            ? 2
            : 1;
    if (state.reserve == 0 ||
        state.overload_debt >
            std::numeric_limits<std::uint32_t>::max() - debt_cost)
    {
        return false;
    }
    if (!rotate_from_player(state, target, true))
    {
        return false;
    }
    state.ports[target.value()] =
        state.ports[target.value()].rotate_clockwise();
    --state.reserve;
    state.overload_debt += debt_cost;
    state.first_overdrive_used = true;
    return true;
}

} // namespace closed_circuit
