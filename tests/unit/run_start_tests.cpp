#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int value)
{
    return *NodeId::from_value(value);
}
struct Vector
{
    RunSeed seed;
    bool top;
    std::array<std::uint8_t, 13> masks;
};
// Independent BigInt SplitMix64 and numerical masks, recorded in P035-02.
const Vector vectors[]{
    {RunSeed{0}, false, {4, 5, 10, 5, 5, 10, 6, 10, 10, 10, 5, 5, 8}},
    {RunSeed{1}, true, {2, 10, 10, 5, 10, 5, 12, 10, 5, 10, 10, 10, 1}},
    {RunSeed{2}, true, {2, 10, 10, 10, 10, 10, 12, 5, 5, 5, 5, 10, 1}},
    {RunSeed{UINT64_MAX}, false, {4, 5, 5, 5, 5, 5, 3, 5, 10, 5, 10, 10, 8}}};
bool cases()
{
    SplitMix64 external{RunSeed{42}}, reference = external;
    for (const auto &v : vectors)
    {
        RunState expected{.seed = v.seed,
                          .sector = GameState{.player_position = node(0)}};
        expected.sector.sector_entry = node(0);
        expected.sector.sector_exit = node(48);
        for (int i = 0; i < 13; ++i)
        {
            const auto n = v.top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                                 : (i <= 6 ? i * 7 : 42 + i - 6);
            expected.sector.ports[n] = *PortMask::from_bits(v.masks[i]);
            expected.sector.rotatable_nodes[n] =
                i != 0 && i != 4 && i != 8 && i != 12;
            if (i == 4 || i == 8)
                expected.sector.receiver_nodes[n] = true;
        }
        // Explicit resource/reset expectations, not copied from start_run.
        expected.sector.player_integrity = 3;
        expected.sector.reserve = 2;
        expected.sector.overload_debt = 0;
        expected.sector.turns_taken = TurnIndex{0};
        expected.sector.modules = {};
        expected.sector.terminal_rotation_used = false;
        expected.reward_claimed = false;
        expected.sector_number = *SectorNumber::from_value(1);
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            auto run = start_run(v.seed);
            const auto before = run;
            if (!test::same_run(run, expected) ||
                run_phase(run) != RunPhase::playing ||
                external.next_u64() != reference.next_u64())
                return false;
            auto copy = run;
            copy.sector.pulses.push_back({*EntityId::from_value(1), node(0)});
            copy.sector.packet = Packet{node(1), node(2)};
            copy.sector.player_integrity = 0;
            copy.sector.reserve = 0;
            copy.sector.overload_debt = 9;
            static_cast<void>(
                copy.sector.modules.add(ModuleId::rotation_range));
            copy.sector.terminal_rotation_used = true;
            copy.reward_claimed = true;
            copy.sector_number = *SectorNumber::from_value(3);
            run.sector.ports[0] = *PortMask::from_bits(15);
            if (!test::same_run(start_run(v.seed), before) ||
                test::same_run(copy, before))
                return false;
        }
    }
    return true;
}

bool boundary_builder()
{
    for (std::uint64_t index = 0; index <= 256; ++index)
    {
        const RunSeed seed{index == 256 ? UINT64_MAX : index};
        const auto topology = generate_sector_topology(seed);
        auto expected = *GameState::initial(
            topology.ports, topology.rotatable_nodes, topology.entry);
        expected.sector_entry = topology.entry;
        expected.sector_exit = topology.exit;
        for (const auto receiver : topology.receivers)
            expected.receiver_nodes[receiver.value()] = true;
        const auto normal = build_sector_state(seed, SectorVariant::normal);
        if (!normal || !test::same_state(*normal, expected) ||
            build_sector_state(seed, static_cast<SectorVariant>(255)))
            return false;
        const bool top = topology.receivers[0] == node(4);
        const auto third = node(top ? 34 : 46);
        expected.ports[third.value()] = *PortMask::from_bits(top ? 5 : 10);
        expected.rotatable_nodes[third.value()] = false;
        expected.receiver_nodes[third.value()] = true;
        expected.delivery_requirement = *DeliveryRequirement::from_value(3);
        auto final = build_sector_state(seed, SectorVariant::final);
        if (!final || !test::same_state(*final, expected) ||
            sector_outcome(*final) != SectorOutcome::ongoing)
            return false;
        final->receiver_nodes.fill(false);
        final->ports[0] = *PortMask::from_bits(0);
        if (!test::same_state(
                *normal, *build_sector_state(seed, SectorVariant::normal)) ||
            !test::same_state(expected,
                              *build_sector_state(seed, SectorVariant::final)))
            return false;
    }
    return true;
}
} // namespace
int main()
{
    if (!boundary_builder() || !cases())
    {
        std::cerr << "Fresh run complete-state vector mismatch\n";
        return 1;
    }
    return 0;
}
