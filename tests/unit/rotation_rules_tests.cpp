#include "../support/state_checks.hpp"
#include "core/splitmix64.hpp"
#include "core/turn_preview.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
ModuleLoadout loadout(unsigned mask)
{
    ModuleLoadout set;
    if (mask & 1)
        static_cast<void>(set.add(ModuleId::rotation_range));
    if (mask & 2)
        static_cast<void>(set.add(ModuleId::terminal_rotation));
    return set;
}
int distance(int a, int b)
{
    int rows = a / 7 - b / 7, columns = a % 7 - b % 7;
    return (rows < 0 ? -rows : rows) + (columns < 0 ? -columns : columns);
}
} // namespace
int main()
{
    SplitMix64 rng{RunSeed{1}}, reference = rng;
    for (unsigned mask = 0; mask < 4; ++mask)
    {
        const auto modules = loadout(mask);
        const int limit = (mask == 1 || mask == 3) ? 2 : 1;
        if (rotation_rules(modules).maximum_distance != limit)
            return 1;
        for (int source = 0; source < 49; ++source)
            for (int destination = 0; destination < 49; ++destination)
                for (const bool rotatable : {false, true})
                {
                    GameState state{.player_position = node(source)};
                    state.modules = modules;
                    state.ports[destination] = *PortMask::from_bits(2);
                    state.rotatable_nodes[destination] = rotatable;
                    auto expected = state;
                    const bool legal =
                        rotatable && distance(source, destination) <= limit;
                    if (legal)
                        expected.ports[destination] = *PortMask::from_bits(4);
                    if (rotate_from_player(state, node(destination)) != legal ||
                        !test::same_state(state, expected))
                    {
                        std::cerr << "Rotation range or full state mismatch\n";
                        return 1;
                    }
                }
        GameState state{.player_position = node(24)};
        state.ports.fill(*PortMask::from_bits(2));
        state.rotatable_nodes.fill(true);
        state.modules = modules;
        state.turns_taken = TurnIndex{3};
        state.overload_debt = 7;
        state.packet = Packet{node(48), node(7)};
        state.sector_entry = node(0);
        state.sector_exit = node(48);
        state.receiver_nodes[7] = state.receiver_nodes[14] = true;
        state.pulses = {{*EntityId::from_value(5), node(48)}};
        const auto before = state;
        for (int target : {24, 25, 31, 26, 32, 0})
            for (const bool overdrive : {false, true})
            {
                const Command command =
                    overdrive ? Command{OverdriveCommand{node(target)}}
                              : Command{RotateCommand{node(target)}};
                const bool legal = distance(24, target) <= limit;
                auto expected = state;
                if (legal)
                {
                    expected.ports[target] =
                        *PortMask::from_bits(overdrive ? 8 : 4);
                    expected.turns_taken = TurnIndex{4};
                    if (overdrive)
                    {
                        expected.reserve = 1;
                        expected.overload_debt = 8;
                    }
                }
                const auto preview = preview_turn(state, command);
                const auto result = resolve_turn(state, command);
                if (is_command_legal(state, command) != legal ||
                    preview.accepted != legal || result.accepted != legal ||
                    !test::same_state(preview.state, expected) ||
                    !test::same_state(result.state, expected) ||
                    !test::same_state(state, before) ||
                    rng.next_u64() != reference.next_u64())
                    return 1;
            }
    }
    return 0;
}
