#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const TextRecord record{"title", "[AI PLACEHOLDER] Text view",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto window = windows::NativeWindow::create(catalog, "title");
    auto label = windows::TextRenderer::create(catalog, "title");
    if (!window.window || !label.renderer)
        return 2;
    const auto expected_text = std::wstring{label.renderer->text()};
    auto run = start_run(RunSeed{0});
    const AssetManifest manifest{presentation::atlas_reference()};
    auto created = windows::RunWindow::create(
        *window.window, run, argv[1], manifest, std::move(label.renderer));
    if (!created.session)
        return 3;
    const auto hwnd = window.window->handle();
    auto &view = const_cast<windows::BoardView &>(created.session->view());
    const auto tap = [&](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    for (const auto dimensions :
         {SIZE{800, 600}, SIZE{600, 800}, SIZE{80, 100}, SIZE{800, 600}})
    {
        SetWindowPos(hwnd, nullptr, -32000, -32000, dimensions.cx,
                     dimensions.cy,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        UpdateWindow(hwnd);
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        if (FAILED(view.error()) || view.label_text() != expected_text ||
            view.board_top() <= 0)
            return 4;
        RECT client{};
        GetClientRect(hwnd, &client);
        // Logical pixels at this test process's default DPI.
        if (view.node_at(4, 4, static_cast<float>(client.right),
                         static_cast<float>(client.bottom)))
            return 5;
        SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(4, 4));
        if (window.window->pointed())
            return 6;
        const auto layout = presentation::fit_board(
            static_cast<float>(client.right),
            static_cast<float>(client.bottom) - view.board_top());
        if (!layout)
            continue;
        for (int value = 0; value < 49; ++value)
        {
            const int x = static_cast<int>(layout->left +
                                           (value % 7 + .5f) * layout->cell);
            const int y = static_cast<int>(view.board_top() + layout->top +
                                           (value / 7 + .5f) * layout->cell);
            SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
            if (window.window->pointed() != NodeId::from_value(value))
                return 7;
        }
        tap(VK_TAB);
        tap(VK_SPACE);
        const auto preview = project_action(run, WaitCommand{});
        if (!test::same_run(created.session->state(), run) ||
            !test::same_state(view.board(), preview.result.state.sector) ||
            view.packet_route() != preview.packet_route)
            return 8;
        UpdateWindow(hwnd);
        const auto frames = view.frames();
        // Inject only the documented loss status; no physical GPU reset.
        view.handle_draw_result(D2DERR_RECREATE_TARGET);
        UpdateWindow(hwnd);
        if (view.error() != S_OK || view.frames() <= frames ||
            view.label_text() != expected_text ||
            view.packet_route() != preview.packet_route)
            return 9;
        tap(VK_ESCAPE);
        if (!test::same_state(view.board(), run.sector) ||
            !view.packet_route().empty())
            return 10;
        tap(VK_TAB);
        tap(VK_SPACE);
        SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
        if (!test::same_state(view.board(), run.sector) ||
            !view.packet_route().empty() ||
            created.session->selection().planning())
            return 11;
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        tap(VK_SPACE);
        run = resolve_run_turn(run, WaitCommand{}).state;
        if (!test::same_run(created.session->state(), run) ||
            view.label_text() != expected_text)
            return 12;
    }
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    created.session.reset();
    window.window.reset();
    CoUninitialize();
    return 0;
}
