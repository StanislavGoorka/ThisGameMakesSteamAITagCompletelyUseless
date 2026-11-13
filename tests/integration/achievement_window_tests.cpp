#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include "platform/windows/run_window.hpp"
#include <iostream>
using namespace closed_circuit;

class GatedSink final : public AchievementSink
{
  public:
    explicit GatedSink(const std::filesystem::path &path) : durable(path)
    {
    }
    AchievementSinkResult unlock(AchievementId id) override
    {
        ++calls;
        if (throwing)
            throw std::runtime_error("Synthetic storage exception");
        return failing ? AchievementSinkResult::unavailable
                       : durable.unlock(id);
    }
    windows::LocalAchievementSink durable;
    unsigned calls = 0;
    bool failing = true, throwing = false;
};
bool fail(std::string_view stage)
{
    std::cerr << stage << '\n';
    return false;
}

bool scenario(const std::filesystem::path &root, const TextCatalog &catalog,
              const std::filesystem::path &path, bool initially_owned)
{
    LocalProfile profile{static_cast<std::uint16_t>(initially_owned ? 1 : 0), 2,
                         37, true};
    if (!windows::save_profile_file(path, profile) ||
        !windows::save_profile_file(path, profile))
        return fail("profile fixture");
    auto backup = path;
    backup += L".bak";
    const auto original = test::read_bytes(path),
               original_backup = test::read_bytes(backup);
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return fail("window");
    auto run = start_run(RunSeed{1});
    run.sector.receiver_nodes[48] = true;
    run.sector.packet =
        Packet{*NodeId::from_value(48), *NodeId::from_value(48)};
    GatedSink sink{path};
    AchievementNoticeSink notices{sink};
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(*window.window, run, root, assets,
                                           {}, &catalog, false, &notices);
    if (!made.session)
        return fail("session");
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
    for (int i = 0; i < 3; ++i)
    {
        tap(VK_TAB);
        tap(VK_SPACE);
        if (!test::same_run(session.state(), run) || sink.calls ||
            notices.pending() || test::read_bytes(path) != original ||
            test::read_bytes(backup) != original_backup)
            return fail("preview services");
        tap(VK_ESCAPE);
    }
    for (int i = 0; i < 2; ++i)
    {
        const auto expected = resolve_run_turn(run, WaitCommand{});
        tap(VK_SPACE);
        if (!expected.accepted ||
            !test::same_run(session.state(), expected.state) ||
            !test::same_state(session.view().board(), expected.state.sector) ||
            !session.achievements_pending() || notices.pending() ||
            session.view().label_text().find(
                L"[AI PLACEHOLDER] Achievement storage unavailable.") ==
                std::wstring_view::npos ||
            !session.view().showing_board() ||
            test::read_bytes(path) != original ||
            test::read_bytes(backup) != original_backup)
            return fail("failed commit queue");
        run = expected.state;
    }
    sink.throwing = true;
    tap(VK_RETURN);
    if (!session.achievements_pending() ||
        !test::same_run(session.state(), run) || notices.pending())
        return fail("throw retry");
    sink.throwing = false;
    sink.failing = false;
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    const auto before_focus = sink.calls;
    tap(VK_RETURN);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
    if (sink.calls != before_focus || !session.achievements_pending())
        return fail("focus retry filter");
    tap(VK_RETURN);
    if (session.achievements_pending() ||
        !test::same_run(session.state(), run) ||
        static_cast<bool>(notices.pending()) == initially_owned ||
        FAILED(session.error()))
        return fail("retry completion");
    profile.achievement_bits = 1;
    if (windows::read_profile_file(path).profile != profile)
        return fail("persisted profile");
    const auto main_bytes = test::read_bytes(path),
               backup_bytes = test::read_bytes(backup);
    if (initially_owned &&
        (main_bytes != original || backup_bytes != original_backup))
        return fail("existing profile changed");
    const auto calls = sink.calls;
    const auto label = std::wstring{session.view().label_text()};
    const std::wstring message = L"[AI PLACEHOLDER] Achievement unlocked: "
                                 L"First delivery. Escape dismisses.";
    if ((label.find(message) != std::wstring::npos) == initially_owned ||
        (!initially_owned &&
         label.find(message, label.find(message) + 1) != std::wstring::npos))
        return fail("notice row");
    for (auto size : {SIZE{100, 60}, SIZE{900, 600}, SIZE{600, 800}})
    {
        SetWindowPos(hwnd, nullptr, -32000, -32000, size.cx, size.cy,
                     SWP_NOACTIVATE | SWP_NOZORDER);
        UpdateWindow(hwnd);
        if (session.view().label_text() != label ||
            FAILED(session.view().error()) || !session.view().showing_board())
            return fail("resize notice");
    }
    SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, 0);
    for (int i = 0; i < 20; ++i)
        SendMessageW(hwnd, WM_KEYDOWN, VK_RETURN, i % 2 ? 1LL << 30 : 0);
    SendMessageW(hwnd, WM_KEYUP, VK_RETURN, 0);
    tap(VK_TAB);
    tap(VK_SPACE);
    tap(VK_ESCAPE);
    if (notices.pending() || sink.calls != calls ||
        !test::same_run(session.state(), run) ||
        test::read_bytes(path) != main_bytes ||
        test::read_bytes(backup) != backup_bytes)
        return fail("dismiss/preview duplicate");
    while (run_phase(run) == RunPhase::playing)
    {
        const auto expected = resolve_run_turn(run, WaitCommand{});
        tap(VK_SPACE);
        if (!expected.accepted ||
            !test::same_run(session.state(), expected.state) ||
            notices.pending())
            return fail("remaining turns");
        run = expected.state;
    }
    if (session.screen() != SessionScreen::result ||
        session.achievements_pending())
        return fail("terminal stream");
    tap(VK_RETURN);
    if (!test::same_run(session.state(), start_run(run.seed)) ||
        notices.pending() || session.achievements_pending())
        return fail("restart scope");
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    made.session.reset();
    window.window.reset();
    // New window and sink owners read the durable profile again.
    window = windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return fail("second window");
    auto restored = start_run(RunSeed{1});
    restored.sector.receiver_nodes[48] = true;
    restored.sector.packet =
        Packet{*NodeId::from_value(48), *NodeId::from_value(48)};
    windows::LocalAchievementSink durable{path};
    AchievementNoticeSink restarted{durable};
    made = windows::RunWindow::create(*window.window, restored, root, assets,
                                      {}, &catalog, false, &restarted);
    if (!made.session)
        return fail("second session");
    SendMessageW(window.window->handle(), WM_SETFOCUS, 0, 0);
    SendMessageW(window.window->handle(), WM_KEYDOWN, VK_SPACE, 0);
    SendMessageW(window.window->handle(), WM_KEYUP, VK_SPACE, 0);
    if (restarted.pending() || made.session->achievements_pending() ||
        !test::same_run(made.session->state(),
                        resolve_run_turn(restored, WaitCommand{}).state) ||
        test::read_bytes(path) != main_bytes ||
        test::read_bytes(backup) != backup_bytes)
        return fail("durable duplicate");
    SendMessageW(window.window->handle(), WM_CLOSE, 0, 0);
    made.session.reset();
    window.window.reset();
    return true;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    test::CheckpointDirectory owned;
    const bool passed =
        scenario(argv[1], catalog, owned.path / L"new.bin", false) &&
        scenario(argv[1], catalog, owned.path / L"owned.bin", true);
    CoUninitialize();
    return passed ? 0 : 2;
}
