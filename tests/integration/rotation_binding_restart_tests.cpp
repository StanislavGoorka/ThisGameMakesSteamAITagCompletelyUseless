#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/profile_encode.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/rotation_binding_startup.hpp"
#include "platform/windows/run_window.hpp"
#include <iostream>
using namespace closed_circuit;

bool child(const std::filesystem::path &root,
           const std::filesystem::path &directory, std::wstring_view mode)
{
    const auto startup =
        windows::rotation_binding_startup({directory / L"profile.bin", S_OK});
    const auto t = *RotationBinding::from_letter('T');
    const bool writing = mode == L"--write";
    if (FAILED(startup.error) ||
        startup.binding != (writing ? RotationBinding{} : t))
        return false;
    const auto catalog = TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    if (!catalog)
        return false;
    auto native =
        windows::NativeWindow::create(*catalog, "developer-window-title");
    if (!native.window)
        return false;
    auto &window = *native.window;
    window.set_rotation_binding(startup.binding);
    windows::LocalRotationBindingStore bindings{startup.path};
    windows::LocalAchievementSink durable{directory / L"profile.bin"};
    AchievementNoticeSink notices{durable};
    windows::LocalCheckpointStore checkpoints{directory / L"checkpoint.bin"};
    auto run = start_run(RunSeed{1});
    // Numeric off-desktop fixture, keeping the normal resolver and services.
    run.sector.rotatable_nodes[run.sector.player_position.value()] = true;
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(window, run, root, assets, {},
                                           &*catalog, false, &notices,
                                           &checkpoints, {}, &bindings);
    if (!made.session)
        return false;
    const auto hwnd = window.handle();
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1000, 800,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    const auto tap = [hwnd](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    if (writing)
    {
        tap(VK_F2);
        tap('T');
        if (window.rotation_binding() != t ||
            !test::same_run(made.session->state(), run))
            return false;
    }
    else if (mode == L"--deny")
    {
        const auto saved = test::read_bytes(startup.path);
        {
            // The reader can validate main, but rename is denied by this
            // handle.
            windows::FileHandle lock(
                CreateFileW(startup.path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                            nullptr, OPEN_EXISTING, 0, nullptr));
            if (!lock)
                return false;
            tap(VK_F2);
            tap('Y');
            const auto line = windows::read_text_utf16(
                *catalog, "rotation-binding-unavailable");
            if (!line.text ||
                made.session->view().label_text().find(*line.text) ==
                    std::wstring_view::npos ||
                window.rotation_binding() != t ||
                !test::same_run(made.session->state(), run) ||
                test::read_bytes(startup.path) != saved)
                return false;
        }
        SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        tap('Z');
        if (test::read_bytes(startup.path) != saved ||
            !test::same_run(made.session->state(), run))
            return false;
    }
    else if (mode == L"--read")
    {
        const auto saved = test::read_bytes(startup.path);
        tap('R');
        if (!test::same_run(made.session->state(), run))
            return false;
        tap(VK_F2);
        tap('O');
        SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
        SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
        if (window.rotation_binding() != t ||
            !test::same_run(made.session->state(), run) ||
            test::read_bytes(startup.path) != saved)
            return false;
        const auto expected =
            resolve_run_turn(run, RotateCommand{run.sector.player_position});
        SendMessageW(hwnd, WM_KEYDOWN, 'T', 0);
        for (int i = 0; i < 20; ++i)
            SendMessageW(hwnd, WM_KEYDOWN, 'T', i % 2 ? 1LL << 30 : 0);
        SendMessageW(hwnd, WM_KEYUP, 'T', 0);
        if (!expected.accepted ||
            !test::same_run(made.session->state(), expected.state) ||
            !test::same_state(made.session->view().board(),
                              expected.state.sector) ||
            test::read_bytes(startup.path) != saved)
            return false;
    }
    else
        return false;
    if (FAILED(made.session->error()) || made.session->checkpoint_failed() ||
        windows::read_rotation_binding_file(startup.path).binding != t)
        return false;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return true;
}

bool launch(const std::filesystem::path &executable,
            const std::filesystem::path &root,
            const std::filesystem::path &directory, std::wstring_view mode)
{
    std::wstring command = L"\"" + executable.wstring() + L"\" " +
                           std::wstring{mode} + L" \"" + root.wstring() +
                           L"\" \"" + directory.wstring() + L"\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &process))
        return false;
    windows::FileHandle process_owner{process.hProcess},
        thread_owner{process.hThread};
    if (WaitForSingleObject(process.hProcess, 20000) != WAIT_OBJECT_0)
    {
        TerminateProcess(process.hProcess, 99);
        WaitForSingleObject(process.hProcess, 5000);
        return false;
    }
    DWORD code = 1;
    return GetExitCodeProcess(process.hProcess, &code) && code == 0;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc == 4)
    {
        if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
            return 1;
        const bool passed = child(argv[2], argv[3], argv[1]);
        CoUninitialize();
        return passed ? 0 : 2;
    }
    if (argc != 2)
        return 3;
    std::wstring executable(32768, L'\0');
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (!length || length >= executable.size())
        return 4;
    executable.resize(length);
    test::CheckpointDirectory scratch;
    const auto profile = *encode_profile(LocalProfile{31, 2, 37, true});
    const auto checkpoint =
        *encode_checkpoint(*capture_checkpoint(start_run(RunSeed{1})));
    for (auto suffix : {L"", L".bak", L".tmp", L".bak.tmp"})
    {
        test::write_bytes(
            scratch.path / (std::wstring{L"profile.bin"} + suffix), profile);
        test::write_bytes(scratch.path /
                              (std::wstring{L"checkpoint.bin"} + suffix),
                          checkpoint);
    }
    const auto t = encode_rotation_binding(*RotationBinding::from_letter('T'));
    for (auto mode : {L"--write", L"--read", L"--deny", L"--read"})
    {
        if (!launch(executable, argv[1], scratch.path, mode))
        {
            std::wcerr << L"Binding child failed: " << mode << L'\n';
            return 5;
        }
        if (!std::ranges::equal(
                test::read_bytes(scratch.path / L"rotation-key.bin"), t))
            return 6;
        for (auto suffix : {L"", L".bak", L".tmp", L".bak.tmp"})
            if (!std::ranges::equal(
                    test::read_bytes(scratch.path /
                                     (std::wstring{L"profile.bin"} + suffix)),
                    profile) ||
                !std::ranges::equal(
                    test::read_bytes(
                        scratch.path /
                        (std::wstring{L"checkpoint.bin"} + suffix)),
                    checkpoint))
                return 7;
    }
    return 0;
}
