#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include "developer_text.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

bool matches(const windows::BoardView &view, const GameState &state)
{
    const auto v = presentation::hud_values(state);
    const auto expected =
        L"[AI PLACEHOLDER] Closed Circuit\n[AI PLACEHOLDER] HP: " +
        std::to_wstring(v.integrity) + L"\n[AI PLACEHOLDER] Turns: " +
        std::to_wstring(static_cast<std::uint32_t>(v.turns)) +
        L"\n[AI PLACEHOLDER] Deliveries: " + std::to_wstring(v.deliveries);
    return view.label_text() == expected &&
           test::same_state(view.board(), state);
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return 2;
    auto run = start_run(RunSeed{0});
    run.sector.packet =
        Packet{*NodeId::from_value(48), *NodeId::from_value(48)};
    run.sector.receiver_nodes[48] = true;
    run.sector.pulses.push_back(
        {*EntityId::from_value(1), run.sector.player_position});
    const AssetManifest manifest{presentation::atlas_reference()};
    auto session = windows::RunWindow::create(*window.window, run, argv[1],
                                              manifest, {}, &catalog);
    if (!session.session)
        return 3;
    const auto hwnd = window.window->handle();
    auto &view = const_cast<windows::BoardView &>(session.session->view());
    auto tap = [&](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    for (auto dimensions :
         {SIZE{800, 600}, SIZE{600, 800}, SIZE{80, 100}, SIZE{800, 600}})
    {
        SetWindowPos(hwnd, nullptr, -32000, -32000, dimensions.cx,
                     dimensions.cy,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        UpdateWindow(hwnd);
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        if (!matches(view, run.sector) || FAILED(view.error()))
            return 4;
        RECT client{};
        GetClientRect(hwnd, &client);
        if (view.node_at(4, 4, static_cast<float>(client.right),
                         static_cast<float>(client.bottom)))
            return 5;
        const auto layout = presentation::fit_board(
            static_cast<float>(client.right),
            static_cast<float>(client.bottom) - view.board_top());
        if (layout)
            for (int i = 0; i < 49; ++i)
            {
                auto at =
                    view.node_at(layout->left + (i % 7 + .5f) * layout->cell,
                                 view.board_top() + layout->top +
                                     (i / 7 + .5f) * layout->cell,
                                 static_cast<float>(client.right),
                                 static_cast<float>(client.bottom));
                if (at != NodeId::from_value(i))
                    return 6;
            }
        tap(VK_TAB);
        tap(VK_SPACE);
        auto next = resolve_run_turn(run, WaitCommand{});
        if (!matches(view, next.state.sector) ||
            !test::same_run(session.session->state(), run))
            return 7;
        tap(VK_ESCAPE);
        if (!matches(view, run.sector))
            return 8;
        tap(VK_TAB);
        tap(VK_SPACE);
        SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
        if (!matches(view, run.sector))
            return 9;
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        tap(VK_TAB);
        tap(VK_SPACE);
        tap(VK_RETURN);
        run = std::move(next.state);
        if (!matches(view, run.sector) ||
            !test::same_run(session.session->state(), run))
            return 10;
        const auto label = std::wstring{view.label_text()};
        view.handle_draw_result(D2DERR_RECREATE_TARGET);
        UpdateWindow(hwnd);
        if (FAILED(view.error()) || view.label_text() != label)
            return 11;
        tap(VK_ESCAPE); // Each resize scenario starts outside planning mode.
    }
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    session.session.reset();
    window.window.reset();
    CoUninitialize();
    return 0;
}
