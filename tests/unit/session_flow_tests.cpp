#include "../support/run_checks.hpp"
#include "application/run_checkpoint.hpp"
#include "application/session_flow.hpp"
#include "core/run_turn.hpp"
#include <bit>
#include <limits>
using namespace closed_circuit;

namespace
{
// Selection model: canonical cycle, screen gating and confirm.
int selection(RunSeed seed)
{
    const auto fresh = start_run(seed);
    auto run = fresh;
    SessionFlow flow;
    if (flow.selected(run) != StartConfigId::baseline)
        return 20;
    const StartConfigId order[]{StartConfigId::preinstalled_range,
                                StartConfigId::charged_debt,
                                StartConfigId::baseline};
    for (const auto expected : order)
        if (!flow.select_next(run) || flow.selected(run) != expected ||
            !test::same_run(run, fresh))
            return 21;
    const StartConfigId reverse[]{StartConfigId::charged_debt,
                                  StartConfigId::preinstalled_range,
                                  StartConfigId::baseline};
    for (const auto expected : reverse)
        if (!flow.select_previous(run) || flow.selected(run) != expected)
            return 22;
    // Start the selected ID3; the choice is consumed by confirm.
    if (!flow.select_previous(run) ||
        flow.selected(run) != StartConfigId::charged_debt ||
        !flow.confirm(run) ||
        !test::same_run(run, *start_run(seed, StartConfigId::charged_debt)) ||
        flow.selected(run) != StartConfigId::charged_debt)
        return 23;
    // The consumed choice never overrides another run's configuration.
    auto other = fresh;
    other.sector.player_integrity = 0;
    SessionFlow consumed{true};
    if (!consumed.select_next(other) || !consumed.confirm(other) ||
        other.start_config != StartConfigId::preinstalled_range)
        return 29;
    auto replaced = fresh;
    replaced.sector.player_integrity = 0;
    if (consumed.selected(replaced) != StartConfigId::baseline ||
        !consumed.confirm(replaced) || !test::same_run(replaced, fresh))
        return 30;
    // During a run and while a reward waits, selection is refused.
    const auto playing = run;
    if (flow.select_next(run) || flow.select_previous(run) ||
        flow.selected(run) != StartConfigId::charged_debt ||
        !test::same_run(run, playing))
        return 24;
    auto reward = run;
    reward.sector.player_position = *reward.sector.sector_exit;
    reward.sector.powered_nodes = reward.sector.receiver_nodes;
    if (run_phase(reward) != RunPhase::choosing_reward ||
        flow.select_next(reward) ||
        flow.selected(reward) != StartConfigId::charged_debt)
        return 25;
    // A plain restart after the result keeps ID3; a new choice switches.
    run.sector.player_integrity = 0;
    if (!flow.confirm(run) || run.start_config != StartConfigId::charged_debt)
        return 26;
    run.sector.player_integrity = 0;
    if (!flow.select_next(run) ||
        flow.selected(run) != StartConfigId::baseline || !flow.confirm(run) ||
        !test::same_run(run, fresh))
        return 27;
    // An unknown run ID: next picks the first, previous the last entry.
    auto unknown = fresh;
    unknown.start_config = static_cast<StartConfigId>(4);
    SessionFlow first, last;
    if (!first.select_next(unknown) ||
        first.selected(unknown) != StartConfigId::baseline ||
        !last.select_previous(unknown) ||
        last.selected(unknown) != StartConfigId::charged_debt ||
        !last.confirm(unknown) ||
        unknown.start_config != StartConfigId::charged_debt)
        return 28;
    return 0;
}

DifficultyModifiers mask(int bits)
{
    // An unknown mask falls back to none, so comparisons fail plainly.
    return DifficultyModifiers::from_value(bits).value_or(
        DifficultyModifiers{});
}

// Modifier selection: ascending cycle of masks, gating, confirm, independence.
int modifier_selection(RunSeed seed)
{
    const auto fresh = start_run(seed);
    auto run = fresh;
    SessionFlow flow;
    if (flow.selected_modifiers(run) != DifficultyModifiers{})
        return 40;
    for (const int expected : {1, 2, 3, 4, 5, 6, 7, 0})
        if (!flow.select_next_modifiers(run) ||
            flow.selected_modifiers(run) != mask(expected) ||
            flow.selected(run) != StartConfigId::baseline ||
            !test::same_run(run, fresh))
            return 41;
    for (const int expected : {7, 6, 5, 4, 3, 2, 1, 0})
        if (!flow.select_previous_modifiers(run) ||
            flow.selected_modifiers(run) != mask(expected))
            return 42;
    // Configuration and modifiers are chosen independently.
    for (int step = 0; step < 5; ++step)
        if (!flow.select_next_modifiers(run))
            return 43;
    if (!flow.select_next(run) ||
        flow.selected(run) != StartConfigId::preinstalled_range ||
        flow.selected_modifiers(run) != mask(5))
        return 43;
    // Confirm starts both choices and consumes them.
    const auto expected =
        *start_run(seed, StartConfigId::preinstalled_range, mask(5));
    if (!flow.confirm(run) || !test::same_run(run, expected) ||
        flow.selected_modifiers(run) != mask(5) ||
        flow.selected(run) != StartConfigId::preinstalled_range)
        return 44;
    // During a run and while a reward waits, selection is refused.
    const auto playing = run;
    if (flow.select_next_modifiers(run) ||
        flow.select_previous_modifiers(run) ||
        flow.selected_modifiers(run) != mask(5) ||
        !test::same_run(run, playing))
        return 45;
    auto reward = run;
    reward.sector.player_position = *reward.sector.sector_exit;
    reward.sector.powered_nodes = reward.sector.receiver_nodes;
    if (run_phase(reward) != RunPhase::choosing_reward ||
        flow.select_next_modifiers(reward) ||
        flow.selected_modifiers(reward) != mask(5))
        return 46;
    // A plain restart keeps mask5; a new choice switches only the mask.
    run.sector.player_integrity = 0;
    if (!flow.confirm(run) || !test::same_run(run, expected))
        return 47;
    run.sector.player_integrity = 0;
    if (!flow.select_next_modifiers(run) || !flow.confirm(run) ||
        !test::same_run(
            run, *start_run(seed, StartConfigId::preinstalled_range, mask(6))))
        return 48;
    // A consumed choice never overrides another run's modifiers.
    auto other = fresh;
    other.sector.player_integrity = 0;
    if (!flow.confirm(other) || !test::same_run(other, fresh))
        return 49;
    return 0;
}
} // namespace

int main()
{
    for (auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
    {
        const auto fresh = start_run(seed);
        auto run = fresh;
        run.sector.reserve = 0;
        run.sector.overload_debt = 99;
        run.sector.player_integrity = 1;
        run.sector.turns_taken = TurnIndex{17};
        const auto before = run;
        SessionFlow flow;
        if (flow.screen(run) != SessionScreen::start ||
            !test::same_run(run, before) || !flow.confirm(run) ||
            flow.screen(run) != SessionScreen::run ||
            !test::same_run(run, fresh) || flow.confirm(run))
            return 1;
        for (int phase = 0; phase < 5; ++phase)
        {
            run = fresh;
            run.sector.reserve = 0;
            run.sector.overload_debt = 99;
            run.sector.terminal_rotation_used = true;
            if (phase == 0)
                run.sector.player_integrity = 0;
            else if (phase == 1)
                run.sector.turns_taken = TurnIndex{48};
            else
            {
                run.sector.powered_nodes = run.sector.receiver_nodes;
                run.sector.player_position = *run.sector.sector_exit;
                run.reward_claimed = phase == 3;
                if (phase == 4)
                    run.sector_number = *SectorNumber::from_value(sector_count);
            }
            const auto terminal = phase < 2 || phase == 4;
            const auto snapshot = run;
            if (flow.screen(run) !=
                    (terminal ? SessionScreen::result : SessionScreen::run) ||
                !test::same_run(run, snapshot) ||
                flow.confirm(run) != terminal ||
                !test::same_run(run, terminal ? fresh : snapshot))
                return 2;
            if (terminal)
            {
                if (flow.confirm(run))
                    return 3;
                const auto next = resolve_run_turn(run, WaitCommand{});
                if (!next.accepted ||
                    next.state.sector.turns_taken != TurnIndex{1})
                    return 4;
            }
        }
        // Resume: the restored run carries the checkpoint configuration and
        // a restart after the result uses it, not a fresh default.
        auto restored = *restore_checkpoint(*capture_checkpoint(fresh));
        restored.sector.player_integrity = 0;
        SessionFlow resumed{true};
        if (restored.start_config != StartConfigId::baseline ||
            resumed.screen(restored) != SessionScreen::result ||
            !resumed.confirm(restored) ||
            !test::same_run(restored,
                            *start_run(seed, StartConfigId::baseline)))
            return 5;
        // ID2 survives capture/restore and a restart after the result.
        const auto range = *start_run(seed, StartConfigId::preinstalled_range);
        const auto range_checkpoint = capture_checkpoint(range);
        if (!range_checkpoint ||
            range_checkpoint->start_config != StartConfigId::preinstalled_range)
            return 7;
        auto range_restored = *restore_checkpoint(*range_checkpoint);
        if (!test::same_run(range_restored, range))
            return 8;
        range_restored.sector.reserve = 3;
        range_restored.sector.turns_taken = TurnIndex{48};
        if (resumed.screen(range_restored) != SessionScreen::result ||
            !resumed.confirm(range_restored) ||
            !test::same_run(range_restored, range))
            return 9;
        // ID3 likewise survives capture/restore and a restart.
        const auto charged = *start_run(seed, StartConfigId::charged_debt);
        const auto charged_checkpoint = capture_checkpoint(charged);
        if (!charged_checkpoint ||
            charged_checkpoint->start_config != StartConfigId::charged_debt ||
            charged_checkpoint->overload_debt != 2 ||
            charged_checkpoint->reserve != 3)
            return 10;
        auto charged_restored = *restore_checkpoint(*charged_checkpoint);
        if (!test::same_run(charged_restored, charged))
            return 11;
        charged_restored.sector.player_integrity = 0;
        charged_restored.sector.overload_debt = 0;
        if (!resumed.confirm(charged_restored) ||
            !test::same_run(charged_restored, charged))
            return 12;
        // An ID outside the catalog refuses on the start and result screens.
        for (const bool started : {false, true})
        {
            auto unknown = fresh;
            unknown.start_config = static_cast<StartConfigId>(4);
            unknown.sector.player_integrity = 0;
            unknown.sector.reserve = 1;
            const auto snapshot = unknown;
            SessionFlow refused{started};
            const auto screen = refused.screen(unknown);
            if (screen == SessionScreen::run || refused.confirm(unknown) ||
                !test::same_run(unknown, snapshot) ||
                refused.screen(unknown) != screen)
                return 6;
        }
        // A restart keeps the run's modifiers with its configuration. Bits
        // are forged because no modifier is implemented yet.
        for (const std::uint8_t bits :
             {std::uint8_t{1}, std::uint8_t{4}, std::uint8_t{7}})
        {
            const auto modifiers = std::bit_cast<DifficultyModifiers>(bits);
            const auto expected =
                *start_run(seed, StartConfigId::charged_debt, modifiers);
            auto ended = expected;
            ended.sector.player_integrity = 0;
            ended.sector.overload_debt = 0;
            ended.sector.turns_taken = TurnIndex{9};
            SessionFlow again{true};
            if (!again.confirm(ended) || !test::same_run(ended, expected) ||
                ended.sector.difficulty.bits() != bits)
                return 13;
            // The restarted, modified run is active and refuses another.
            const auto active = ended;
            if (again.confirm(ended) || !test::same_run(ended, active))
                return 14;
        }
    }
    for (auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
    {
        if (const auto code = selection(seed))
            return code;
        if (const auto code = modifier_selection(seed))
            return code;
    }
    return 0;
}
