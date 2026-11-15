#include "../support/run_checks.hpp"
#include "application/run_checkpoint.hpp"
#include "application/session_flow.hpp"
#include "core/run_turn.hpp"
#include <limits>
using namespace closed_circuit;

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
    }
    return 0;
}
