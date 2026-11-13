#pragma once
#include "application/keyboard_input.hpp"
#include "core/command.hpp"

namespace closed_circuit
{
// Produces a candidate only. The resolver owns legality and turn costs.
[[nodiscard]] inline std::optional<Command> map_key_command(InputKey key,
                                                            NodeId player)
{
    const int value = player.value();
    switch (key)
    {
    case InputKey::north:
        if (value >= 7)
            return MoveCommand{*NodeId::from_value(value - 7)};
        break;
    case InputKey::east:
        if (value % 7 < 6)
            return MoveCommand{*NodeId::from_value(value + 1)};
        break;
    case InputKey::south:
        if (value < 42)
            return MoveCommand{*NodeId::from_value(value + 7)};
        break;
    case InputKey::west:
        if (value % 7 > 0)
            return MoveCommand{*NodeId::from_value(value - 1)};
        break;
    case InputKey::rotate:
        return RotateCommand{player};
    case InputKey::overdrive:
        return OverdriveCommand{player};
    case InputKey::wait:
        return WaitCommand{};
    default:
        break;
    }
    return std::nullopt;
}
} // namespace closed_circuit
