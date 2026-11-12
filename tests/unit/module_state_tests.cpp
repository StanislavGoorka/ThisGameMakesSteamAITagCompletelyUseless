#include "../support/state_checks.hpp"
#include "core/turn_preview.hpp"
#include "corpus/baseline_witness.hpp"
#include "replay/state_hash.hpp"
#include <iostream>
using namespace closed_circuit;
using namespace closed_circuit::developer;
int main()
{
    const auto zero = *NodeId::from_value(0);
    const GameState empty{.player_position = zero};
    const std::array<std::uint64_t, 4> hashes{
        15636418876360735861ULL, 15635462301244381516ULL,
        15638332026593444551ULL, 15635464500267637938ULL};
    for (unsigned mask = 0; mask < 4; ++mask)
    {
        auto state = empty;
        if ((mask & 1) && !state.modules.add(ModuleId::rotation_range))
            return 1;
        if ((mask & 2) && !state.modules.add(ModuleId::terminal_rotation))
            return 1;
        const auto before = state;
        if (state_hash(state) != hashes[mask] ||
            !test::same_state(state, before))
        {
            std::cerr << "Independent module hash vector mismatch\n";
            return 1;
        }
        const Command invalid = MoveCommand{*NodeId::from_value(48)};
        const auto preview = preview_turn(state, invalid);
        const auto result = resolve_turn(state, invalid);
        if (preview.accepted || result.accepted ||
            !test::same_state(preview.state, before) ||
            !test::same_state(result.state, before) ||
            !test::same_state(state, before))
            return 1;
        auto expected = before;
        expected.turns_taken = TurnIndex{1};
        const auto waited = resolve_turn(state, WaitCommand{});
        if (!waited.accepted || !test::same_state(waited.state, expected))
            return 1;
        auto copy = state;
        copy.modules = {};
        if (!test::same_state(state, before) || empty.modules.size() != 0)
            return 1;
        auto baseline = sector_fixture(RunSeed{0});
        baseline.modules = state.modules;
        if (static_cast<bool>(build_baseline_witness(RunSeed{0}, baseline)) !=
            (mask == 0))
            return 1;
    }
    auto initial = GameState::initial(empty.ports, empty.rotatable_nodes, zero);
    if (!initial || initial->modules.size() != 0)
        return 1;
    return 0;
}
