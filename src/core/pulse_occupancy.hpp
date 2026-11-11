#pragma once

#include "core/game_state.hpp"

#include <algorithm>

namespace closed_circuit
{

[[nodiscard]] constexpr bool pulse_occupies(const GameState &state,
                                            NodeId position) noexcept
{
    return std::ranges::any_of(state.pulses, [position](const Pulse &pulse)
                               { return pulse.position == position; });
}

} // namespace closed_circuit
