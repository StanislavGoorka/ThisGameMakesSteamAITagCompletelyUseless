#include "../support/state_checks.hpp"
#include "core/turn_preview.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
GameState scenario(unsigned mask)
{
    GameState state{.player_position = node(0)};
    state.ports[0] = *PortMask::from_bits(2);
    state.ports[1] = *PortMask::from_bits(10);
    state.ports[2] = *PortMask::from_bits(6);
    state.ports[3] = *PortMask::from_bits(2);
    state.ports[9] = *PortMask::from_bits(1);
    state.rotatable_nodes[2] = state.rotatable_nodes[3] = true;
    state.blocked_nodes[2] = true;
    state.packet = Packet{node(1), node(9)};
    state.pulses = {{*EntityId::from_value(0), node(9)}};
    state.sector_entry = node(0);
    state.sector_exit = node(48);
    state.receiver_nodes[9] = state.receiver_nodes[16] = true;
    if (mask & 1)
        static_cast<void>(state.modules.add(ModuleId::rotation_range));
    if (mask & 2)
        static_cast<void>(state.modules.add(ModuleId::terminal_rotation));
    return state;
}
} // namespace
int main()
{
    for (unsigned mask = 0; mask < 4; ++mask)
    {
        const auto state = scenario(mask);
        const auto before = state;
        const bool legal = mask == 1 || mask == 3;
        const Command command = RotateCommand{node(2)};
        auto expected = state;
        if (legal)
        {
            expected.ports[2] = *PortMask::from_bits(12);
            expected.packet->position = node(2);
            expected.pulses[0].position = node(2);
            expected.turns_taken = TurnIndex{1};
        }
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto preview = preview_turn(state, command);
            const auto turn = resolve_turn(state, command);
            if (is_command_legal(state, command) != legal ||
                preview.accepted != legal || turn.accepted != legal ||
                preview.outcome != SectorOutcome::ongoing ||
                turn.outcome != SectorOutcome::ongoing ||
                !test::same_state(preview.state, expected) ||
                !test::same_state(turn.state, expected) ||
                !test::same_state(state, before))
            {
                std::cerr << "Range module shared-route state mismatch\n";
                return 1;
            }
        }
        if (legal)
        {
            const auto delivered = resolve_turn(expected, WaitCommand{});
            expected.packet.reset();
            expected.powered_nodes[9] = true;
            expected.reserve = 3;
            expected.pulses[0].position = node(1);
            expected.turns_taken = TurnIndex{2};
            if (!delivered.accepted ||
                !test::same_state(delivered.state, expected))
                return 1;
        }
        for (const bool fixed : {false, true})
        {
            auto refused = state;
            if (fixed)
                refused.rotatable_nodes[2] = false;
            const auto copy = refused;
            const Command invalid = RotateCommand{node(fixed ? 2 : 3)};
            const auto result = resolve_turn(refused, invalid);
            if (result.accepted || is_command_legal(refused, invalid) ||
                !test::same_state(result.state, copy) ||
                !test::same_state(refused, copy))
                return 1;
        }
    }
    return 0;
}
