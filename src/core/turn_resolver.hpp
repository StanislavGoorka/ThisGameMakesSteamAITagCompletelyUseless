#pragma once

#include "core/command.hpp"
#include "core/packet_delivery.hpp"
#include "core/packet_introduction.hpp"
#include "core/packet_movement.hpp"
#include "core/pulse_phase.hpp"
#include "core/turn_cost.hpp"

#include <type_traits>
#include <variant>

namespace closed_circuit
{

struct TurnResult
{
    GameState state;
    bool accepted;
};

// A rejected command runs no phase. The result owns its state in both cases;
// accepted commands run each phase once and pay one final turn cost.
[[nodiscard]] inline TurnResult resolve_turn(const GameState &state,
                                             const Command &command)
{
    const bool accepted = is_command_legal(state, command);
    TurnResult result{state, accepted};
    if (!accepted)
    {
        return result;
    }

    auto &next = result.state;
    static_cast<void>(introduce_packet(next));
    std::visit(
        [&next](const auto &action)
        {
            using Action = std::decay_t<decltype(action)>;
            if constexpr (std::is_same_v<Action, MoveCommand>)
            {
                static_cast<void>(move_player(next, action.target));
            }
            else if constexpr (std::is_same_v<Action, RotateCommand>)
            {
                static_cast<void>(rotate_from_player(next, action.target));
            }
            // A validated wait has no board effect; the common cost is below.
        },
        command);
    static_cast<void>(advance_packet(next));
    static_cast<void>(deliver_packet(next));
    resolve_pulse_phase(next);
    charge_turn(next);
    return result;
}

} // namespace closed_circuit
