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
} // namespace
int main()
{
    for (const bool installed : {false, true})
        for (const bool paired : {false, true})
            for (const std::uint32_t turns : {0U, 1U, 46U, 47U, UINT32_MAX})
                for (const bool initial_contact : {false, true})
                    for (const bool prior_damage : {false, true})
                    {
                        std::array<unsigned, 3> order{0, 1, 2};
                        do
                        {
                            GameState state{.player_position = node(2)};
                            state.ports[0] = *PortMask::from_bits(2);
                            state.ports[1] = *PortMask::from_bits(10);
                            state.ports[2] = *PortMask::from_bits(8);
                            state.turns_taken = TurnIndex{turns};
                            state.overload_debt = 7;
                            state.powered_nodes[1] = state.receiver_nodes[1] =
                                true;
                            if (installed &&
                                !state.modules.add(ModuleId::pulse_cadence))
                                return 1;
                            if (paired &&
                                !state.modules.add(ModuleId::delivery_reserve))
                                return 1;
                            const std::array<Pulse, 3> units{
                                Pulse{id(9), node(0)}, Pulse{id(2), node(1)},
                                Pulse{id(8), node(2)}};
                            for (const auto index : order)
                                if (index != 2 || initial_contact)
                                    state.pulses.push_back(units[index]);
                            const auto before = state;
                            auto expected = state;
                            const bool moves =
                                !installed ||
                                (std::uint64_t{turns} + 1) % 2 == 0;
                            const bool contact = initial_contact || moves;
                            if (contact && !prior_damage)
                                --expected.player_integrity;
                            std::erase_if(expected.pulses,
                                          [&](const auto &pulse) {
                                              return pulse.id == id(8) ||
                                                     (moves &&
                                                      pulse.id == id(2));
                                          });
                            if (moves)
                                std::ranges::find(expected.pulses, id(9),
                                                  &Pulse::id)
                                    ->position = node(1);
                            bool damage = prior_damage;
                            resolve_pulse_phase(state, damage);
                            if (!test::same_state(state, expected) ||
                                damage != (prior_damage || contact))
                                return 2;
                            if (!prior_damage && turns < 48)
                            {
                                auto turn_expected = expected;
                                turn_expected.turns_taken =
                                    TurnIndex{turns + 1};
                                for (unsigned repeat = 0; repeat < 3; ++repeat)
                                {
                                    const auto preview =
                                        preview_turn(before, WaitCommand{});
                                    const auto result =
                                        resolve_turn(before, WaitCommand{});
                                    const auto rejected = resolve_turn(
                                        before, MoveCommand{node(48)});
                                    if (!preview.accepted || !result.accepted ||
                                        rejected.accepted ||
                                        !test::same_state(preview.state,
                                                          turn_expected) ||
                                        !test::same_state(result.state,
                                                          turn_expected) ||
                                        !test::same_state(rejected.state,
                                                          before))
                                        return 3;
                                }
                                auto terminal = before;
                                terminal.turns_taken = TurnIndex{48};
                                const auto result =
                                    resolve_turn(terminal, WaitCommand{});
                                if (result.accepted ||
                                    !test::same_state(result.state, terminal))
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
    return 0;
}
