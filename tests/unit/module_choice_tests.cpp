#include "../support/state_checks.hpp"
#include "core/module_choice.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
// Literal default-context offers (seed 0, sector 1) for owned {}, {1}, {2}
// and {1,2} from tools/corpus/module_offer_vectors.cjs offers.
constexpr std::array<std::array<unsigned, 2>, 4> offers{
    {{1, 3}, {11, 12}, {11, 12}, {0, 0}}};
} // namespace
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
                const bool allowed = raw != 0 && (raw == offers[mask][0] ||
                                                  raw == offers[mask][1]);
                auto expected = before;
                // Expected slots: owned IDs plus the allowed one, ascending.
                std::array<unsigned, 3> ids{};
                std::size_t count = 0;
                for (unsigned id = 1; id <= 12; ++id)
                    if ((id == 1 && (mask & 1)) || (id == 2 && (mask & 2)) ||
                        (allowed && id == raw))
                        ids[count++] = id;
                ModuleLoadout slots;
                for (std::size_t i = 0; i < count; ++i)
                    if (!slots.add(static_cast<ModuleId>(ids[i])) ||
                        *slots.slots()[i] != static_cast<ModuleId>(ids[i]))
                        return 2;
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
