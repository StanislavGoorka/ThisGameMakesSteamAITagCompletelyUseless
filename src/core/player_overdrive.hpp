#pragma once

#include "core/player_rotation.hpp"

#include <limits>

namespace closed_circuit
{

// Action only, with the same range as a normal rotation. All rejection checks
// precede the first mutation. The second rotation cannot invalidate its target.
[[nodiscard]] constexpr bool overdrive_from_player(GameState &state,
                                                   NodeId target) noexcept
{
    if (state.reserve == 0 ||
        state.overload_debt == std::numeric_limits<std::uint32_t>::max())
    {
        return false;
    }
    if (!rotate_from_player(state, target))
    {
        return false;
    }
    state.ports[target.value()] =
        state.ports[target.value()].rotate_clockwise();
    --state.reserve;
    ++state.overload_debt;
    return true;
}

} // namespace closed_circuit
