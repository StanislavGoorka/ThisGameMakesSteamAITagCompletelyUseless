#include "../support/state_checks.hpp"
#include "core/debt_spawn.hpp"
#include "corpus/sector_fixture.hpp"
#include "corpus/spawn_check.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
namespace
{
bool expect(const GameState &state, SpawnIssue issue)
{
    const auto before = state;
    for (int i = 0; i < 3; ++i)
        if (initial_spawn_issue(state) != issue ||
            !test::same_state(state, before))
            return false;
    return true;
}
} // namespace
int main()
{
    for (const auto seed : {0ULL, 1ULL, 2ULL, UINT64_MAX})
    {
        const auto base = sector_fixture(RunSeed{seed});
        if (!expect(base, SpawnIssue::none))
            return 1;
        for (const std::uint32_t debt : {0U, 2U, 3U, 6U})
        {
            auto state = base;
            state.overload_debt = debt;
            auto expected = state;
            if (debt >= 3)
            {
                expected.overload_debt -= 3;
                expected.pulses.push_back(
                    {*EntityId::from_value(0), *base.sector_entry});
            }
            if (spawn_debt_pulse(state) != (debt >= 3) ||
                !test::same_state(state, expected) ||
                !expect(state, SpawnIssue::none))
                return 1;
            if (spawn_debt_pulse(state) || !test::same_state(state, expected))
                return 1;
        }
        for (int mode = 0; mode < 3; ++mode)
        {
            auto state = base;
            state.overload_debt = 6;
            if (mode == 0)
                state.sector_entry.reset();
            if (mode == 1)
                state.blocked_nodes[base.sector_entry->value()] = true;
            if (mode == 2)
                state.pulses.push_back(
                    {*EntityId::from_value(9), *base.sector_entry});
            const auto before = state;
            if (spawn_debt_pulse(state) || !test::same_state(state, before))
                return 1;
        }
        auto shared = base;
        const auto receiver =
            *NodeId::from_value(base.receiver_nodes[4] ? 4 : 28);
        shared.packet = Packet{*base.sector_entry, receiver};
        shared.pulses.push_back({*EntityId::from_value(5), *base.sector_entry});
        if (!expect(shared, SpawnIssue::none))
            return 1;
        auto bad = shared;
        bad.pulses.push_back({*EntityId::from_value(5), receiver});
        if (!expect(bad, SpawnIssue::duplicate_identity))
            return 1;
        bad = shared;
        bad.pulses.push_back({*EntityId::from_value(6), *base.sector_entry});
        if (!expect(bad, SpawnIssue::duplicate_position))
        {
            std::cerr << "Duplicate position not rejected\n";
            return 1;
        }
        bad = shared;
        bad.pulses[0].position = *NodeId::from_value(24);
        if (!expect(bad, SpawnIssue::inactive_node))
            return 1;
        bad = shared;
        bad.blocked_nodes[receiver.value()] = true;
        if (!expect(bad, SpawnIssue::blocked_spawn))
            return 1;
        bad = shared;
        bad.sector_exit.reset();
        if (!expect(bad, SpawnIssue::missing_endpoint))
            return 1;
        bad = shared;
        bad.receiver_nodes[receiver.value()] = false;
        if (!expect(bad, SpawnIssue::receiver_layout))
            return 1;
    }
    return 0;
}
