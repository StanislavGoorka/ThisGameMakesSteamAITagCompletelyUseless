#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/profile_save.hpp"
#include "platform/windows/run_window.hpp"
#include <iostream>
using namespace closed_circuit;

bool failed(std::string_view stage)
{
    std::cerr << stage << '\n';
    return false;
}
bool scenario(const std::filesystem::path &root, const TextCatalog &catalog,
              int kind)
{
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return failed("window");
    auto run = start_run(RunSeed{17});
    WPARAM action = VK_SPACE;
    if (kind)
    {
        run.sector.reserve = 0;
        run.sector.overload_debt = 99;
        run.sector.terminal_rotation_used = true;
        static_cast<void>(run.sector.modules.add(ModuleId::rotation_range));
        if (kind == 1)
        {
            run.sector.player_integrity = 1;
            run.sector.pulses.push_back(
                {*EntityId::from_value(99), run.sector.player_position});
        }
        else if (kind == 2)
            run.sector.turns_taken = TurnIndex{47};
        else
        {
            run.sector_number = *SectorNumber::from_value(3);
            run.sector.powered_nodes = run.sector.receiver_nodes;
            const int exit = run.sector.sector_exit->value();
            run.sector.player_position =
                *NodeId::from_value(exit >= 7 ? exit - 7 : exit + 7);
            run.sector.ports.fill(*PortMask::from_bits(15));
            action = exit >= 7 ? VK_DOWN : VK_UP;
        }
    }
    const auto initial = run;
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(*window.window, run, root, assets,
                                           {}, &catalog, kind == 0);
    if (!made.session)
        return failed("create session");
    const auto hwnd = window.window->handle();
    auto &session = *made.session;
    const auto tap = [&](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    const auto held_enter = [&]
    {
        SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 0);
        for (int repeat = 0; repeat < 20; ++repeat)
            SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN,
                         repeat % 2 ? 1LL << 30 : 0);
        SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
    };
    const std::array<WPARAM, 6> ignored{VK_SPACE, VK_TAB, VK_RIGHT,
                                        'R',      'O',    VK_ESCAPE};
    SetWindowPos(hwnd, nullptr, -32000, -32000, 800, 600,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    if (kind == 0)
    {
        if (session.screen() != SessionScreen::start ||
            session.view().showing_board() ||
            session.view().label_text().find(
                L"[AI PLACEHOLDER] Press Enter to start.") ==
                std::wstring_view::npos ||
            session.view().node_at(100, 100, 800, 600))
            return failed("start display");
        for (auto key : ignored)
            tap(key);
        if (!test::same_run(session.state(), initial) ||
            session.selection().planning())
            return failed("start refusal");
        held_enter();
        run = start_run(initial.seed);
        if (session.screen() != SessionScreen::run ||
            !session.view().showing_board() ||
            !test::same_run(session.state(), run))
            return failed("start confirm");
        for (int i = 0; i < 48; ++i)
        {
            tap(VK_SPACE);
            run = resolve_run_turn(run, WaitCommand{}).state;
        }
    }
    else
    {
        const Command command =
            kind == 3 ? Command{MoveCommand{*run.sector.sector_exit}}
                      : Command{WaitCommand{}};
        const auto next = resolve_run_turn(run, command);
        if (!next.accepted)
            return failed("fixture command");
        tap(VK_TAB);
        tap(action);
        if (session.screen() != SessionScreen::run ||
            !session.view().showing_board() ||
            !test::same_run(session.state(), run) ||
            !test::same_state(session.view().board(), next.state.sector))
            return failed("terminal preview");
        tap(VK_RETURN);
        run = next.state;
    }
    if (session.screen() != SessionScreen::result ||
        session.view().showing_board() ||
        !test::same_run(session.state(), run) ||
        session.selection().planning() || session.selection().candidate() ||
        session.selection().pointed() || window.window->pointed() ||
        !session.view().packet_route().empty())
        return failed("result display/reset");
    const auto message = kind == 1   ? "session-death"
                         : kind == 3 ? "session-won"
                                     : "session-overload";
    const auto expected = windows::read_text_utf16(catalog, message);
    if (!expected.text || session.view().label_text().find(*expected.text) ==
                              std::wstring_view::npos)
        return failed("result label");
    for (auto key : ignored)
        tap(key);
    if (!test::same_run(session.state(), run))
        return failed("result refusal");
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    tap(VK_RETURN);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
    if (!test::same_run(session.state(), run))
        return failed("focus/repeat refusal");
    for (auto size : {SIZE{800, 600}, SIZE{80, 100}, SIZE{600, 800}})
    {
        SetWindowPos(hwnd, nullptr, 0, 0, size.cx, size.cy,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        UpdateWindow(hwnd);
        if (FAILED(session.error()) || session.view().showing_board())
            return failed("result repaint");
    }
    held_enter();
    const auto fresh = start_run(initial.seed);
    if (!test::same_run(session.state(), fresh) ||
        session.screen() != SessionScreen::run ||
        !session.view().showing_board() || session.selection().planning() ||
        session.selection().candidate() || window.window->pointed())
        return failed("restart reset");
    tap(VK_SPACE);
    if (!test::same_run(session.state(),
                        resolve_run_turn(fresh, WaitCommand{}).state))
        return failed("first restarted turn");
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return SUCCEEDED(session.error());
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    test::CheckpointDirectory directory;
    const auto profile = directory.path / L"profile.dat";
    const LocalProfile baseline{1, 2, 31, false}, unlocked{31, 3, 17, true};
    if (!windows::save_profile_file(profile, baseline) ||
        !windows::save_profile_file(profile, unlocked))
        return 2;
    const auto backup = std::filesystem::path{profile.native() + L".bak"};
    const auto before = test::read_bytes(profile),
               before_backup = test::read_bytes(backup);
    for (int kind = 0; kind < 4; ++kind)
        if (!scenario(argv[1], catalog, kind) ||
            test::read_bytes(profile) != before ||
            test::read_bytes(backup) != before_backup ||
            windows::read_profile_file(profile).profile != unlocked)
        {
            std::cerr << "Session scenario failed: " << kind << '\n';
            return 3;
        }
    CoUninitialize();
    return 0;
}
