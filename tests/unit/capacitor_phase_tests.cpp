#include "../support/state_checks.hpp"
#include "core/capacitor_phase.hpp"
#include "core/pulse_phase.hpp"
#include <iostream>
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
DelayedCapacitor cap(unsigned n, unsigned source, unsigned delay,
                     unsigned range)
{
    return *DelayedCapacitor::from_values(id(n), node(source), delay, range);
}
GameState fixture(unsigned player = 0, unsigned hp = 3)
{
    GameState s{.player_position = node(player)};
    s.player_integrity = static_cast<std::uint8_t>(hp);
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(10);
    s.ports[2] = *PortMask::from_bits(8);
    s.rotatable_nodes.fill(true);
    s.blocked_nodes.fill(true);
    s.powered_nodes[7] = true;
    s.receiver_nodes[8] = true;
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.packet = Packet{node(1), node(2)};
    s.turns_taken = TurnIndex{19};
    s.reserve = 1;
    s.overload_debt = 7;
    s.emitters.push_back(
        *PeriodicEmitter::from_values(id(7), node(0), 2, 0, 48));
    return s;
}
bool same_configuration(const DelayedCapacitor &a, const DelayedCapacitor &b)
{
    return a.id() == b.id() && a.position() == b.position() &&
           a.delay() == b.delay() && a.range() == b.range();
}
bool matrix()
{
    for (unsigned player = 0; player < 49; ++player)
        for (unsigned hp = 0; hp <= 3; ++hp)
            for (bool limited : {false, true})
                for (unsigned delay : {1u, 2u, 48u})
                    for (unsigned progress = 0; progress <= delay; ++progress)
                        for (unsigned range : {1u, 2u})
                        {
                            auto s = fixture(player, hp);
                            auto expected = s;
                            std::vector<DelayedCapacitor> values{
                                cap(0, 0, delay, range)};
                            for (unsigned i = 0; i < progress; ++i)
                                static_cast<void>(values[0].advance_fuse());
                            const auto before = values;
                            const bool hit = progress == delay - 1 &&
                                             player <= range && hp > 0 &&
                                             !limited;
                            if (hit)
                                --expected.player_integrity;
                            bool damage = limited;
                            resolve_capacitor_phase(s, values, damage);
                            const unsigned remaining =
                                progress < delay ? delay - progress - 1 : 0;
                            if (!test::same_state(s, expected) ||
                                damage != (limited || hit) ||
                                values[0].remaining() != remaining ||
                                !same_configuration(values[0], before[0]))
                                return false;
                        }
    auto s = fixture();
    const auto before = s;
    bool damage = false;
    resolve_capacitor_phase(s, {}, damage);
    return !damage && test::same_state(s, before);
}
bool orders_and_shared_limit()
{
    std::array<unsigned, 3> order{0, 1, 2};
    const std::array values{cap(10, 2, 1, 2), cap(20, 0, 1, 1),
                            cap(30, 1, 2, 1)};
    do
    {
        auto s = fixture();
        auto expected = s;
        std::vector<DelayedCapacitor> list;
        for (auto index : order)
            list.push_back(values[index]);
        for (unsigned phase = 1; phase <= 5; ++phase)
        {
            bool damage = false;
            resolve_capacitor_phase(s, list, damage);
            expected.player_integrity = phase == 1 ? 2 : 1;
            if (!test::same_state(s, expected) || damage != (phase <= 2))
                return false;
            for (const auto &v : list)
                if (v.remaining() != (phase == 1 && v.id() == id(30) ? 1 : 0))
                    return false;
        }
    } while (std::next_permutation(order.begin(), order.end()));
    for (bool pulse_first : {false, true})
    {
        auto s = fixture();
        s.pulses = {{id(4), node(0)}};
        auto expected = s;
        expected.pulses.clear();
        expected.player_integrity = 2;
        std::array list{cap(0, 0, 1, 1)};
        bool damage = false;
        if (pulse_first)
            resolve_pulse_phase(s, damage);
        resolve_capacitor_phase(s, list, damage);
        if (!pulse_first)
            resolve_pulse_phase(s, damage);
        if (!damage || list[0].remaining() != 0 ||
            !test::same_state(s, expected))
            return false;
    }
    return true;
}
bool rotation_between_phases()
{
    auto s = fixture(2);
    auto expected = s;
    std::array list{cap(0, 0, 2, 2)};
    bool damage = false;
    resolve_capacitor_phase(s, list, damage);
    if (damage || list[0].remaining() != 1 || !test::same_state(s, expected))
        return false;
    s.ports[1] = s.ports[1].rotate_clockwise();
    expected = s;
    resolve_capacitor_phase(s, list, damage);
    if (damage || list[0].remaining() != 0 || !test::same_state(s, expected))
        return false;
    s.ports[1] = *PortMask::from_bits(10);
    expected = s;
    resolve_capacitor_phase(s, list, damage);
    return !damage && test::same_state(s, expected);
}
} // namespace
int main()
{
    if (!matrix() || !orders_and_shared_limit() || !rotation_between_phases())
    {
        std::cerr << "Capacitor phase mismatch\n";
        return 1;
    }
    return 0;
}
