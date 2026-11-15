#include "../support/checkpoint_files.hpp"
#include "../support/run_witness.hpp"
#include "application/run_achievements.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_start.hpp"
#include "platform/windows/local_achievement_sink.hpp"

#include <iostream>
#include <map>
#include <string>

using namespace closed_circuit;
namespace
{
// A full run through the certified commands. Every accepted turn hands its
// events to RunAchievements over the real profile file; a separate
// evaluator sees the same events. Returns that evaluator's mask, or nothing
// when the run or a flush fails.
std::optional<std::uint32_t> full_run(std::uint64_t raw, StartConfigId config,
                                      DifficultyModifiers modifiers,
                                      const std::filesystem::path &profile,
                                      std::uint64_t branch)
{
    auto started = start_run(RunSeed{raw}, config, modifiers);
    if (!started)
        return std::nullopt;
    auto run = *started;
    windows::LocalAchievementSink sink(profile);
    RunAchievements achievements(
        DeliveryScope{run.seed, {1}, {branch}, *SectorNumber::from_value(1)},
        sink, RunHistoryOrigin::fresh_run);
    MvpAchievementEvaluator evaluator{RunHistoryOrigin::fresh_run};
    std::uint32_t evaluated = 0;
    for (;;)
    {
        const auto commands = test::sector_commands(run);
        if (!commands)
            return std::nullopt;
        for (const auto &command : *commands)
        {
            const auto live = resolve_run_turn_events(run, command);
            if (!live.turn.accepted)
                return std::nullopt;
            for (const auto &event : live.events)
                evaluated |= evaluator.evaluate(event);
            achievements.publish(achievements.prepare(live.events));
            if (!achievements.flush() || achievements.pending())
                return std::nullopt;
            run = live.turn.state;
        }
        const auto phase = run_phase(run);
        if (phase == RunPhase::won)
            return evaluated;
        if (phase == RunPhase::choosing_reward)
        {
            const auto choice = test::witness_choice(run);
            if (!choice || !choose_run_module(run, *choice))
                return std::nullopt;
        }
        else if (phase != RunPhase::ready_for_next)
            return std::nullopt;
        const auto next = advance_run(run);
        if (!next.accepted)
            return std::nullopt;
        run = next.state;
    }
}
} // namespace

int main()
{
    test::CheckpointDirectory directory;
    const std::uint64_t seeds[]{0, 1, 2, 3, 4, 5, 42, UINT64_MAX};
    const LocalProfile initial{0, 3, 42, true};
    // seed → configuration → mask → achievement bits read from the file.
    std::map<std::uint64_t,
             std::map<unsigned, std::map<unsigned, std::uint16_t>>>
        stored_by;
    for (const auto config : start_config_catalog)
        for (const unsigned mask : {0U, 7U})
        {
            const auto modifiers = DifficultyModifiers::from_value(mask);
            if (!modifiers)
                return 5;
            const auto config_value = static_cast<unsigned>(config);
            unsigned completed = 0;
            for (std::size_t i = 0; i < std::size(seeds); ++i)
            {
                const auto started =
                    start_run(RunSeed{seeds[i]}, config, *modifiers);
                if (!started || !test::witness_completes(*started))
                    continue;
                const auto path = directory.path /
                                  (L"profil " + std::to_wstring(config_value) +
                                   L" " + std::to_wstring(mask) + L" " +
                                   std::to_wstring(i) + L" Łódź.dat");
                auto backup = path;
                backup += L".bak";
                if (!windows::save_profile_file(path, initial))
                    return 6;
                const auto evaluated =
                    full_run(seeds[i], config, *modifiers, path, 1);
                const auto read = windows::read_profile_file(path);
                // The file holds exactly the evaluated candidates and keeps
                // the other profile fields; first_delivery, first_sector and
                // completed_run every time.
                if (!evaluated || !read.profile ||
                    read.profile->achievement_bits != *evaluated ||
                    read.profile->variant_bits != initial.variant_bits ||
                    read.profile->master_volume != initial.master_volume ||
                    read.profile->muted != initial.muted ||
                    (*evaluated & 0x13u) != 0x13u)
                {
                    std::cerr << "Profile run failed: config " << config_value
                              << " mask " << mask << " seed index " << i
                              << '\n';
                    return 2;
                }
                // A new owner replaying the same run finds every ID owned and
                // writes neither the profile nor its backup.
                const auto main_bytes = test::read_bytes(path);
                const auto backup_bytes = test::read_bytes(backup);
                if (full_run(seeds[i], config, *modifiers, path, 2) !=
                        evaluated ||
                    test::read_bytes(path) != main_bytes ||
                    test::read_bytes(backup) != backup_bytes)
                {
                    std::cerr << "Replay changed the profile: config "
                              << config_value << " mask " << mask
                              << " seed index " << i << '\n';
                    return 7;
                }
                stored_by[seeds[i]][config_value][mask] =
                    read.profile->achievement_bits;
                ++completed;
            }
            std::cout << "config " << config_value << " mask " << mask
                      << " completed " << completed << '\n';
            if (completed < 3)
            {
                std::cerr << "Too few completed runs for config "
                          << config_value << " mask " << mask << '\n';
                return 3;
            }
        }
    // Only ID2 stores won_preinstalled_range (0x40), only ID3
    // won_charged_debt (0x80) and only mask 7 the four modifier wins (0xf00);
    // every other bit matches the ID1 mask0 run of the same seed.
    for (const auto &[seed, by_config] : stored_by)
        for (const auto &[config, by_mask] : by_config)
            for (const auto &[mask, stored] : by_mask)
                if (((stored & 0x40) != 0) != (config == 2) ||
                    ((stored & 0x80) != 0) != (config == 3) ||
                    (stored & 0xf00) != (mask == 7 ? 0xf00 : 0) ||
                    (by_config.contains(1) && by_config.at(1).contains(0) &&
                     (stored & ~0xfc0u) != (by_config.at(1).at(0) & ~0xfc0u)))
                {
                    std::cerr << "Stored achievements differ: seed " << seed
                              << " config " << config << " mask " << mask
                              << '\n';
                    return 4;
                }
    return 0;
}
