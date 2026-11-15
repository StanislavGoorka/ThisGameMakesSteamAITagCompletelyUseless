#include "../support/checkpoint_files.hpp"
#include "../support/profile_vectors.hpp"
#include "../support/run_witness.hpp"
#include "application/run_achievements.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_start.hpp"
#include "platform/windows/local_achievement_sink.hpp"

#include <array>
#include <cerrno>
#include <cwchar>
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
std::optional<std::uint32_t>
full_run(std::uint64_t raw, StartConfigId config, DifficultyModifiers modifiers,
         const std::filesystem::path &profile, std::uint64_t branch,
         RunHistoryOrigin origin = RunHistoryOrigin::fresh_run)
{
    auto started = start_run(RunSeed{raw}, config, modifiers);
    if (!started)
        return std::nullopt;
    auto run = *started;
    windows::LocalAchievementSink sink(profile);
    RunAchievements achievements(
        DeliveryScope{run.seed, {1}, {branch}, *SectorNumber::from_value(1)},
        sink, origin);
    MvpAchievementEvaluator evaluator{origin};
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

// A whole decimal argument, or nothing.
std::optional<std::uint64_t> number(const wchar_t *text)
{
    if (*text < L'0' || *text > L'9')
        return std::nullopt;
    wchar_t *end = nullptr;
    errno = 0;
    const auto value = std::wcstoull(text, &end, 10);
    if (errno != 0 || *end != L'\0')
        return std::nullopt;
    return value;
}

// Child modes: --run path config mask seed branch restored finishes a run
// through the file sink (restored 1 means unknown history); --read path bits
// checks a schema 2 file holding exactly bits.
int worker(int argc, wchar_t **argv)
{
    const std::wstring_view mode = argv[1];
    const std::filesystem::path path = argv[2];
    if (mode == L"--run" && argc == 8)
    {
        const auto config = number(argv[3]);
        const auto mask = number(argv[4]);
        const auto seed = number(argv[5]);
        const auto branch = number(argv[6]);
        const auto restored = number(argv[7]);
        const auto id = config ? start_config_from_value(*config)
                               : std::optional<StartConfigId>{};
        const auto modifiers = mask ? DifficultyModifiers::from_value(*mask)
                                    : std::optional<DifficultyModifiers>{};
        return id && modifiers && seed && branch && restored &&
                       *restored <= 1 &&
                       full_run(*seed, *id, *modifiers, path, *branch,
                                *restored == 1
                                    ? RunHistoryOrigin::restored_checkpoint
                                    : RunHistoryOrigin::fresh_run)
                   ? 0
                   : 2;
    }
    if (mode == L"--read" && argc == 4)
    {
        const auto bits = number(argv[3]);
        const auto read = windows::read_profile_file(path);
        const auto stored = test::read_bytes(path);
        return bits && read.profile &&
                       read.profile->achievement_bits == *bits &&
                       stored.size() == 24 && stored[4] == 2
                   ? 0
                   : 2;
    }
    return 2;
}

bool launch(const std::vector<std::wstring> &arguments)
{
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) + L"\"";
    for (const auto &argument : arguments)
        command += L" \"" + argument + L"\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION information{};
    if (!CreateProcessW(executable.data(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &information))
        return false;
    windows::FileHandle process(information.hProcess);
    windows::FileHandle thread(information.hThread);
    if (WaitForSingleObject(process.get(), 60000) != WAIT_OBJECT_0)
    {
        TerminateProcess(process.get(), 99);
        static_cast<void>(WaitForSingleObject(process.get(), 10000));
        return false;
    }
    DWORD code = 99;
    return GetExitCodeProcess(process.get(), &code) && code == 0;
}

// A schema 1 profile owning ID1, ID3 and ID5 is migrated by a child that
// finishes an ID3 mask7 run with unknown history, so run_without_overdrive
// (ID3) can only survive through migration. New processes read the same bits
// and a replay leaves the bytes alone. A control ID1 mask0 run stores none of
// ID7..12.
int restart(const std::filesystem::path &directory, std::uint64_t seed)
{
    const auto all = DifficultyModifiers::from_value(7);
    if (!all)
        return 10;
    // The expected candidates come from an in-process run on a scratch file.
    const auto scratch = directory / L"scratch.dat";
    if (!windows::save_profile_file(scratch, LocalProfile{}))
        return 11;
    const auto evaluated =
        full_run(seed, StartConfigId::charged_debt, *all, scratch, 1,
                 RunHistoryOrigin::restored_checkpoint);
    if (!evaluated || (*evaluated & 0xfc4u) != 0xf80u)
        return 12;
    const auto legacy = test::profile_v1_vectors[2];
    const auto expected = test::profile_values[2].achievement_bits | *evaluated;
    const auto path = directory / L"restart Łódź.dat";
    auto backup = path;
    backup += L".bak";
    test::write_bytes(path, legacy);
    const auto text = [](std::uint64_t value)
    { return std::to_wstring(value); };
    if (!launch({L"--run", path, L"3", L"7", text(seed), L"1", L"1"}))
        return 13;
    // A child reporting other bits must fail, so success above is evidence.
    if (!launch({L"--read", path, text(expected)}) ||
        launch({L"--read", path, text(expected ^ 0x800u)}))
        return 14;
    const auto main_bytes = test::read_bytes(path);
    const auto backup_bytes = test::read_bytes(backup);
    if (!launch({L"--run", path, L"3", L"7", text(seed), L"2", L"1"}) ||
        test::read_bytes(path) != main_bytes ||
        test::read_bytes(backup) != backup_bytes ||
        !launch({L"--read", path, text(expected)}))
        return 15;
    const auto control = directory / L"control.dat";
    if (!windows::save_profile_file(control, LocalProfile{}) ||
        !launch({L"--run", control, L"1", L"0", text(seed), L"1", L"0"}))
        return 16;
    const auto read = windows::read_profile_file(control);
    if (!read.profile || (read.profile->achievement_bits & 0xfc0u) != 0 ||
        (read.profile->achievement_bits & 0x13u) != 0x13u ||
        !launch({L"--read", control, text(read.profile->achievement_bits)}))
        return 17;
    return 0;
}
} // namespace

int corpus()
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
    // The first seed finishing ID3 with every modifier also proves restart.
    for (const auto &[seed, by_config] : stored_by)
        if (by_config.contains(3) && by_config.at(3).contains(7))
            return restart(directory.path, seed);
    return 8;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc >= 3)
        return worker(argc, argv);
    return argc == 1 ? corpus() : 1;
}
