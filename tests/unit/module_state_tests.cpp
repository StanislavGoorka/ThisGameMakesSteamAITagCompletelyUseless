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
        10316237216983530265ULL, 6805292114135316750ULL, 3294347011287103235ULL,
        3253568260056888444ULL};
    const std::array<std::uint64_t, 4> used_hashes{
        18140584759172842758ULL, 17427688645655555873ULL,
        11118694553476415728ULL, 13875964791577127567ULL};
    for (const bool used : {false, true})
        for (unsigned mask = 0; mask < 4; ++mask)
        {
            auto state = empty;
            state.terminal_rotation_used = used;
            if ((mask & 1) && !state.modules.add(ModuleId::rotation_range))
                return 1;
            if ((mask & 2) && !state.modules.add(ModuleId::terminal_rotation))
                return 1;
            const auto before = state;
            if (state_hash(state) !=
                    (used ? used_hashes[mask] : hashes[mask]) ||
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
            if (static_cast<bool>(build_baseline_witness(
                    RunSeed{0}, baseline)) != (mask == 0))
                return 1;
            baseline.modules = {};
            baseline.terminal_rotation_used = used;
            if (static_cast<bool>(
                    build_baseline_witness(RunSeed{0}, baseline)) == used)
                return 1;
        }
    auto initial = GameState::initial(empty.ports, empty.rotatable_nodes, zero);
    if (!initial || initial->modules.size() != 0 ||
        initial->terminal_rotation_used)
        return 1;
    return 0;
}
