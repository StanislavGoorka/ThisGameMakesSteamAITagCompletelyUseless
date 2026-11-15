#pragma once

#include "core/game_state.hpp"
#include "core/node_rotation.hpp"
#include "core/rotation_rules.hpp"

#include <bit>

namespace closed_circuit
{

// Range depends on grid positions, not connected ports or blocking units.
// Applies only the action; the turn resolver accounts for turn costs.
// A rejected rotation leaves every field of the state untouched. Only the
// overdrive action asks for its own range.
[[nodiscard]] constexpr bool rotate_from_player(GameState &state, NodeId target,
                                                bool overdrive = false) noexcept
{
    constexpr int grid_size = 7;
    static_assert(grid_size * grid_size == GameState::node_count);

    const int source = state.player_position.value();
    const int destination = target.value();
    const auto absolute = [](int value) { return value < 0 ? -value : value; };
    const int distance =
        absolute(source / grid_size - destination / grid_size) +
        absolute(source % grid_size - destination % grid_size);
    const auto rules = rotation_rules(state.modules);
    if (distance >
        (overdrive ? rules.overdrive_distance : rules.maximum_distance))
    {
        return false;
    }

    const bool rotatable = state.rotatable_nodes[target.value()];
    if (!rotatable &&
        (!rules.allows_fixed_terminal || state.terminal_rotation_used ||
         !std::has_single_bit(state.ports[target.value()].bits())))
        return false;

    // The overdrive's own second quarter turn follows in its action.
    auto &ports = state.ports[target.value()];
    for (int turn = 0; turn < (overdrive ? 1 : rules.quarter_turns); ++turn)
        ports = ports.rotate_clockwise();
    // A fixed terminal uses the shared allowance once per accepted rotation.
    if (!rotatable)
        state.terminal_rotation_used = true;
    return true;
}

} // namespace closed_circuit
