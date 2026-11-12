#include "../support/state_checks.hpp"
#include "core/module_choice.hpp"
#include <iostream>
using namespace closed_circuit;
int main()
{
    const auto zero = *NodeId::from_value(0);
    const auto last = *NodeId::from_value(48);
    for (unsigned mask = 0; mask < 4; ++mask)
        for (const bool used : {false, true})
            for (unsigned raw = 0; raw <= 255; ++raw)
            {
                GameState state{.player_position = last};
                state.ports[0] = *PortMask::from_bits(15);
                state.rotatable_nodes[2] = state.blocked_nodes[3] = true;
                state.powered_nodes[4] = state.receiver_nodes[4] = true;
                state.packet = Packet{zero, last};
                state.pulses = {{*EntityId::from_value(9), zero}};
                state.sector_entry = zero;
                state.sector_exit = last;
                state.player_integrity = 1;
                state.turns_taken = TurnIndex{48};
                state.reserve = 1;
                state.overload_debt = 6;
                state.terminal_rotation_used = used;
                if (mask & 1)
                    static_cast<void>(
                        state.modules.add(ModuleId::rotation_range));
                if (mask & 2)
                    static_cast<void>(
                        state.modules.add(ModuleId::terminal_rotation));
                const auto before = state;
                const bool allowed =
                    (raw == 1 && !(mask & 1)) || (raw == 2 && !(mask & 2));
                auto expected = before;
                // Expected slot data is specified independently, including
                // sorting.
                ModuleLoadout slots;
                const bool has_range = (mask & 1) || (allowed && raw == 1);
                const bool has_terminal = (mask & 2) || (allowed && raw == 2);
                if (has_range)
                    static_cast<void>(slots.add(ModuleId::rotation_range));
                if (has_terminal)
                    static_cast<void>(slots.add(ModuleId::terminal_rotation));
                expected.modules = slots;
                const auto id = static_cast<ModuleId>(raw);
                if (choose_module(state, id) != allowed ||
                    !test::same_state(state, expected))
                {
                    std::cerr << "Atomic module choice full-state mismatch\n";
                    return 1;
                }
                if (choose_module(state, id) ||
                    !test::same_state(state, expected))
                    return 1;
                if (allowed &&
                    state.modules.size() != before.modules.size() + 1)
                    return 1;
            }
    return 0;
}
