#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    const AssetManifest assets{presentation::atlas_reference()};
    for (unsigned mask = 0; mask < 3; ++mask)
    {
        auto window =
            windows::NativeWindow::create(catalog, "developer-window-title");
        if (!window.window)
            return 2;
        auto run = start_run(RunSeed{0});
        run.sector.powered_nodes = run.sector.receiver_nodes;
        run.sector.player_position = *run.sector.sector_exit;
        if (mask & 1)
            static_cast<void>(run.sector.modules.add(ModuleId::rotation_range));
        if (mask & 2)
            static_cast<void>(
                run.sector.modules.add(ModuleId::terminal_rotation));
        auto session = windows::RunWindow::create(*window.window, run, argv[1],
                                                  assets, {}, &catalog);
        if (!session.session)
            return 3;
        auto &view = const_cast<windows::BoardView &>(session.session->view());
        SendMessageW(window.window->handle(), WM_SETFOCUS, 0, 0);
        SendMessageW(window.window->handle(), WM_KEYDOWN, VK_RIGHT, 0);
        SendMessageW(window.window->handle(), WM_KEYUP, VK_RIGHT, 0);
        if (view.showing_board() ||
            !test::same_run(session.session->state(), run))
            return 11;
        RewardSelection selection;
        selection.move(run, false);
        const auto lines = *presentation::reward_lines(
            reward_offer(run), selection.candidate(run));
        view.set_board(run.sector, {}, std::nullopt, lines);
        std::wstring expected = L"[AI PLACEHOLDER] Closed Circuit";
        for (const auto line : lines)
        {
            const auto formatted =
                windows::decode_text_utf16(*format_text_line(catalog, line));
            if (!formatted.text)
                return 4;
            expected += L"\n" + *formatted.text;
        }
        const auto hwnd = window.window->handle();
        for (const auto size :
             {SIZE{900, 600}, SIZE{100, 60}, SIZE{600, 800}, SIZE{900, 600}})
        {
            SetWindowPos(hwnd, nullptr, -32000, -32000, size.cx, size.cy,
                         SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
            UpdateWindow(hwnd);
            if (FAILED(view.error()) || view.showing_board() ||
                view.label_text() != expected ||
                !test::same_run(session.session->state(), run) ||
                !test::same_state(view.board(), run.sector))
                return 5;
            for (int i = 0; i < 49; ++i)
                if (view.node_at((i % 7 + .5f) * size.cx / 7,
                                 (i / 7 + .5f) * size.cy / 7,
                                 static_cast<float>(size.cx),
                                 static_cast<float>(size.cy)))
                    return 6;
        }
        // A failed replacement must preserve the entire previous overlay.
        try
        {
            const std::array bad{TextLine{"missing-reward", std::nullopt}};
            view.set_board(start_run(RunSeed{1}).sector, {}, std::nullopt, bad);
            return 7;
        }
        catch (const std::runtime_error &)
        {
        }
        if (view.label_text() != expected ||
            !test::same_state(view.board(), run.sector) || view.showing_board())
            return 8;
        view.handle_draw_result(D2DERR_RECREATE_TARGET);
        UpdateWindow(hwnd);
        if (FAILED(view.error()) || view.label_text() != expected)
            return 9;
        // DirectWrite measures the complete block at each target DPI; a tiny
        // target omits it wholesale. Existing pixel tests cover this renderer.
        const auto text = windows::TextRenderer::create(catalog, lines);
        if (!text.renderer ||
            text.renderer->text().find(L"[AI PLACEHOLDER] Selected option:") ==
                std::wstring_view::npos)
            return 10;
        SendMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    CoUninitialize();
    return 0;
}
