#include "../support/state_checks.hpp"
#include "corpus/baseline_witness.hpp"
#include "replay/state_hash.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
namespace
{
DelayedCapacitor cap(unsigned id, unsigned pos, unsigned delay, unsigned range)
{
    return *DelayedCapacitor::from_values(
        *EntityId::from_value(id), *NodeId::from_value(pos), delay, range);
}
bool different(const GameState &a, const GameState &b)
{
    return !test::same_state(a, b) && state_hash(a) != state_hash(b);
}
bool cases()
{
    GameState state{.player_position = *NodeId::from_value(0)};
    const auto initial = GameState::initial(state.ports, state.rotatable_nodes,
                                            state.player_position);
    if (!initial || !initial->capacitors.empty() || !state.capacitors.empty())
        return false;
    state.capacitors = {cap(0, 0, 1, 1), cap(UINT32_MAX, 48, 48, 48)};
    const auto before = state;
    auto copy = state;
    auto second = copy;
    if (!test::same_state(copy, state) || !test::same_state(second, state) ||
        state_hash(state) != 12803702081908847061ULL)
        return false;
    auto changed_delay = cap(0, 0, 2, 1);
    static_cast<void>(changed_delay.advance_fuse());
    auto spent = cap(0, 0, 1, 1);
    static_cast<void>(spent.advance_fuse());
    // Each substitution changes exactly one encoded field, including delay
    // while remaining stays 1, and remaining while delay stays 1.
    for (const auto value : {cap(1, 0, 1, 1), cap(0, 1, 1, 1), changed_delay,
                             cap(0, 0, 1, 2), spent})
    {
        copy = state;
        copy.capacitors[0] = value;
        if (!different(copy, state))
            return false;
    }
    for (int mode = 0; mode < 3; ++mode)
    {
        second = state;
        if (mode == 0)
            second.capacitors.clear();
        if (mode == 1)
            second.capacitors.pop_back();
        if (mode == 2)
            std::swap(second.capacitors[0], second.capacitors[1]);
        if (!different(second, state))
            return false;
    }
    copy = state;
    for (auto &c : copy.capacitors)
        static_cast<void>(c.advance_fuse());
    if (state_hash(copy) != 7210142062484689355ULL || !different(copy, state))
        return false;
    for (unsigned i = 0; i < 47; ++i)
        for (auto &c : copy.capacitors)
            static_cast<void>(c.advance_fuse());
    if (state_hash(copy) != 1314440864372147646ULL || !different(copy, state))
        return false;
    const auto spent_state = copy;
    for (auto &c : copy.capacitors)
        if (c.advance_fuse())
            return false;
    if (!test::same_state(copy, spent_state))
        return false;
    auto baseline = sector_fixture(RunSeed{0});
    if (!build_baseline_witness(RunSeed{0}, baseline))
        return false;
    for (const auto &list : {state.capacitors, copy.capacitors})
    {
        baseline.capacitors = list;
        if (build_baseline_witness(RunSeed{0}, baseline))
            return false;
    }
    return test::same_state(state, before);
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Capacitor state/hash mismatch\n";
        return 1;
    }
    return 0;
}
