#include "../support/state_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/turn_preview.hpp"

using namespace closed_circuit;
namespace
{
NodeId node(unsigned n)
{
    return *NodeId::from_value(n);
}
EntityId id(unsigned n)
{
    return *EntityId::from_value(n);
}
GameState square(unsigned player)
{
    GameState state{.player_position = node(player)};
    // Equal routes 0-1-8 and 0-7-8; E wins the BFS tie.
    state.ports[0] = *PortMask::from_bits(6);
    state.ports[1] = *PortMask::from_bits(12);
    state.ports[7] = *PortMask::from_bits(3);
    state.ports[8] = *PortMask::from_bits(9);
    return state;
}
} // namespace
int main()
{
    for (const bool installed : {false, true})
        for (const bool cadence : {false, true})
            for (const bool powered : {false, true})
                for (const bool receiver : {false, true})
                    for (const unsigned turns : {0U, 1U})
                    {
                        std::array<unsigned, 3> order{0, 1, 2};
                        do
                        {
                            auto state = square(8);
                            state.powered_nodes[1] = powered;
                            state.receiver_nodes[1] = receiver;
                            state.turns_taken = TurnIndex{turns};
                            if (installed &&
                                !state.modules.add(
                                    ModuleId::powered_pulse_barrier))
                                return 1;
                            if (cadence &&
                                !state.modules.add(ModuleId::pulse_cadence))
                                return 1;
                            const std::array<Pulse, 3> units{
                                Pulse{id(9), node(0)}, Pulse{id(2), node(1)},
                                Pulse{id(20), node(8)}};
                            for (const auto index : order)
                                state.pulses.push_back(units[index]);
                            const auto before = state;
                            const bool blocked =
                                installed && powered && receiver;
                            auto isolated = before;
                            isolated.pulses = {{id(9), node(0)}};
                            const auto isolated_before = isolated;
                            const auto step = next_pulse_step(isolated, id(9));
                            if (step != (blocked ? std::nullopt
                                                 : std::optional{node(1)}) ||
                                !test::same_state(isolated, isolated_before))
                                return 2;
                            auto expected = before;
                            --expected.player_integrity;
                            const bool moves = !cadence || turns == 1;
                            std::erase_if(expected.pulses,
                                          [&](const auto &pulse) {
                                              return pulse.id == id(20) ||
                                                     (moves &&
                                                      pulse.id == id(2));
                                          });
                            if (moves && !blocked)
                                std::ranges::find(expected.pulses, id(9),
                                                  &Pulse::id)
                                    ->position = node(1);
                            bool damage = false;
                            resolve_pulse_phase(state, damage);
                            if (!damage || !test::same_state(state, expected))
                                return 3;
                            expected.turns_taken = TurnIndex{turns + 1};
                            for (unsigned repeat = 0; repeat < 3; ++repeat)
                            {
                                const auto preview =
                                    preview_turn(before, WaitCommand{});
                                const auto result =
                                    resolve_turn(before, WaitCommand{});
                                const auto rejected =
                                    resolve_turn(before, MoveCommand{node(48)});
                                if (!preview.accepted || !result.accepted ||
                                    rejected.accepted ||
                                    !test::same_state(preview.state,
                                                      expected) ||
                                    !test::same_state(result.state, expected) ||
                                    !test::same_state(rejected.state, before))
                                    return 4;
                            }
                            const RunCheckpoint cp{
                                RunSeed{0}, *SectorNumber::from_value(1),
                                3,          2,
                                0,          before.modules,
                                false};
                            const auto bytes = encode_checkpoint(cp);
                            if (!bytes || decode_checkpoint(*bytes) != cp)
                                return 5;
                        } while (
                            std::next_permutation(order.begin(), order.end()));
                    }
    // The same powered receiver remains passable for the player and packet.
    for (const bool installed : {false, true})
        for (const bool stride : {false, true})
        {
            auto state = square(0);
            state.receiver_nodes[1] = state.powered_nodes[1] =
                state.receiver_nodes[8] = true;
            state.packet = Packet{node(0), node(8)};
            state.pulses = {{id(9), node(0)}};
            if (installed &&
                !state.modules.add(ModuleId::powered_pulse_barrier))
                return 6;
            if (stride && !state.modules.add(ModuleId::packet_stride))
                return 6;
            auto expected = state;
            expected.player_position = node(1);
            expected.turns_taken = TurnIndex{1};
            if (stride)
            {
                expected.packet.reset();
                expected.powered_nodes[8] = true;
                expected.reserve = 3;
            }
            else
                expected.packet->position = node(1);
            if (!installed)
            {
                expected.pulses.clear();
                --expected.player_integrity;
            }
            const auto result = resolve_turn(state, MoveCommand{node(1)});
            const auto preview = preview_turn(state, MoveCommand{node(1)});
            if (!result.accepted || !preview.accepted ||
                !test::same_state(result.state, expected) ||
                !test::same_state(preview.state, expected))
                return 7;
        }
    return 0;
}
