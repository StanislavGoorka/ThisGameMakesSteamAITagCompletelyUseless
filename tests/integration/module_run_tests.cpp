#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "../support/run_witness.hpp"
#include "application/run_checkpoint.hpp"
#include "core/run_start.hpp"
#include "developer_text.hpp"
#include "platform/windows/checkpoint_read.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include "platform/windows/run_window.hpp"

#include <iostream>
#include <set>
#include <string>

using namespace closed_circuit;
namespace
{
// Registry values are ASCII placeholders; widening keeps every character.
std::wstring widen(std::string_view text)
{
    return {text.begin(), text.end()};
}

// A full run through the certified commands. Every sector start is saved and
// read back by the real file adapter; the restored copy replays the sector
// next to the live run and must reach the same states and offers.
bool run_with_restores(std::uint64_t raw, const std::filesystem::path &main,
                       ModuleLoadout &final_modules)
{
    auto run = start_run(RunSeed{raw});
    auto backup = main;
    backup += L".bak";
    std::optional<std::vector<std::uint8_t>> previous;
    for (;;)
    {
        const auto checkpoint = capture_checkpoint(run);
        if (!checkpoint || !windows::save_checkpoint_file(main, *checkpoint))
            return false;
        if (previous ? test::read_bytes(backup) != *previous
                     : std::filesystem::exists(backup))
            return false;
        previous = test::read_bytes(main);
        const auto read = windows::read_checkpoint_file(main);
        auto restored = read.checkpoint ? restore_checkpoint(*read.checkpoint)
                                        : std::nullopt;
        if (!restored || *read.checkpoint != *checkpoint ||
            !test::same_run(*restored, run))
            return false;
        const auto commands = test::sector_commands(run);
        if (!commands)
            return false;
        for (const auto &command : *commands)
        {
            const auto live = resolve_run_turn(run, command);
            const auto again = resolve_run_turn(*restored, command);
            if (!live.accepted || !again.accepted ||
                !test::same_run(live.state, again.state))
                return false;
            run = live.state;
            restored = again.state;
        }
        const auto phase = run_phase(run);
        if (phase == RunPhase::won)
        {
            final_modules = run.sector.modules;
            return true;
        }
        if (reward_offer(run) != reward_offer(*restored))
            return false;
        if (phase == RunPhase::choosing_reward)
        {
            const auto choice = test::witness_choice(run);
            if (!choice || !choose_run_module(run, *choice) ||
                !choose_run_module(*restored, *choice))
                return false;
        }
        else if (phase != RunPhase::ready_for_next)
            return false;
        const auto next = advance_run(run);
        const auto next_again = advance_run(*restored);
        if (!next.accepted || !next_again.accepted ||
            !test::same_run(next.state, next_again.state))
            return false;
        run = next.state;
    }
}

// The reward overlay shows both offered rows with their own marked labels;
// the cursor and Enter install the second offered module.
int window_offer(const TextCatalog &catalog, const std::filesystem::path &root,
                 std::uint64_t raw, std::array<std::string_view, 2> texts)
{
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return 20;
    auto run = start_run(RunSeed{raw});
    run.sector.powered_nodes = run.sector.receiver_nodes;
    run.sector.player_position = *run.sector.sector_exit;
    const auto offer = reward_offer(run);
    if (!offer[0] || !offer[1] || static_cast<unsigned>(*offer[1]) < 3)
        return 21;
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(*window.window, run, root, assets,
                                           {}, &catalog);
    if (!made.session)
        return 22;
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
    const auto label = session.view().label_text();
    for (std::size_t i = 0; i < offer.size(); ++i)
    {
        // Literal registry IDs, independent of the projection under test.
        const auto *value = catalog.find(texts[i]);
        if (!value || !value->starts_with("[AI PLACEHOLDER] ") ||
            label.find(widen(*value + " " + std::to_string(i + 1))) ==
                std::wstring_view::npos)
            return 23;
    }
    tap(VK_RIGHT);
    tap(VK_RIGHT);
    if (session.reward_candidate() != offer[1])
        return 24;
    auto expected = run;
    if (!choose_run_module(expected, *offer[1]))
        return 25;
    const auto next = advance_run(expected);
    tap(VK_RETURN);
    if (!next.accepted || !test::same_run(session.state(), next.state) ||
        !session.state().sector.modules.contains(*offer[1]) ||
        !session.view().showing_board())
        return 26;
    return 0;
}
} // namespace

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    test::CheckpointDirectory directory;
    std::set<unsigned> admitted;
    unsigned runs_with_new = 0;
    const std::uint64_t seeds[]{0, 1, 2, 3, 4, 5, 42, UINT64_MAX};
    for (std::size_t i = 0; i < std::size(seeds); ++i)
    {
        ModuleLoadout modules;
        const auto main =
            directory.path / (L"run " + std::to_wstring(i) + L" Łódź.bin");
        if (!run_with_restores(seeds[i], main, modules))
        {
            std::cerr << "Full run with restores failed for seed index " << i
                      << '\n';
            return 2;
        }
        bool has_new = false;
        std::cout << "seed " << seeds[i] << " modules";
        for (const auto slot : modules.slots())
        {
            const unsigned raw = slot ? static_cast<unsigned>(*slot) : 0U;
            std::cout << ' ' << raw;
            if (raw >= 3)
            {
                has_new = true;
                admitted.insert(raw);
            }
        }
        std::cout << '\n';
        runs_with_new += has_new;
    }
    if (runs_with_new != std::size(seeds) || admitted.size() < 5)
    {
        std::cerr << "Too few admitted modules in completed runs\n";
        return 3;
    }
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    // Offers after sector 1: seed 0 -> {1, 3}, seed 1 -> {5, 10}.
    if (const int code = window_offer(
            catalog, argv[1], 0, {"reward-range", "reward-delivery-reserve"}))
        return code;
    if (const int code =
            window_offer(catalog, argv[1], 1,
                         {"reward-packet-stride", "reward-overdrive-range"}))
        return code;
    return 0;
}
