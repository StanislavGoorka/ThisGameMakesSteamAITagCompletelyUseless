#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/run_window.hpp"
#include <iostream>
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    const AssetManifest assets{presentation::atlas_reference()};
    for (int kind = 0; kind < 3; ++kind)
    {
        auto window =
            windows::NativeWindow::create(catalog, "developer-window-title");
        if (!window.window)
            return 2;
        auto run = start_run(RunSeed{1});
        run.sector.powered_nodes = run.sector.receiver_nodes;
        run.sector.player_position = *run.sector.sector_exit;
        if (kind == 2)
        {
            static_cast<void>(run.sector.modules.add(ModuleId::rotation_range));
            static_cast<void>(
                run.sector.modules.add(ModuleId::terminal_rotation));
        }
        auto made = windows::RunWindow::create(*window.window, run, argv[1],
                                               assets, {}, &catalog);
        if (!made.session)
            return 3;
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
        for (int boundary = 0; boundary < 2; ++boundary)
        {
            const auto before = run;
            if (run_phase(run) == RunPhase::choosing_reward)
            {
                tap(VK_RETURN);
                if (!test::same_run(session.state(), before) ||
                    session.view().label_text().find(
                        L"[AI PLACEHOLDER] Select an available") ==
                        std::wstring_view::npos)
                    return 4;
                tap(VK_RIGHT);
                const auto first = module_offer(run.sector.modules)[0];
                if (session.reward_candidate() != first)
                    return 5;
                SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
                tap(VK_RETURN);
                SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
                SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 1LL << 30);
                SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
                if (session.reward_candidate() ||
                    !test::same_run(session.state(), before))
                    return 6;
                tap(kind ? VK_LEFT : VK_RIGHT);
                const auto selected = session.reward_candidate();
                if (!selected || !choose_run_module(run, *selected))
                    return 7;
            }
            SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 0);
            for (int repeat = 0; repeat < 10; ++repeat)
                SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN,
                             repeat % 2 ? 1LL << 30 : 0);
            SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
            const auto next = advance_run(run);
            if (!next.accepted ||
                !test::same_run(session.state(), next.state) ||
                !test::same_state(session.view().board(), next.state.sector) ||
                !session.view().showing_board() || session.reward_candidate() ||
                session.selection().planning() ||
                session.selection().candidate() ||
                session.selection().pointed() || window.window->pointed() ||
                !session.view().packet_route().empty())
                return 8;
            run = next.state;
            // Solve the generated perimeter through actual input, comparing
            // every full state.
            const auto apply = [&](Command command, WPARAM key,
                                   std::optional<NodeId> target = {})
            {
                if (target)
                {
                    RECT client{};
                    GetClientRect(hwnd, &client);
                    const auto layout = *presentation::fit_board(
                        static_cast<float>(client.right),
                        static_cast<float>(client.bottom) -
                            session.view().board_top());
                    const auto x = static_cast<int>(
                        layout.left +
                        (target->value() % 7 + .5f) * layout.cell);
                    const auto y = static_cast<int>(
                        session.view().board_top() + layout.top +
                        (target->value() / 7 + .5f) * layout.cell);
                    SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
                }
                const auto result = resolve_run_turn(run, command);
                tap(key);
                if (!result.accepted ||
                    !test::same_run(session.state(), result.state))
                    return false;
                run = result.state;
                return true;
            };
            const bool top = run.sector.receiver_nodes[4];
            const auto at = [top](int i)
            {
                return top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                           : (i <= 6 ? i * 7 : 42 + i - 6);
            };
            const auto direction = [](int from, int to) {
                return to == from + 1   ? 2
                       : to == from - 1 ? 8
                       : to > from      ? 4
                                        : 1;
            };
            for (int i = 1; i < 13; ++i)
            {
                const auto target = *NodeId::from_value(at(i));
                const int desired = direction(at(i), at(i - 1)) |
                                    (i < 12 ? direction(at(i), at(i + 1)) : 0);
                int rotations = 0;
                while (run.sector.ports[at(i)].bits() != desired &&
                       rotations++ < 4)
                    if (!apply(RotateCommand{target}, 'R', target))
                        return 9;
                const int delta = at(i) - at(i - 1);
                if (!apply(MoveCommand{target},
                           delta == 1 ? VK_RIGHT : VK_DOWN))
                    return 10;
            }
            while (run_phase(run) == RunPhase::playing)
                if (!apply(WaitCommand{}, VK_SPACE))
                    return 11;
            if (boundary == 0 &&
                run_phase(run) != (kind == 2 ? RunPhase::ready_for_next
                                             : RunPhase::choosing_reward))
                return 12;
        }
        if (session.screen() != SessionScreen::result ||
            run_phase(run) != RunPhase::won)
            return 13;
        SendMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    CoUninitialize();
    return 0;
}
