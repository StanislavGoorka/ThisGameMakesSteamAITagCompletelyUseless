#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/profile_encode.hpp"
#include "developer_text.hpp"
#include "platform/windows/rotation_binding_file.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

class GatedBindingStore final : public RotationBindingStore
{
  public:
    explicit GatedBindingStore(const std::filesystem::path &path) : local(path)
    {
    }
    unsigned calls = 0;
    int mode = 0;
    windows::LocalRotationBindingStore local;
    bool save(RotationBinding binding) override
    {
        ++calls;
        if (mode == 2)
            throw std::runtime_error("Synthetic binding storage exception");
        return mode == 0 && local.save(binding);
    }
};
bool scenario(const std::filesystem::path &root, const TextCatalog &catalog,
              const std::filesystem::path &directory)
{
    const auto path = directory / L"rotation-key.bin";
    const auto profile = *encode_profile(LocalProfile{3, 2, 37, true});
    const auto checkpoint =
        *encode_checkpoint(*capture_checkpoint(start_run(RunSeed{1})));
    test::write_bytes(directory / L"profile.bin", profile);
    test::write_bytes(directory / L"checkpoint.bin", checkpoint);
    GatedBindingStore store{path};
    auto created =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!created.window)
        return false;
    auto &window = *created.window;
    const auto hwnd = window.handle();
    auto run = start_run(RunSeed{1});
    // Off-desktop numeric integration fixture allows observing an actual turn.
    run.sector.rotatable_nodes[run.sector.player_position.value()] = true;
    const AssetManifest assets{presentation::atlas_reference()};
    if (windows::RunWindow::create(window, run, root, assets, {}, nullptr,
                                   false, nullptr, nullptr, {}, &store)
            .session)
        return false;
    auto made =
        windows::RunWindow::create(window, run, root, assets, {}, &catalog,
                                   false, nullptr, nullptr, {}, &store);
    if (!made.session)
        return false;
    auto &session = *made.session;
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1000, 800,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    const auto tap = [hwnd](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    const auto matches = [&](std::string_view id)
    {
        const auto label = windows::read_text_utf16(catalog, id);
        return label.text &&
               session.view().label_text().find(*label.text) !=
                   std::wstring_view::npos &&
               !FAILED(session.error()) &&
               test::same_run(session.state(), run) &&
               test::same_state(session.view().board(), run.sector);
    };
    tap(VK_TAB);
    tap(VK_SPACE);
    if (!session.selection().candidate())
        return false;
    tap(VK_F2);
    if (session.selection().candidate() || !matches("rotation-binding-prompt"))
        return false;
    for (WPARAM key : {static_cast<WPARAM>('O'), static_cast<WPARAM>(VK_SPACE)})
    {
        tap(key);
        if (store.calls || !matches("rotation-binding-conflict") ||
            std::filesystem::exists(path))
            return false;
    }
    for (int mode : {1, 2})
    {
        store.mode = mode;
        const auto before = store.calls;
        SendMessageW(hwnd, WM_KEYDOWN, 'T', 0);
        for (int i = 0; i < 20; ++i)
            SendMessageW(hwnd, WM_KEYDOWN, 'T', i % 2 ? 1LL << 30 : 0);
        SendMessageW(hwnd, WM_KEYUP, 'T', 0);
        if (store.calls != before + 1 ||
            !matches("rotation-binding-unavailable") ||
            window.rotation_binding() != RotationBinding{} ||
            std::filesystem::exists(path))
            return false;
    }
    store.mode = 0;
    tap('T');
    const auto t = *RotationBinding::from_letter('T');
    if (!matches("rotation-binding-saved") || window.rotation_binding() != t ||
        windows::read_rotation_binding_file(path).binding != t)
        return false;
    const auto saved = test::read_bytes(path);
    tap('R');
    if (!test::same_run(session.state(), run))
        return false;
    const auto before = store.calls;
    tap(VK_F2);
    tap(VK_ESCAPE);
    tap('Z');
    if (!matches("rotation-binding-help") || store.calls != before)
        return false;
    tap(VK_F2);
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    tap('Z');
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    tap('Z');
    if (!matches("rotation-binding-help") || store.calls != before ||
        test::read_bytes(path) != saved)
        return false;
    const auto expected =
        resolve_run_turn(run, RotateCommand{run.sector.player_position});
    tap('T');
    if (!expected.accepted ||
        !test::same_run(session.state(), expected.state) ||
        test::read_bytes(path) != saved ||
        !std::ranges::equal(test::read_bytes(directory / L"profile.bin"),
                            profile) ||
        !std::ranges::equal(test::read_bytes(directory / L"checkpoint.bin"),
                            checkpoint))
        return false;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return true;
}
int main(int argc, char **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const auto catalog = TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    bool passed = false;
    if (catalog)
    {
        test::CheckpointDirectory scratch;
        passed = scenario(argv[1], *catalog, scratch.path);
    }
    CoUninitialize();
    return passed ? 0 : 2;
}
