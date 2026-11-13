#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto node = [](int i) { return *NodeId::from_value(i); };
    const TextRecord record{"title", "[AI PLACEHOLDER] Planning test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto window = windows::NativeWindow::create(catalog, "title");
    if (!window.window)
        return 2;
    auto initial = start_run(RunSeed{0});
    initial.sector = GameState{.player_position = node(0)};
    auto &s = initial.sector;
    s.ports[0] = *PortMask::from_bits(2);
    s.ports[1] = *PortMask::from_bits(10);
    s.ports[2] = *PortMask::from_bits(8);
    s.rotatable_nodes[0] = s.rotatable_nodes[1] = true;
    s.sector_entry = node(0);
    s.sector_exit = node(48);
    s.receiver_nodes[2] = s.receiver_nodes[48] = true;
    const AssetManifest assets{presentation::atlas_reference()};
    auto made =
        windows::RunWindow::create(*window.window, initial, argv[1], assets);
    if (!made.session)
        return 3;
    const auto hwnd = window.window->handle();
    SetWindowPos(hwnd, nullptr, -32000, -32000, 640, 480,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    const auto tap = [&](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        for (int i = 0; i < 8; ++i)
            SendMessageW(hwnd, WM_KEYDOWN, key, i % 2 ? (1LL << 30) : 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    const auto hover = [&](int value)
    {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        const auto fit = *presentation::fit_board(
            static_cast<float>(rect.right), static_cast<float>(rect.bottom));
        const int x = static_cast<int>(fit.left + (value % 7 + .5f) * fit.cell);
        const int y = static_cast<int>(fit.top + (value / 7 + .5f) * fit.cell);
        SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
        return window.window->pointed() == node(value) &&
               made.session->selection().pointed() == node(value);
    };
    auto expected = initial;
    const auto view_matches =
        [&](const RunState &state, const std::vector<NodeId> &route)
    {
        return test::same_state(made.session->view().board(), state.sector) &&
               made.session->view().packet_route() == route;
    };
    tap(VK_TAB);
    tap(VK_SPACE);
    const auto wait_preview = project_action(expected, WaitCommand{});
    if (!view_matches(wait_preview.result.state, wait_preview.packet_route) ||
        wait_preview.packet_route.empty() ||
        !test::same_run(made.session->state(), expected))
        return 21;
    if (!hover(48))
        return 31;
    tap('R'); // out of range: restore live scene and erase the old route
    if (!view_matches(expected, {}))
        return 32;
    tap(VK_SPACE);
    SetWindowPos(hwnd, nullptr, 0, 0, 700, 500,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    UpdateWindow(hwnd);
    if (!view_matches(wait_preview.result.state, wait_preview.packet_route) ||
        made.session->error() != S_OK)
        return 33;
    tap(VK_ESCAPE);
    if (!view_matches(expected, {}))
        return 22;
    if (!hover(1))
        return 4;
    tap(VK_TAB);
    tap('R');
    const auto rotation_preview =
        project_action(expected, RotateCommand{node(1)});
    if (!view_matches(rotation_preview.result.state,
                      rotation_preview.packet_route) ||
        !rotation_preview.packet_route.empty())
        return 23;
    if (!made.session->selection().planning() ||
        made.session->selection().candidate() !=
            Command{RotateCommand{node(1)}} ||
        !test::same_run(made.session->state(), expected) || !hover(0))
        return 5;
    expected = resolve_run_turn(expected, RotateCommand{node(1)}).state;
    tap(VK_RETURN);
    tap(VK_RETURN);
    if (!view_matches(rotation_preview.result.state, {}))
        return 24;
    if (!test::same_run(made.session->state(), expected) ||
        made.session->selection().candidate())
        return 6;
    if (!hover(1))
        return 7;
    tap('O');
    tap(VK_ESCAPE);
    tap(VK_RETURN);
    if (!view_matches(expected, {}))
        return 25;
    if (made.session->selection().planning() ||
        !test::same_run(made.session->state(), expected))
        return 8;
    tap('O');
    expected = resolve_run_turn(expected, OverdriveCommand{node(1)}).state;
    if (!test::same_run(made.session->state(), expected))
        return 9;
    tap(VK_TAB);
    tap(VK_SPACE);
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    if (!view_matches(expected, {}))
        return 26;
    if (made.session->selection().planning() ||
        made.session->selection().candidate() ||
        made.session->selection().pointed() || window.window->pointed())
        return 10;
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
    tap('R');
    expected = resolve_run_turn(expected, RotateCommand{node(0)}).state;
    if (!test::same_run(made.session->state(), expected))
        return 11;
    if (!hover(1))
        return 12;
    SetWindowPos(hwnd, nullptr, 0, 0, 480, 640,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (window.window->pointed() || made.session->selection().pointed() ||
        !hover(1))
        return 13;
    SendMessageW(hwnd, WM_MOUSELEAVE, 0, 0);
    if (window.window->pointed() || made.session->selection().pointed())
        return 14;
    SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(-1, -1));
    if (window.window->pointed())
        return 15;
    if (!hover(1))
        return 16;
    SendMessageW(hwnd, WM_MOUSEMOVE, 0,
                 MAKELPARAM(0, 0)); // top margin after resize
    if (window.window->pointed())
        return 17;
    tap('O');
    expected = resolve_run_turn(expected, OverdriveCommand{node(0)}).state;
    if (!test::same_run(made.session->state(), expected) ||
        !test::same_state(made.session->view().board(), expected.sector))
        return 18;
    tap(VK_TAB);
    tap(VK_SPACE);
    tap(VK_TAB);
    tap(VK_RETURN);
    if (!test::same_run(made.session->state(), expected) ||
        made.session->selection().planning())
        return 19;
    // Refused candidates, terminal runs and resize must never resurrect a
    // previous route. Repaint a chosen preview through a real WM_PAINT.
    tap(VK_TAB);
    tap(VK_RIGHT);
    const auto move = project_action(expected, MoveCommand{node(1)});
    if (!view_matches(move.result.accepted ? move.result.state : expected,
                      move.result.accepted ? move.packet_route
                                           : std::vector<NodeId>{}))
        return 27;
    UpdateWindow(hwnd);
    tap(VK_ESCAPE);
    for (int i = 0; i < 50; ++i)
        tap(VK_SPACE);
    expected = made.session->state();
    if (run_phase(expected) != RunPhase::lost)
        return 28;
    tap(VK_TAB);
    tap(VK_SPACE);
    if (!view_matches(expected, {}))
        return 29;
    tap(VK_RETURN);
    if (!view_matches(expected, {}) ||
        !test::same_run(made.session->state(), expected))
        return 30;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    if (made.session->selection().planning() ||
        made.session->selection().candidate())
        return 20;
    made.session.reset();
    window.window.reset();
    CoUninitialize();
    return 0;
}
