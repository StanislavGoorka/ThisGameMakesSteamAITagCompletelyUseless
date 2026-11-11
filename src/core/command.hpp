#pragma once

#include "core/player_movement.hpp"
#include "core/player_rotation.hpp"
#include "core/sector_outcome.hpp"

#include <type_traits>
#include <variant>

namespace closed_circuit
{

struct MoveCommand
{
    NodeId target;

    [[nodiscard]] constexpr bool
    operator==(const MoveCommand &) const noexcept = default;
};

struct RotateCommand
{
    NodeId target;

    [[nodiscard]] constexpr bool
    operator==(const RotateCommand &) const noexcept = default;
};

struct WaitCommand
{
    [[nodiscard]] constexpr bool
    operator==(const WaitCommand &) const noexcept = default;
};

using Command = std::variant<MoveCommand, RotateCommand, WaitCommand>;

// Reuse the action rules on a private copy. Unlike the action-only move and
// rotation helpers, every command requires an ongoing sector. Wait has no
// board effect and checking it never charges a turn.
[[nodiscard]] inline bool is_command_legal(const GameState &state,
                                           const Command &command)
{
    if (sector_outcome(state) != SectorOutcome::ongoing)
    {
        return false;
    }

    return std::visit(
        [&state](const auto &action)
        {
            using Action = std::decay_t<decltype(action)>;
            if constexpr (std::is_same_v<Action, WaitCommand>)
            {
                return true;
            }
            else
            {
                auto candidate = state;
                if constexpr (std::is_same_v<Action, MoveCommand>)
                {
                    return move_player(candidate, action.target);
                }
                else
                {
                    return rotate_from_player(candidate, action.target);
                }
            }
        },
        command);
}

} // namespace closed_circuit
