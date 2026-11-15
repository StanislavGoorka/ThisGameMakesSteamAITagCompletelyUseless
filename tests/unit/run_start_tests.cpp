#include "../support/run_checks.hpp"
#include "core/run_reward.hpp"
#include "core/run_start.hpp"
#include "core/run_turn.hpp"
#include <cstdlib>
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
            copy.sector_number = *SectorNumber::from_value(sector_count);
            run.sector.ports[0] = *PortMask::from_bits(15);
            auto other_config = before;
            other_config.start_config = static_cast<StartConfigId>(2);
            if (!test::same_run(start_run(v.seed), before) ||
                test::same_run(copy, before) ||
                test::same_run(other_config, before))
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
bool start_configs()
{
    // ID1 and ID2 are implemented; 3 stays reserved until its own step.
    const auto known = [](unsigned raw) { return raw == 1 || raw == 2; };
    for (unsigned raw = 0; raw <= UINT8_MAX; ++raw)
    {
        const auto parsed = start_config_from_value(raw);
        if (parsed.has_value() != known(raw) ||
            (parsed && static_cast<unsigned>(*parsed) != raw) ||
            is_known_start_config(static_cast<StartConfigId>(raw)) !=
                known(raw) ||
            start_run(RunSeed{raw}, static_cast<StartConfigId>(raw))
                    .has_value() != known(raw))
            return false;
    }
    if (start_config_from_value(-1) || start_config_from_value(256) ||
        start_config_from_value(INT64_MIN) ||
        start_config_from_value(UINT64_MAX) || start_config_catalog.size() != 2)
        return false;
    for (std::uint64_t index = 0; index <= 256; ++index)
    {
        const RunSeed seed{index == 256 ? UINT64_MAX : index};
        const auto chosen = start_run(seed, StartConfigId::baseline);
        const auto implicit = start_run(seed);
        if (!chosen || !test::same_run(*chosen, implicit) ||
            chosen->start_config != StartConfigId::baseline ||
            implicit.start_config != StartConfigId::baseline ||
            chosen->sector.player_integrity != 3 ||
            chosen->sector.reserve != 2 || chosen->sector.overload_debt != 0 ||
            chosen->sector.modules.size() != 0)
            return false;
        // ID2 differs from ID1 only by reserve0 and module ID1 in a slot.
        const auto range = start_run(seed, StartConfigId::preinstalled_range);
        auto expected = implicit;
        expected.start_config = StartConfigId::preinstalled_range;
        expected.sector.reserve = 0;
        expected.sector.modules = {};
        static_cast<void>(
            expected.sector.modules.add(ModuleId::rotation_range));
        if (!range || !test::same_run(*range, expected) ||
            range->sector.modules.size() != 1 ||
            !range->sector.modules.contains(ModuleId::rotation_range) ||
            test::same_run(*range, implicit))
            return false;
        // Range2 applies from the first turn; ID1 still refuses that target.
        const int entry = implicit.sector.player_position.value();
        std::optional<NodeId> far;
        for (int n = 0; n < 49 && !far; ++n)
            if (implicit.sector.rotatable_nodes[n] &&
                std::abs(n / 7 - entry / 7) + std::abs(n % 7 - entry % 7) == 2)
                far = node(n);
        if (!far || !resolve_run_turn(*range, RotateCommand{*far}).accepted ||
            resolve_run_turn(implicit, RotateCommand{*far}).accepted)
            return false;
        // One slot is taken: the first offer never repeats ID1 and one reward
        // fills the loadout, so later sectors offer nothing.
        auto done = *range;
        done.sector.player_position = *done.sector.sector_exit;
        done.sector.powered_nodes = done.sector.receiver_nodes;
        const auto offer = module_offer(done.sector.modules, seed, 1);
        if (run_phase(done) != RunPhase::choosing_reward || !offer[0] ||
            offer[0] == ModuleId::rotation_range ||
            offer[1] == ModuleId::rotation_range ||
            !choose_run_module(done, *offer[0]) ||
            done.sector.modules.size() != ModuleLoadout::slot_limit ||
            run_phase(done) != RunPhase::ready_for_next ||
            module_offer(done.sector.modules, seed, 2)[0])
            return false;
    }
    return true;
}
} // namespace
int main()
{
    if (!boundary_builder() || !cases() || !start_configs())
    {
        std::cerr << "Fresh run complete-state vector mismatch\n";
        return 1;
    }
    return 0;
}
