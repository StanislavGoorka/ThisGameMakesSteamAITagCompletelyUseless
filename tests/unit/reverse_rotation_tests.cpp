#include "../support/state_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/turn_preview.hpp"

using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
// Independent bit formulas with N=1, E=2, S=4, W=8.
unsigned clockwise(unsigned m)
{
    return ((m << 1) & 15U) | (m >> 3);
}
unsigned counter_clockwise(unsigned m)
{
    return (m >> 1) | ((m & 1U) << 3);
}
unsigned half(unsigned m)
{
    return ((m << 2) | (m >> 2)) & 15U;
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
ModuleLoadout loadout(bool reverse, std::optional<ModuleId> partner)
{
    ModuleLoadout modules;
    if (reverse && !modules.add(ModuleId::reverse_rotation))
        throw 0;
    if (partner && !modules.add(*partner))
        throw 0;
    return modules;
}
} // namespace
int main()
{
    constexpr std::array partners{std::optional<ModuleId>{},
                                  std::optional{ModuleId::rotation_range},
                                  std::optional{ModuleId::terminal_rotation},
                                  std::optional{ModuleId::overdrive_range}};
    for (const bool reverse : {false, true})
        for (const auto partner : partners)
            for (unsigned mask = 0; mask < 16; ++mask)
                for (const int target : {24, 25, 17, 26, 10, 27})
                {
                    // Player at 24; targets at distance 0, 1, 1, 2, 2 and 3.
                    GameState state{.player_position = node(24)};
                    state.modules = loadout(reverse, partner);
                    state.rotatable_nodes[target] = true;
                    state.ports[target] = *PortMask::from_bits(mask);
                    const int d = target == 24                     ? 0
                                  : target == 27                   ? 3
                                  : (target == 26 || target == 10) ? 2
                                                                   : 1;
                    const int rotate_range =
                        partner == ModuleId::rotation_range ? 2 : 1;
                    const int overdrive_range =
                        rotate_range +
                        (partner == ModuleId::overdrive_range ? 1 : 0);
                    std::optional<GameState> rotated, overdriven;
                    if (d <= rotate_range)
                    {
                        rotated = state;
                        rotated->ports[target] = *PortMask::from_bits(
                            reverse ? counter_clockwise(mask)
                                    : clockwise(mask));
                        rotated->turns_taken = TurnIndex{1};
                    }
                    if (d <= overdrive_range)
                    {
                        overdriven = state;
                        overdriven->ports[target] =
                            *PortMask::from_bits(half(mask));
                        overdriven->turns_taken = TurnIndex{1};
                        overdriven->reserve = 1;
                        overdriven->overload_debt = 1;
                        overdriven->first_overdrive_used = true;
                    }
                    if (!same_turns(state, RotateCommand{node(target)},
                                    rotated) ||
                        !same_turns(state, OverdriveCommand{node(target)},
                                    overdriven))
                        return 1;
                    if (!rotated)
                        continue;
                    // Four ordinary rotations restore the mask; each charges
                    // exactly one turn and no resource.
                    auto cycled = state;
                    for (unsigned turn = 1; turn <= 4; ++turn)
                    {
                        const auto next =
                            resolve_turn(cycled, RotateCommand{node(target)});
                        cycled = next.state;
                        if (!next.accepted ||
                            cycled.turns_taken != TurnIndex{turn} ||
                            cycled.reserve != 2 || cycled.overload_debt != 0 ||
                            (turn < 4 && cycled.ports[target].bits() == mask &&
                             mask != 0 && mask != 5 && mask != 10 &&
                             mask != 15))
                            return 2;
                    }
                    if (cycled.ports[target].bits() != mask)
                        return 3;
                }
    // A fixed terminal turns counter-clockwise once; the allowance is shared
    // with overdrive and is used exactly once.
    for (const bool reverse : {false, true})
        for (const unsigned mask : {1U, 2U, 4U, 8U})
            for (const bool overdrive : {false, true})
            {
                GameState state{.player_position = node(24)};
                state.modules = loadout(reverse, ModuleId::terminal_rotation);
                state.ports[25] = *PortMask::from_bits(mask);
                auto expected = state;
                expected.terminal_rotation_used = true;
                expected.turns_taken = TurnIndex{1};
                if (overdrive)
                {
                    expected.ports[25] = *PortMask::from_bits(half(mask));
                    expected.reserve = 1;
                    expected.overload_debt = 1;
                    expected.first_overdrive_used = true;
                }
                else
                    expected.ports[25] = *PortMask::from_bits(
                        reverse ? counter_clockwise(mask) : clockwise(mask));
                const Command command =
                    overdrive ? Command{OverdriveCommand{node(25)}}
                              : Command{RotateCommand{node(25)}};
                if (!same_turns(state, command, expected) ||
                    !same_turns(expected, RotateCommand{node(25)},
                                std::nullopt) ||
                    !same_turns(expected, OverdriveCommand{node(25)},
                                std::nullopt))
                    return 4;
                // Multi-port fixed nodes and missing ID2 stay refused.
                auto multi = state;
                multi.ports[25] = *PortMask::from_bits(mask | 5U);
                auto without = state;
                without.modules = loadout(reverse, std::nullopt);
                if (!same_turns(multi, command, std::nullopt) ||
                    !same_turns(without, command, std::nullopt))
                    return 5;
                // Rotating an ordinary node afterwards never renews it.
                auto spent = expected;
                spent.rotatable_nodes[23] = true;
                spent.ports[23] = *PortMask::from_bits(3);
                auto renewed = spent;
                renewed.ports[23] =
                    *PortMask::from_bits(reverse ? counter_clockwise(3) : 6);
                renewed.turns_taken = TurnIndex{2};
                if (!same_turns(spent, RotateCommand{node(23)}, renewed) ||
                    !same_turns(renewed, RotateCommand{node(25)}, std::nullopt))
                    return 5;
                const RunCheckpoint cp{
                    RunSeed{11}, *SectorNumber::from_value(3),
                    2,           1,
                    1,           state.modules,
                    false};
                const auto bytes = encode_checkpoint(cp);
                if (!bytes || decode_checkpoint(*bytes) != cp)
                    return 6;
            }
    // The direction is observable through the shared route phase: only the
    // counter-clockwise turn opens 0-1-8 before the packet moves.
    for (const bool reverse : {false, true})
    {
        GameState state{.player_position = node(2)};
        state.modules = loadout(reverse, std::nullopt);
        state.ports[0] = *PortMask::from_bits(2);
        state.ports[1] = *PortMask::from_bits(9);
        state.ports[8] = *PortMask::from_bits(1);
        state.rotatable_nodes[1] = true;
        state.receiver_nodes[8] = true;
        state.packet = Packet{node(0), node(8)};
        auto expected = state;
        expected.turns_taken = TurnIndex{1};
        expected.ports[1] = *PortMask::from_bits(reverse ? 12 : 3);
        if (reverse)
            expected.packet->position = node(1);
        if (!same_turns(state, RotateCommand{node(1)}, expected))
            return 7;
    }
    for (const bool reverse : {false, true})
        if (rotation_rules(loadout(reverse, std::nullopt)).quarter_turns !=
            (reverse ? 3 : 1))
            return 8;
    return 0;
}
