#pragma once

#include "core/player_rotation.hpp"

#include <limits>

namespace closed_circuit
{

// Action only, with the range of a normal rotation plus overdrive_range's
// extra step. All rejection checks
// precede the first mutation. The second rotation cannot invalidate its target.
[[nodiscard]] constexpr bool overdrive_from_player(GameState &state,
                                                   NodeId target) noexcept
{
    const bool charge_debt =
        !state.modules.contains(ModuleId::first_overdrive_debt) ||
        state.first_overdrive_used;
    if (state.reserve == 0 ||
        (charge_debt &&
         state.overload_debt == std::numeric_limits<std::uint32_t>::max()))
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
    if (charge_debt)
        ++state.overload_debt;
    state.first_overdrive_used = true;
    return true;
}

} // namespace closed_circuit
