#pragma once

#include "core/game_state.hpp"

#include <type_traits>

namespace closed_circuit
{

// The only code that advances the turn counter. Player actions such as
// move_player and rotate_from_player never charge their own cost; an accepted
// command is charged exactly once through this function. It validates
// nothing and changes no other field.
constexpr void charge_turn(GameState &state) noexcept
{
    using Count = std::underlying_type_t<TurnIndex>;
    state.turns_taken = TurnIndex{
        static_cast<Count>(static_cast<Count>(state.turns_taken) + Count{1})};
}

} // namespace closed_circuit
