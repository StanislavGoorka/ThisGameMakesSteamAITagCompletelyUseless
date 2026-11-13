#include "../support/run_checks.hpp"
#include "application/reward_selection.hpp"
#include "core/run_start.hpp"
using namespace closed_circuit;

int main()
{
    for (unsigned loadout = 0; loadout < 4; ++loadout)
    {
        auto run = start_run(RunSeed{1});
        run.sector.powered_nodes = run.sector.receiver_nodes;
        run.sector.player_position = *run.sector.sector_exit;
        if (loadout & 1)
            static_cast<void>(run.sector.modules.add(ModuleId::rotation_range));
        if (loadout & 2)
            static_cast<void>(
                run.sector.modules.add(ModuleId::terminal_rotation));
        const auto before = run;
        const auto offer = reward_offer(run);
        RewardSelection selection;
        if (selection.candidate(run))
            return 1;
        for (unsigned i = 0; i < 6; ++i)
        {
            selection.move(run, true);
            const auto expected = offer[1] ? offer[i % 2] : offer[0];
            if (selection.candidate(run) != expected)
                return 2;
        }
        selection.reset();
        selection.move(run, false);
        if (selection.candidate(run) != (offer[1] ? offer[1] : offer[0]))
            return 3;
        selection.move(run, false);
        if (selection.candidate(run) != offer[0] ||
            !test::same_run(run, before))
            return 4;
        for (unsigned change = 0; change < 7; ++change)
        {
            selection.move(run, true);
            auto changed = run;
            switch (change)
            {
            case 0:
                changed.seed = RunSeed{2};
                break;
            case 1:
                changed.sector_number = *SectorNumber::from_value(2);
                break;
            case 2:
                changed.reward_claimed = true;
                break;
            case 3:
                changed.sector.player_integrity = 0;
                break;
            case 4:
                changed.sector.player_position = *changed.sector.sector_entry;
                break;
            case 5:
                changed.sector_number = *SectorNumber::from_value(3);
                break;
            case 6:
                static_cast<void>(
                    changed.sector.modules.add(ModuleId::rotation_range));
                static_cast<void>(
                    changed.sector.modules.add(ModuleId::terminal_rotation));
                break;
            }
            if (selection.candidate(changed))
                return 5;
            selection.sync(changed);
            if (selection.candidate(changed) || selection.candidate(run))
                return 6;
            selection.reset();
        }
        auto playing = start_run(RunSeed{1});
        selection.move(playing, true);
        if (selection.candidate(playing) ||
            reward_offer(playing) != ModuleOffer{})
            return 7;
    }
    return 0;
}
