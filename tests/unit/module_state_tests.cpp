#include "../support/state_checks.hpp"
#include "application/run_checkpoint.hpp"
#include "core/run_transition.hpp"
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
    // Independent v8 literals from hash_vectors.cjs: only the new bit differs.
    auto used_overdrive = empty;
    used_overdrive.first_overdrive_used = true;
    if (state_hash(empty) != 5173095973100270318ULL ||
        state_hash(used_overdrive) != 5173097072611898529ULL ||
        test::same_state(empty, used_overdrive))
        return 2;
    const auto copy = used_overdrive;
    reset_sector_module_usage(used_overdrive);
    if (!test::same_state(used_overdrive, empty) || !copy.first_overdrive_used)
        return 3;
    auto fixture = sector_fixture(RunSeed{0});
    fixture.first_overdrive_used = true;
    if (build_baseline_witness(RunSeed{0}, fixture))
        return 4;
    for (unsigned number = 1; number <= sector_count; ++number)
    {
        const RunCheckpoint cp{
            RunSeed{42}, *SectorNumber::from_value(number), 3, 2, 0, {}, false};
        auto restored = restore_checkpoint(cp);
        if (!restored || restored->sector.first_overdrive_used ||
            capture_checkpoint(*restored) != cp)
            return 5;
        restored->sector.first_overdrive_used = true;
        const auto before = restored->sector;
        if (capture_checkpoint(*restored) ||
            !test::same_state(restored->sector, before))
            return 6;
        reset_sector_module_usage(restored->sector);
        if (capture_checkpoint(*restored) != cp)
            return 7;
    }
    const std::array<std::uint64_t, 4> hashes{
        5173095973100270318ULL, 15076083063427166573ULL, 6536152380509928592ULL,
        931437974484716655ULL};
    const std::array<std::uint64_t, 4> used_hashes{
        1907211077153216437ULL, 18341967959374220454ULL, 3277920085493709471ULL,
        4189670269500935776ULL};
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
