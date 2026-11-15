#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "../support/run_witness.hpp"
#include "application/mvp_achievement_evaluator.hpp"
#include "application/run_checkpoint.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_start.hpp"
#include "developer_text.hpp"
#include "platform/windows/checkpoint_read.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include "platform/windows/run_window.hpp"

#include <iostream>
#include <map>
#include <string>

using namespace closed_circuit;
namespace
{
// Registry values are ASCII placeholders; widening keeps every character.
std::wstring widen(std::string_view text)
{
    return {text.begin(), text.end()};
}

// A full run from the chosen configuration through the certified commands.
// Every sector start is saved and read back by the real file adapter; the
// restored copy replays the sector next to the live run and must reach the
// same states and offers. Returns the achievement mask of the live run.
std::optional<std::uint32_t>
run_with_restores(std::uint64_t raw, StartConfigId config,
                  const std::filesystem::path &main)
{
    auto run = *start_run(RunSeed{raw}, config);
    MvpAchievementEvaluator evaluator{RunHistoryOrigin::fresh_run};
    std::uint32_t earned = 0;
    auto backup = main;
    backup += L".bak";
    std::optional<std::vector<std::uint8_t>> previous;
    for (;;)
    {
        const auto checkpoint = capture_checkpoint(run);
        if (!checkpoint || checkpoint->start_config != config ||
            !windows::save_checkpoint_file(main, *checkpoint))
            return std::nullopt;
        if (previous ? test::read_bytes(backup) != *previous
                     : std::filesystem::exists(backup))
            return std::nullopt;
        previous = test::read_bytes(main);
        const auto read = windows::read_checkpoint_file(main);
        auto restored = read.checkpoint ? restore_checkpoint(*read.checkpoint)
                                        : std::nullopt;
        if (!restored || *read.checkpoint != *checkpoint ||
            !test::same_run(*restored, run) || restored->start_config != config)
            return std::nullopt;
        const auto commands = test::sector_commands(run);
        if (!commands)
            return std::nullopt;
        for (const auto &command : *commands)
        {
            const auto live = resolve_run_turn_events(run, command);
            const auto again = resolve_run_turn(*restored, command);
            if (!live.turn.accepted || !again.accepted ||
                !test::same_run(live.turn.state, again.state))
                return std::nullopt;
            for (const auto &event : live.events)
                earned |= evaluator.evaluate(event);
            run = live.turn.state;
            restored = again.state;
        }
        const auto phase = run_phase(run);
        if (phase == RunPhase::won)
            return run.start_config == config ? std::optional{earned}
                                              : std::nullopt;
        if (reward_offer(run) != reward_offer(*restored))
            return std::nullopt;
        if (phase == RunPhase::choosing_reward)
        {
            const auto choice = test::witness_choice(run);
            if (!choice || !choose_run_module(run, *choice) ||
                !choose_run_module(*restored, *choice))
                return std::nullopt;
        }
        else if (phase != RunPhase::ready_for_next)
            return std::nullopt;
        const auto next = advance_run(run);
        const auto next_again = advance_run(*restored);
        if (!next.accepted || !next_again.accepted ||
            !test::same_run(next.state, next_again.state))
            return std::nullopt;
        run = next.state;
    }
}

// The start screen lists the marked label of the selected configuration;
// arrows cycle it and Enter starts exactly that configuration.
int window_choice(const TextCatalog &catalog, const std::filesystem::path &root,
                  std::uint64_t raw)
{
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return 20;
    const auto run = start_run(RunSeed{raw});
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(*window.window, run, root, assets,
                                           {}, &catalog, true);
    if (!made.session)
        return 21;
    auto &session = *made.session;
    const auto hwnd = window.window->handle();
    const auto tap = [&](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1000, 800,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    // Literal registry IDs, independent of the projection under test.
    const auto shows = [&](std::string_view id)
    {
        const auto *value = catalog.find(id);
        return value && value->starts_with("[AI PLACEHOLDER] ") &&
               session.view().label_text().find(widen(*value)) !=
                   std::wstring_view::npos;
    };
    const std::string_view order[]{
        "start-config-baseline", "start-config-range", "start-config-charged"};
    for (std::size_t step = 0; step < 4; ++step)
    {
        if (session.screen() != SessionScreen::start ||
            !shows(order[step % 3]) || shows(order[(step + 1) % 3]) ||
            shows(order[(step + 2) % 3]) ||
            !test::same_run(session.state(), run))
            return 22;
        tap(VK_RIGHT);
    }
    // After four steps the cursor sits on ID2; one more right selects ID3.
    tap(VK_RIGHT);
    if (!shows("start-config-charged"))
        return 23;
    tap(VK_RETURN);
    if (session.screen() != SessionScreen::run ||
        !test::same_run(
            session.state(),
            *start_run(RunSeed{raw}, StartConfigId::charged_debt)) ||
        shows("start-config-charged") || !session.view().showing_board())
        return 24;
    return 0;
}
} // namespace

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    test::CheckpointDirectory directory;
    const std::uint64_t seeds[]{0, 1, 2, 3, 4, 5, 42, UINT64_MAX};
    // The certificates ignore modules and the extra debt pulse, so a seed
    // counts only when they still finish the run; this bounds the proof and
    // is not a balance measurement.
    std::map<std::uint64_t, std::map<unsigned, std::uint32_t>> masks;
    for (const auto config : start_config_catalog)
    {
        unsigned completed = 0;
        std::cout << "config " << static_cast<unsigned>(config) << " seeds";
        for (std::size_t i = 0; i < std::size(seeds); ++i)
        {
            if (!test::witness_completes(*start_run(RunSeed{seeds[i]}, config)))
                continue;
            const auto main =
                directory.path /
                (L"start " + std::to_wstring(static_cast<unsigned>(config)) +
                 L" " + std::to_wstring(i) + L" Łódź.bin");
            const auto earned = run_with_restores(seeds[i], config, main);
            // first_delivery, first_sector and completed_run every time.
            if (!earned || (*earned & 0x13u) != 0x13u)
            {
                std::cerr << "Full run failed: config "
                          << static_cast<unsigned>(config) << " seed index "
                          << i << '\n';
                return 2;
            }
            masks[seeds[i]][static_cast<unsigned>(config)] = *earned;
            ++completed;
            std::cout << ' ' << seeds[i];
        }
        std::cout << '\n';
        if (completed < 3)
        {
            std::cerr << "Too few completed runs for config "
                      << static_cast<unsigned>(config) << '\n';
            return 3;
        }
    }
    // Same commands, same predicates: where ID1 and another configuration
    // both finish a seed, they earn the same achievements, except that only
    // ID2 earns won_preinstalled_range (0x40).
    for (const auto &[seed, by_config] : masks)
        for (const auto &[config, earned] : by_config)
            if (((earned & 0x40) != 0) != (config == 2) ||
                (by_config.contains(1) &&
                 (earned & ~0x40u) != (by_config.at(1) & ~0x40u)))
            {
                std::cerr << "Achievement mask differs: seed " << seed
                          << " config " << config << '\n';
                return 4;
            }
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    if (const int code = window_choice(catalog, argv[1], 17))
        return code;
    CoUninitialize();
    return 0;
}
