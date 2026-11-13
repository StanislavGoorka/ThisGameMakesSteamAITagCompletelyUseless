#include "../support/checkpoint_files.hpp"
#include "../support/state_checks.hpp"
#include "core/run_start.hpp"
#include "core/turn_resolver.hpp"
#include "platform/windows/board_view.hpp"
#include <iostream>
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const TextRecord record{"title", "[AI PLACEHOLDER] Board test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    const auto run = start_run(RunSeed{0});
    const auto original = run;
    const AssetManifest manifest{presentation::atlas_reference()};
    auto window = windows::NativeWindow::create(catalog, "title");
    if (!window.window)
        return 2;
    test::CheckpointDirectory directory;
    if (windows::BoardView::create(*window.window, run.sector, directory.path,
                                   manifest)
            .view)
        return 3;
    auto created = windows::BoardView::create(*window.window, run.sector,
                                              argv[1], manifest);
    if (!created.view)
        return 4;
    const auto hwnd = window.window->handle();
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        // A hidden HWND does not receive UpdateWindow paints. Show without
        // activation outside the desktop to exercise the real paint route.
        if (!SetWindowPos(hwnd, nullptr, -32000, -32000, 340 + repeat * 90,
                          290 + repeat * 40,
                          SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW))
            return 5;
        InvalidateRect(hwnd, nullptr, FALSE);
        const auto before = created.view->frames();
        UpdateWindow(hwnd);
        if (FAILED(created.view->error()) || created.view->frames() <= before)
        {
            std::cerr << "Paint error/frames/before " << created.view->error()
                      << '/' << created.view->frames() << '/' << before << '\n';
            return 6;
        }
        created.view->handle_draw_result(D2DERR_RECREATE_TARGET);
        if (created.view->error() != D2DERR_RECREATE_TARGET)
            return 7;
        InvalidateRect(hwnd, nullptr, FALSE);
        UpdateWindow(hwnd);
        if (created.view->error() != S_OK ||
            created.view->frames() <= before + 1)
            return 8;
    }
    if (!test::same_state(run.sector, original.sector))
        return 9;
    const auto next = resolve_turn(run.sector, WaitCommand{});
    const auto old_frames = created.view->frames();
    created.view->set_board(next.state);
    UpdateWindow(hwnd);
    if (!next.accepted ||
        !test::same_state(created.view->board(), next.state) ||
        created.view->error() != S_OK || created.view->frames() <= old_frames)
        return 11;
    const auto before = created.view->frames();
    created.view.reset();
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd); // Detached callback must not be used.
    if (!IsWindow(hwnd) || before < 6)
        return 10;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return 0;
}
