#include "../support/state_checks.hpp"
#include "core/emitter_phase.hpp"
#include "core/pulse_contact.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(unsigned value)
{
    return *NodeId::from_value(value);
}
PeriodicEmitter emitter(unsigned id, unsigned position, unsigned period,
                        unsigned phase, unsigned range)
{
    return *PeriodicEmitter::from_values(*EntityId::from_value(id),
                                         node(position), period, phase, range);
}
GameState fixture()
{
    std::array<PortMask, 49> ports{};
    std::array<bool, 49> rotatable{};
    ports[0] = *PortMask::from_bits(2);
    ports[1] = *PortMask::from_bits(10);
    ports[2] = *PortMask::from_bits(8);
    rotatable[1] = true;
    auto state = *GameState::initial(ports, rotatable, node(2));
    state.turns_taken = TurnIndex{2};
    state.blocked_nodes[2] = true;
    state.packet = Packet{node(1), node(2)};
    state.receiver_nodes[2] = true;
    state.powered_nodes[7] = true;
    state.sector_entry = node(0);
    state.sector_exit = node(48);
    state.pulses.push_back({*EntityId::from_value(42), node(20)});
    state.reserve = 1;
    state.overload_debt = 8;
    static_cast<void>(state.modules.add(ModuleId::rotation_range));
    state.terminal_rotation_used = true;
    return state;
}
bool check(const GameState &before, std::span<const PeriodicEmitter> emitters,
           bool used, bool hit)
{
    const std::vector<PeriodicEmitter> configurations(emitters.begin(),
                                                      emitters.end());
    const auto original = before;
    auto expected = before, actual = before;
    bool expected_used = used, actual_used = used;
    if (hit && !used && before.player_alive())
    {
        --expected.player_integrity;
        expected_used = true;
    }
    resolve_emitter_phase(actual, emitters, actual_used);
    return test::same_state(actual, expected) && actual_used == expected_used &&
           std::ranges::equal(emitters, configurations) &&
           test::same_state(before, original);
}
bool cases()
{
    const std::array single{emitter(1, 0, 3, 0, 2)};
    const std::array<bool, 8> active{false, false, true,  false,
                                     false, true,  false, false};
    for (unsigned position = 0; position < 49; ++position)
        for (unsigned turn = 0; turn < active.size(); ++turn)
            for (unsigned hp = 0; hp <= 3; ++hp)
                for (const bool used : {false, true})
                {
                    auto state = fixture();
                    state.player_position = node(position);
                    state.turns_taken = TurnIndex{turn};
                    state.player_integrity = static_cast<std::uint8_t>(hp);
                    if (!check(state, single, used,
                               active[turn] && position <= 2))
                        return false;
                }
    auto state = fixture();
    if (!check(state, {}, false, false))
        return false;
    const std::array short_range{emitter(1, 0, 3, 0, 1)};
    if (!check(state, short_range, false, false))
        return false;
    state.ports[1] = state.ports[1].rotate_clockwise();
    if (!check(state, single, false, false))
        return false;
    state = fixture();
    std::array multiple{emitter(3, 0, 3, 0, 2), emitter(2, 2, 2, 1, 1),
                        emitter(1, 48, 3, 0, 48)};
    for (int order = 0; order < 6; ++order)
    {
        if (!check(state, multiple, false, true) ||
            !check(state, multiple, true, true))
            return false;
        std::next_permutation(multiple.begin(), multiple.end(),
                              [](const auto &a, const auto &b)
                              { return a.id() < b.id(); });
    }
    // The same shared guard prevents double damage in either contact order.
    for (const bool pulse_first : {false, true})
    {
        auto actual = fixture();
        const auto contact = *EntityId::from_value(99);
        actual.pulses.push_back({contact, actual.player_position});
        auto expected = fixture();
        --expected.player_integrity;
        bool used = false;
        if (pulse_first && !resolve_pulse_contact(actual, contact, used))
            return false;
        resolve_emitter_phase(actual, multiple, used);
        if (!pulse_first && !resolve_pulse_contact(actual, contact, used))
            return false;
        if (!used || !test::same_state(actual, expected))
            return false;
    }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Emitter phase or shared damage limit mismatch\n";
        return 1;
    }
    return 0;
}
