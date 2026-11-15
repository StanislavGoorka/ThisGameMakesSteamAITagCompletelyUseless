#include "../support/state_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/turn_preview.hpp"

#include <limits>

using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
// Independent grid metric: row and column from the raw index.
int distance(int a, int b)
{
    const int rows = a / 7 - b / 7, columns = a % 7 - b % 7;
    return (rows < 0 ? -rows : rows) + (columns < 0 ? -columns : columns);
}
bool same_turns(const GameState &state, const Command &command,
                const std::optional<GameState> &expected)
{
    const auto before = state;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_turn(state, command);
        const auto result = resolve_turn(state, command);
        if (is_command_legal(state, command) != expected.has_value() ||
            preview.accepted != expected.has_value() ||
            result.accepted != expected.has_value() ||
            !test::same_state(preview.state, expected.value_or(before)) ||
            !test::same_state(result.state, expected.value_or(before)) ||
            !test::same_state(state, before))
            return false;
    }
    return true;
}
ModuleLoadout loadout(std::initializer_list<ModuleId> ids)
{
    ModuleLoadout modules;
    for (const auto id : ids)
        if (!modules.add(id))
            throw 0;
    return modules;
}
} // namespace
int main()
{
    struct Set
    {
        ModuleLoadout modules;
        int rotate;
        int overdrive;
    };
    const std::array sets{
        Set{{}, 1, 1}, Set{loadout({ModuleId::rotation_range}), 2, 2},
        Set{loadout({ModuleId::overdrive_range}), 1, 2},
        Set{loadout({ModuleId::rotation_range, ModuleId::overdrive_range}), 2,
            3},
        Set{loadout({ModuleId::overdrive_range, ModuleId::delivery_reserve}), 1,
            2}};
    // Every source and target, including corners and edges without wrapping.
    for (const auto &set : sets)
        for (int source = 0; source < 49; ++source)
            for (int target = 0; target < 49; ++target)
            {
                GameState state{.player_position = node(source)};
                state.modules = set.modules;
                state.rotatable_nodes[target] = true;
                state.ports[target] = *PortMask::from_bits(3);
                const int d = distance(source, target);
                std::optional<GameState> rotated, overdriven;
                if (d <= set.rotate)
                {
                    rotated = state;
                    rotated->ports[target] = *PortMask::from_bits(6);
                    rotated->turns_taken = TurnIndex{1};
                }
                if (d <= set.overdrive)
                {
                    overdriven = state;
                    overdriven->ports[target] = *PortMask::from_bits(12);
                    overdriven->turns_taken = TurnIndex{1};
                    overdriven->reserve = 1;
                    overdriven->overload_debt = 1;
                    overdriven->first_overdrive_used = true;
                }
                if (!same_turns(state, RotateCommand{node(target)}, rotated) ||
                    !same_turns(state, OverdriveCommand{node(target)},
                                overdriven))
                    return 1;
                // Resource refusals stay free at every distance.
                auto empty = state;
                empty.reserve = 0;
                auto indebted = state;
                indebted.overload_debt =
                    std::numeric_limits<std::uint32_t>::max();
                if (!same_turns(empty, OverdriveCommand{node(target)},
                                std::nullopt) ||
                    !same_turns(indebted, OverdriveCommand{node(target)},
                                std::nullopt))
                    return 2;
            }
    // A free first overdrive at MAX debt still reaches two steps.
    {
        GameState state{.player_position = node(24)};
        state.modules = loadout(
            {ModuleId::first_overdrive_debt, ModuleId::overdrive_range});
        state.overload_debt = std::numeric_limits<std::uint32_t>::max();
        state.rotatable_nodes[10] = true;
        state.ports[10] = *PortMask::from_bits(1);
        auto expected = state;
        expected.ports[10] = *PortMask::from_bits(4);
        expected.turns_taken = TurnIndex{1};
        expected.reserve = 1;
        expected.first_overdrive_used = true;
        if (!same_turns(state, OverdriveCommand{node(10)}, expected) ||
            !same_turns(expected, OverdriveCommand{node(10)}, std::nullopt))
            return 3;
    }
    // The fixed-terminal allowance is used exactly once, also at range 2.
    for (const bool extended : {false, true})
        for (const int target : {23, 17, 22, 10, 26, 38, 8, 3})
        {
            GameState state{.player_position = node(24)};
            state.modules = extended ? loadout({ModuleId::terminal_rotation,
                                                ModuleId::overdrive_range})
                                     : loadout({ModuleId::terminal_rotation});
            state.ports[target] = *PortMask::from_bits(1);
            const int d = distance(24, target);
            std::optional<GameState> expected;
            if (d <= (extended ? 2 : 1))
            {
                expected = state;
                expected->ports[target] = *PortMask::from_bits(4);
                expected->terminal_rotation_used = true;
                expected->turns_taken = TurnIndex{1};
                expected->reserve = 1;
                expected->overload_debt = 1;
                expected->first_overdrive_used = true;
            }
            std::optional<GameState> rotated;
            if (d <= 1)
            {
                rotated = state;
                rotated->ports[target] = *PortMask::from_bits(2);
                rotated->terminal_rotation_used = true;
                rotated->turns_taken = TurnIndex{1};
            }
            if (!same_turns(state, OverdriveCommand{node(target)}, expected) ||
                !same_turns(state, RotateCommand{node(target)}, rotated))
                return 4;
            if (expected &&
                (!same_turns(*expected, OverdriveCommand{node(target)},
                             std::nullopt) ||
                 !same_turns(*expected, RotateCommand{node(target)},
                             std::nullopt)))
                return 5;
            const RunCheckpoint cp{RunSeed{7}, *SectorNumber::from_value(2),
                                   3,          2,
                                   0,          state.modules,
                                   false};
            const auto bytes = encode_checkpoint(cp);
            if (!bytes || decode_checkpoint(*bytes) != cp)
                return 6;
        }
    for (const auto &set : sets)
    {
        const auto rules = rotation_rules(set.modules);
        if (rules.maximum_distance != set.rotate ||
            rules.overdrive_distance != set.overdrive)
            return 7;
    }
    return 0;
}
