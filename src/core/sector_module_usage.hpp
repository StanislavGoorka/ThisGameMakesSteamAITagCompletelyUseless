#pragma once

#include "core/game_state.hpp"

namespace closed_circuit
{
// Explicit sector-boundary operation. Ordinary turns never renew allowances.
constexpr void reset_sector_module_usage(GameState &state) noexcept
{
    state.terminal_rotation_used = false;
}
} // namespace closed_circuit
