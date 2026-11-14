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
} // namespace
int main()
{
    for (const bool installed : {false, true})
        for (const bool paired : {false, true})
            for (const bool broken : {false, true})
                for (unsigned distance = 0; distance <= 3; ++distance)
                {
                    GameState state{.player_position = node(48)};
                    for (unsigned i = 0; i < 4; ++i)
                        state.ports[i] = *PortMask::from_bits(i == 0   ? 2
                                                              : i == 3 ? 8
                                                                       : 10);
                    if (broken)
                        state.ports[0] = *PortMask::from_bits(0);
                    state.reserve = 0;
                    state.sector_entry = node(0);
                    state.receiver_nodes[distance] = state.receiver_nodes[4] =
                        true;
                    state.packet = Packet{node(0), node(distance)};
                    if (installed &&
                        !state.modules.add(ModuleId::packet_stride))
                        return 1;
                    if (paired &&
                        !state.modules.add(ModuleId::delivery_reserve))
                        return 1;
                    const auto before = state;
                    auto moved = state;
                    const unsigned position =
                        broken ? 0 : std::min(distance, installed ? 2U : 1U);
                    moved.packet->position = node(position);
                    if (advance_packet(state) != (position > 0) ||
                        !test::same_state(state, moved))
                        return 2;
                    auto expected = moved;
                    const bool delivered = position == distance;
                    if (delivered)
                    {
                        expected.packet.reset();
                        expected.powered_nodes[distance] = true;
                        expected.reserve = paired ? 2 : 1;
                    }
                    expected.turns_taken = TurnIndex{1};
                    for (unsigned repeat = 0; repeat < 3; ++repeat)
                    {
                        const auto preview =
                            preview_turn(before, WaitCommand{});
                        const auto result = resolve_turn(before, WaitCommand{});
                        const auto rejected =
                            resolve_turn(before, MoveCommand{node(0)});
                        if (!preview.accepted || !result.accepted ||
                            rejected.accepted ||
                            !test::same_state(preview.state, expected) ||
                            !test::same_state(result.state, expected) ||
                            !test::same_state(rejected.state, before))
                            return 3;
                    }
                    if (delivered)
                    {
                        // The other receiver is unreachable. It is introduced
                        // only next turn.
                        auto next = expected;
                        next.packet = Packet{node(0), node(4)};
                        next.turns_taken = TurnIndex{2};
                        const auto result =
                            resolve_turn(expected, WaitCommand{});
                        if (!result.accepted ||
                            !test::same_state(result.state, next))
                            return 4;
                    }
                    const RunCheckpoint cp{
                        RunSeed{9}, *SectorNumber::from_value(1),
                        3,          0,
                        0,          before.modules,
                        false};
                    const auto bytes = encode_checkpoint(cp);
                    if (!bytes || decode_checkpoint(*bytes) != cp)
                        return 5;
                }
    GameState empty{.player_position = node(0)};
    if (!empty.modules.add(ModuleId::packet_stride))
        return 6;
    const auto before = empty;
    return advance_packet(empty) || !test::same_state(empty, before) ? 7 : 0;
}
