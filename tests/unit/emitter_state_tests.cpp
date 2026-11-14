#include "../support/state_checks.hpp"
#include "corpus/baseline_witness.hpp"
#include "replay/state_hash.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
namespace
{
PeriodicEmitter emitter(unsigned id, unsigned pos, unsigned period,
                        unsigned phase, unsigned range)
{
    return *PeriodicEmitter::from_values(*EntityId::from_value(id),
                                         *NodeId::from_value(pos), period,
                                         phase, range);
}
bool cases()
{
    GameState state{.player_position = *NodeId::from_value(0)};
    if (!state.emitters.empty())
        return false;
    state.emitters = {emitter(0, 0, 2, 0, 1),
                      emitter(UINT32_MAX, 48, 48, 47, 48)};
    const auto before = state;
    auto copy = state;
    auto second = copy;
    if (!test::same_state(copy, state) ||
        state_hash(state) != 7919473700277768818ULL)
        return false;
    for (const auto changed : {emitter(1, 0, 2, 0, 1), emitter(0, 1, 2, 0, 1),
                               emitter(0, 0, 3, 0, 1), emitter(0, 0, 2, 1, 1),
                               emitter(0, 0, 2, 0, 2)})
    {
        copy = state;
        copy.emitters[0] = changed;
        if (test::same_state(copy, state) ||
            state_hash(copy) == state_hash(state))
            return false;
    }
    for (int mode = 0; mode < 3; ++mode)
    {
        second = state;
        if (mode == 0)
            second.emitters.clear();
        if (mode == 1)
            second.emitters.pop_back();
        if (mode == 2)
            std::swap(second.emitters[0], second.emitters[1]);
        if (test::same_state(second, state) ||
            state_hash(second) == state_hash(state))
            return false;
    }
    auto baseline = sector_fixture(RunSeed{0});
    if (!build_baseline_witness(RunSeed{0}, baseline))
        return false;
    baseline.emitters = state.emitters;
    return !build_baseline_witness(RunSeed{0}, baseline) &&
           test::same_state(state, before);
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Emitter value state/hash mismatch\n";
        return 1;
    }
    return 0;
}
