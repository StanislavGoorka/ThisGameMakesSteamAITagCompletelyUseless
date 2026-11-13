#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "application/checkpoint_encode.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/run_window.hpp"
#include <algorithm>
#include <iostream>
using namespace closed_circuit;
namespace
{
class GatedStore final : public CheckpointStore
{
  public:
    explicit GatedStore(const std::filesystem::path &path) : durable(path)
    {
    }
    bool save(const RunCheckpoint &checkpoint) override
    {
        ++saves;
        return gate() && durable.save(checkpoint);
    }
    bool clear() override
    {
        ++clears;
        return gate() && durable.clear();
    }
    windows::LocalCheckpointStore durable;
    unsigned saves = 0, clears = 0;
    bool failing = false, throwing = false;

  private:
    bool gate()
    {
        if (throwing)
            throw std::runtime_error("Synthetic checkpoint exception");
        return !failing;
    }
};
bool fail(std::string_view stage)
{
    std::cerr << stage << '\n';
    return false;
}
const std::wstring_view unsaved =
    L"[AI PLACEHOLDER] Run progress could not be saved.";

struct Fixture
{
    std::filesystem::path main, backup;
    [[nodiscard]] bool holds(const RunState &run) const
    {
        const auto checkpoint = capture_checkpoint(run);
        const auto bytes =
            checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
        return bytes && std::filesystem::exists(main) &&
               std::ranges::equal(test::read_bytes(main), *bytes);
    }
    [[nodiscard]] bool empty() const
    {
        return !std::filesystem::exists(main) &&
               !std::filesystem::exists(backup);
    }
};

class Session
{
  public:
    Session(const TextCatalog &catalog, const std::filesystem::path &root,
            const RunState &run, bool show_start, CheckpointStore *store)
    {
        window_ =
            windows::NativeWindow::create(catalog, "developer-window-title")
                .window;
        if (!window_)
            return;
        made_ = windows::RunWindow::create(*window_, run, root, assets_, {},
                                           &catalog, show_start, nullptr, store)
                    .session;
        hwnd_ = window_->handle();
        SetWindowPos(hwnd_, nullptr, -32000, -32000, 1000, 800,
                     SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(hwnd_);
        SendMessageW(hwnd_, WM_SETFOCUS, 0, 0);
    }
    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    ~Session()
    {
        if (window_)
            SendMessageW(hwnd_, WM_CLOSE, 0, 0);
        made_.reset();
        window_.reset();
    }
    void tap(WPARAM key) const
    {
        SendMessageW(hwnd_, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd_, WM_KEYUP, key, 0);
    }
    void resize(SIZE size) const
    {
        SetWindowPos(hwnd_, nullptr, -32000, -32000, size.cx, size.cy,
                     SWP_NOACTIVATE | SWP_NOZORDER);
        UpdateWindow(hwnd_);
    }
    [[nodiscard]] windows::RunWindow *get() const noexcept
    {
        return made_.get();
    }
    [[nodiscard]] bool shows_unsaved() const
    {
        return made_->view().label_text().find(unsaved) !=
               std::wstring_view::npos;
    }
    // Waits until the run ends; every accepted turn matches the core.
    [[nodiscard]] bool finish(RunState &run) const
    {
        while (run_phase(run) == RunPhase::playing)
        {
            const auto expected = resolve_run_turn(run, WaitCommand{});
            tap(VK_SPACE);
            if (!expected.accepted ||
                !test::same_run(made_->state(), expected.state))
                return false;
            run = expected.state;
        }
        return made_->screen() == SessionScreen::result;
    }

  private:
    const AssetManifest assets_{presentation::atlas_reference()};
    std::unique_ptr<windows::NativeWindow> window_;
    std::unique_ptr<windows::RunWindow> made_;
    HWND hwnd_ = nullptr;
};

bool lifecycle(const TextCatalog &catalog, const std::filesystem::path &root,
               const Fixture &files)
{
    GatedStore store{files.main};
    auto run = start_run(RunSeed{1});
    Session session(catalog, root, run, true, &store);
    auto *window = session.get();
    if (!window || store.saves || store.clears || !files.empty())
        return fail("creation writes");
    session.tap(VK_RETURN);
    if (!files.holds(run) || std::filesystem::exists(files.backup) ||
        store.saves != 1 || window->checkpoint_failed() ||
        session.shows_unsaved())
        return fail("start boundary");
    const auto saved = test::read_bytes(files.main);
    for (int i = 0; i < 3; ++i)
    {
        session.tap(VK_TAB);
        session.tap(VK_SPACE);
        session.tap(VK_ESCAPE);
    }
    for (int i = 0; i < 2; ++i)
    {
        const auto expected = resolve_run_turn(run, WaitCommand{});
        session.tap(VK_SPACE);
        if (!expected.accepted ||
            !test::same_run(window->state(), expected.state))
            return fail("turn");
        run = expected.state;
    }
    if (store.saves != 1 || store.clears ||
        test::read_bytes(files.main) != saved)
        return fail("turn or preview wrote");
    if (!session.finish(run) || store.clears != 1 || !files.empty() ||
        window->checkpoint_failed())
        return fail("terminal clear");
    session.tap(VK_RETURN);
    run = start_run(run.seed);
    if (!test::same_run(window->state(), run) || !files.holds(run) ||
        store.saves != 2)
        return fail("restart boundary");
    // A failed clear keeps the result screen usable and the line visible.
    store.failing = true;
    if (!session.finish(run) || store.clears != 2 ||
        !files.holds(start_run(run.seed)) || !window->checkpoint_failed() ||
        !session.shows_unsaved() || FAILED(window->error()))
        return fail("failed clear");
    for (auto size : {SIZE{900, 600}, SIZE{600, 800}, SIZE{1000, 800}})
    {
        session.resize(size);
        if (!session.shows_unsaved() || FAILED(window->error()))
            return fail("resize unsaved line");
    }
    store.failing = false;
    store.throwing = true;
    session.tap(VK_RETURN);
    run = start_run(run.seed);
    if (!test::same_run(window->state(), run) || store.saves != 3 ||
        !window->checkpoint_failed() || !session.shows_unsaved() ||
        FAILED(window->error()) || !window->view().showing_board())
        return fail("throwing save");
    store.throwing = false;
    if (!session.finish(run) || store.clears != 3 || !files.empty() ||
        window->checkpoint_failed() || session.shows_unsaved())
        return fail("recovered clear");
    return true;
}

bool transition(const TextCatalog &catalog, const std::filesystem::path &root,
                const Fixture &files, bool failing)
{
    GatedStore store{files.main};
    store.failing = failing;
    auto run = start_run(RunSeed{1});
    run.sector.powered_nodes = run.sector.receiver_nodes;
    run.sector.player_position = *run.sector.sector_exit;
    Session session(catalog, root, run, false, &store);
    auto *window = session.get();
    if (!window || store.saves || store.clears)
        return fail("reward creation writes");
    const bool existed = std::filesystem::exists(files.main);
    const auto before =
        existed ? test::read_bytes(files.main) : std::vector<std::uint8_t>{};
    session.tap(VK_RIGHT);
    if (store.saves || store.clears)
        return fail("reward selection wrote");
    const auto chosen = window->reward_candidate();
    if (!chosen || !choose_run_module(run, *chosen))
        return fail("reward candidate");
    session.tap(VK_RETURN);
    const auto next = advance_run(run);
    if (!next.accepted || !test::same_run(window->state(), next.state) ||
        store.saves != 1 || store.clears)
        return fail("transition");
    if (failing)
    {
        const bool unchanged = existed ? test::read_bytes(files.main) == before
                                       : !std::filesystem::exists(files.main);
        if (!window->checkpoint_failed() || !session.shows_unsaved() ||
            FAILED(window->error()) || !window->view().showing_board() ||
            !unchanged)
            return fail("failed transition");
        return true;
    }
    if (!files.holds(next.state) || window->checkpoint_failed() ||
        session.shows_unsaved())
        return fail("transition bytes");
    return true;
}
} // namespace

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    bool passed = false;
    try
    {
        const auto catalog = *TextCatalog::from_records(
            windows::developer_text_records, {}, TextAudience::developer);
        test::CheckpointDirectory owned;
        const Fixture files{owned.path / L"checkpoint.bin",
                            owned.path / L"checkpoint.bin.bak"};
        // A store requires the catalog that holds its marked failure line.
        GatedStore unused{files.main};
        auto window =
            windows::NativeWindow::create(catalog, "developer-window-title");
        const AssetManifest assets{presentation::atlas_reference()};
        passed = window.window &&
                 windows::RunWindow::create(
                     *window.window, start_run(RunSeed{1}), argv[1], assets, {},
                     nullptr, false, nullptr, &unused)
                         .error == E_INVALIDARG &&
                 !unused.saves;
        window.window.reset();
        passed = passed && lifecycle(catalog, argv[1], files) &&
                 transition(catalog, argv[1], files, true) &&
                 transition(catalog, argv[1], files, false) &&
                 transition(catalog, argv[1], files, true);
    }
    catch (...)
    {
        passed = false;
    }
    CoUninitialize();
    return passed ? 0 : 2;
}
