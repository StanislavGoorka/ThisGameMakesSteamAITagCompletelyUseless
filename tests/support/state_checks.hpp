#pragma once

#include "core/game_state.hpp"

namespace closed_circuit::test
{
[[nodiscard]] inline bool same_state(const GameState &a, const GameState &b)
{
    return a.reserve == b.reserve && a.overload_debt == b.overload_debt &&
           a.player_position == b.player_position &&
           a.player_integrity == b.player_integrity &&
           a.turns_taken == b.turns_taken && a.packet == b.packet &&
           a.powered_nodes == b.powered_nodes &&
           a.receiver_nodes == b.receiver_nodes &&
           a.sector_entry == b.sector_entry && a.sector_exit == b.sector_exit &&
           a.pulses == b.pulses && a.rotatable_nodes == b.rotatable_nodes &&
           a.blocked_nodes == b.blocked_nodes &&
           std::ranges::equal(a.ports, b.ports, {}, &PortMask::bits,
                              &PortMask::bits);
}
} // namespace closed_circuit::test
