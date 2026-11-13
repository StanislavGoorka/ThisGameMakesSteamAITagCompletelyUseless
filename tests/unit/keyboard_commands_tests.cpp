#include "../support/state_checks.hpp"
#include "application/keyboard_commands.hpp"
#include "core/turn_resolver.hpp"
using namespace closed_circuit;

int main()
{
    for (int row = 0; row < 7; ++row)
        for (int column = 0; column < 7; ++column)
        {
            const auto player = *NodeId::from_value(row * 7 + column);
            constexpr int dr[]{-1, 0, 1, 0}, dc[]{0, 1, 0, -1};
            for (unsigned direction = 0; direction < 4; ++direction)
            {
                const auto command =
                    map_key_command(static_cast<InputKey>(direction), player);
                const int r = row + dr[direction], c = column + dc[direction];
                const bool inside = r >= 0 && r < 7 && c >= 0 && c < 7;
                if (command.has_value() != inside)
                    return 1;
                if (inside && *command != Command{MoveCommand{
                                              *NodeId::from_value(r * 7 + c)}})
                    return 2;
            }
            if (map_key_command(InputKey::rotate, player) !=
                    Command{RotateCommand{player}} ||
                map_key_command(InputKey::overdrive, player) !=
                    Command{OverdriveCommand{player}} ||
                map_key_command(InputKey::wait, player) !=
                    Command{WaitCommand{}} ||
                map_key_command(InputKey::count, player) ||
                map_key_command(static_cast<InputKey>(999), player))
                return 3;
        }
    // A geometrical neighbor is still a candidate when no ports connect it.
    // Legality and the invariant on refusal remain in the shared resolver.
    GameState state{.player_position = *NodeId::from_value(24)};
    const auto original = state;
    const auto command = map_key_command(InputKey::east, state.player_position);
    if (!command)
        return 4;
    const auto result = resolve_turn(state, *command);
    if (result.accepted || !test::same_state(state, original) ||
        !test::same_state(result.state, original))
        return 5;
    return 0;
}
