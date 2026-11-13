#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/resume_window.hpp"
#include <iostream>
#include <map>
using namespace closed_circuit;
namespace
{
bool fail(std::string_view stage)
{
    std::cerr << stage << '\n';
    return false;
}
using Files = std::map<std::filesystem::path, std::vector<std::uint8_t>>;
Files snapshot(const std::filesystem::path &directory)
{
    Files files;
    for (const auto &entry : std::filesystem::directory_iterator(directory))
        files[entry.path().filename()] = test::read_bytes(entry.path());
    return files;
}
// Every displayed line carries the full marker: no codes, paths or IDs.
bool marked(std::wstring_view label)
{
    if (label.empty())
        return false;
    for (std::size_t start = 0; start <= label.size();)
    {
        const auto end = std::min(label.find(L'\n', start), label.size());
        if (!label.substr(start, end - start).starts_with(L"[AI PLACEHOLDER] "))
            return false;
        start = end + 1;
    }
    return true;
}
bool contains(std::wstring_view label, std::wstring_view text)
{
    return label.find(text) != std::wstring_view::npos;
}
const std::wstring_view damaged =
    L"[AI PLACEHOLDER] Saved run is damaged and was not loaded.";
const std::wstring_view unreadable =
    L"[AI PLACEHOLDER] Saved run could not be read.";
const std::wstring_view unsaved =
    L"[AI PLACEHOLDER] Run progress could not be saved.";

// One process lifetime of the developer window: read, create, close.
class Launch
{
  public:
    Launch(const TextCatalog &catalog, const std::filesystem::path &root,
           const std::filesystem::path &main)
        : store_(main), resume_(windows::resume_checkpoint_file(main))
    {
        window_ =
            windows::NativeWindow::create(catalog, "developer-window-title")
                .window;
        if (!window_)
            return;
        auto made =
            windows::create_resumed_window(*window_, resume_, RunSeed{0}, root,
                                           assets_, catalog, nullptr, &store_);
        session_ = std::move(made.session);
        hwnd_ = window_->handle();
        SetWindowPos(hwnd_, nullptr, -32000, -32000, 1000, 800,
                     SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(hwnd_);
        SendMessageW(hwnd_, WM_SETFOCUS, 0, 0);
    }
    Launch(const Launch &) = delete;
    Launch &operator=(const Launch &) = delete;
    ~Launch()
    {
        if (window_)
            SendMessageW(hwnd_, WM_CLOSE, 0, 0);
        session_.reset();
        window_.reset();
    }
    void tap(WPARAM key) const
    {
        SendMessageW(hwnd_, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd_, WM_KEYUP, key, 0);
    }
    [[nodiscard]] windows::RunWindow *get() const noexcept
    {
        return session_.get();
    }
    [[nodiscard]] ResumeStatus status() const noexcept
    {
        return resume_.status;
    }
    [[nodiscard]] std::wstring_view label() const noexcept
    {
        return session_->view().label_text();
    }

  private:
    const AssetManifest assets_{presentation::atlas_reference()};
    windows::LocalCheckpointStore store_;
    windows::CheckpointResume resume_;
    std::unique_ptr<windows::NativeWindow> window_;
    std::unique_ptr<windows::RunWindow> session_;
    HWND hwnd_ = nullptr;
};

bool close_and_restart(const TextCatalog &catalog,
                       const std::filesystem::path &root,
                       const std::filesystem::path &main)
{
    const auto fresh = start_run(RunSeed{0});
    {
        Launch first(catalog, root, main);
        auto *window = first.get();
        if (!window || first.status() != ResumeStatus::none ||
            window->screen() != SessionScreen::start ||
            std::filesystem::exists(main) || !marked(first.label()))
            return fail("first launch");
        first.tap(VK_RETURN);
        // Mid-sector progress is not saved; resume returns to the start.
        first.tap(VK_SPACE);
        first.tap(VK_SPACE);
        if (window->state().sector.turns_taken != TurnIndex{2} ||
            !std::filesystem::exists(main))
            return fail("first run");
    }
    const auto saved = snapshot(main.parent_path());
    for (int restart = 0; restart < 3; ++restart)
    {
        Launch next(catalog, root, main);
        auto *window = next.get();
        if (!window || next.status() != ResumeStatus::resumed ||
            window->screen() != SessionScreen::run ||
            !test::same_run(window->state(), fresh) ||
            !test::same_state(window->view().board(), fresh.sector) ||
            !marked(next.label()) || contains(next.label(), damaged) ||
            snapshot(main.parent_path()) != saved)
            return fail("resumed launch");
        const auto expected = resolve_run_turn(fresh, WaitCommand{});
        next.tap(VK_SPACE);
        if (!test::same_run(window->state(), expected.state) ||
            snapshot(main.parent_path()) != saved)
            return fail("resumed turn");
    }
    // A later sector with a paid debt pulse resumes exactly, pulse included.
    const auto &late = test::checkpoint_vectors[1];
    if (!windows::save_checkpoint_file(main, late.checkpoint))
        return fail("late fixture");
    const auto late_files = snapshot(main.parent_path());
    const auto restored = restore_checkpoint(late.checkpoint);
    for (int restart = 0; restart < 2; ++restart)
    {
        Launch next(catalog, root, main);
        auto *window = next.get();
        if (!window || !restored || next.status() != ResumeStatus::resumed ||
            !test::same_run(window->state(), *restored) ||
            window->state().sector.pulses.size() != 1 ||
            snapshot(main.parent_path()) != late_files)
            return fail("late resume");
    }
    return true;
}

bool bad_saves(const TextCatalog &catalog, const std::filesystem::path &root,
               const std::filesystem::path &main)
{
    auto backup = main;
    backup += L".bak";
    std::filesystem::remove(backup);
    auto bad = test::checkpoint_vectors[2].bytes;
    bad[21] ^= 0x40;
    test::write_bytes(main, bad);
    {
        Launch launch(catalog, root, main);
        auto *window = launch.get();
        if (!window || launch.status() != ResumeStatus::corrupt ||
            window->screen() != SessionScreen::start ||
            !contains(launch.label(), damaged) || !marked(launch.label()) ||
            test::read_bytes(main) != std::vector(bad.begin(), bad.end()))
            return fail("corrupt launch");
        // Starting replaces the read error; corrupt main still blocks saving.
        launch.tap(VK_RETURN);
        if (window->screen() != SessionScreen::run ||
            contains(launch.label(), damaged) ||
            !contains(launch.label(), unsaved) || !marked(launch.label()) ||
            test::read_bytes(main) != std::vector(bad.begin(), bad.end()))
            return fail("corrupt start");
    }
    test::write_bytes(main, test::checkpoint_vectors[0].bytes);
    {
        windows::FileHandle lock(CreateFileW(
            main.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!lock)
            return fail("lock");
        Launch launch(catalog, root, main);
        auto *window = launch.get();
        if (!window || launch.status() != ResumeStatus::unavailable ||
            window->screen() != SessionScreen::start ||
            !contains(launch.label(), unreadable) || !marked(launch.label()))
            return fail("unavailable launch");
    }
    // The lock is gone, so the same valid file resumes again.
    Launch launch(catalog, root, main);
    return launch.get() && launch.status() == ResumeStatus::resumed &&
                   !contains(launch.label(), unreadable)
               ? true
               : fail("unlocked resume");
}

bool missing_line(const std::filesystem::path &root)
{
    std::vector<TextRecord> records;
    for (const auto &record : windows::developer_text_records)
        if (record.id != "checkpoint-corrupt")
            records.push_back(record);
    const auto catalog =
        *TextCatalog::from_records(records, {}, TextAudience::developer);
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    const AssetManifest assets{presentation::atlas_reference()};
    if (!window.window)
        return fail("missing line window");
    const auto refused = windows::create_resumed_window(
        *window.window, {std::nullopt, ResumeStatus::corrupt}, RunSeed{0}, root,
        assets, catalog, nullptr, nullptr);
    const auto allowed = windows::create_resumed_window(
        *window.window, {std::nullopt, ResumeStatus::unavailable}, RunSeed{0},
        root, assets, catalog, nullptr, nullptr);
    return !refused.session &&
                   refused.error == HRESULT_FROM_WIN32(ERROR_NOT_FOUND) &&
                   allowed.session &&
                   allowed.session->show_resume_status(
                       static_cast<ResumeStatus>(9)) == E_INVALIDARG
               ? true
               : fail("missing line");
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
        const auto main = owned.path / L"checkpoint ż.bin";
        passed = close_and_restart(catalog, argv[1], main) &&
                 bad_saves(catalog, argv[1], main) && missing_line(argv[1]);
    }
    catch (...)
    {
        passed = false;
    }
    CoUninitialize();
    return passed ? 0 : 2;
}
