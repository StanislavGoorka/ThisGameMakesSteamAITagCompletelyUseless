#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "../support/xaudio2_fake.hpp"
#include "application/checkpoint_encode.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/resume_window.hpp"
#include "platform/windows/xaudio2_output.hpp"
#include <iostream>
using namespace closed_circuit;

bool check(const TextCatalog &catalog, const std::filesystem::path &root,
           const std::filesystem::path &path, int mode)
{
    test::FakeXAudio2Api api;
    if (mode == 1)
        api.engine_result = E_FAIL;
    if (mode == 2)
        api.mastering_result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    auto audio =
        std::make_unique<windows::XAudio2Output>(std::filesystem::path{}, api);
    AudioOutput &port = *audio;
    if (mode == 3 && api.callback)
        api.callback->OnCriticalError(XAUDIO2_E_DEVICE_INVALIDATED);
    const auto count = api.count;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const auto cue = static_cast<AudioCue>(raw);
        if (port.play(cue) != (is_audio_cue(cue) ? AudioPlayResult::unavailable
                                                 : AudioPlayResult::unknown))
            return false;
    }
    windows::LocalCheckpointStore store{path};
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return false;
    const AssetManifest assets{presentation::atlas_reference()};
    const windows::CheckpointResume resume{std::nullopt, ResumeStatus::none};
    auto made = windows::create_resumed_window(
        *window.window, resume, RunSeed{0}, root, assets, catalog, nullptr,
        &store, std::move(audio));
    if (!made.session || made.session->screen() != SessionScreen::start ||
        std::filesystem::exists(path))
        return false;
    const auto hwnd = window.window->handle();
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1000, 800,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    const auto tap = [hwnd](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    auto run = start_run(RunSeed{0});
    tap(VK_RETURN);
    const auto encoded = encode_checkpoint(*capture_checkpoint(run));
    if (!encoded || !std::ranges::equal(test::read_bytes(path), *encoded) ||
        !test::same_run(made.session->state(), run))
        return false;
    const auto saved = test::read_bytes(path);
    const auto expected = resolve_run_turn(run, WaitCommand{});
    tap(VK_TAB);
    tap(VK_SPACE);
    if (!made.session->selection().candidate() ||
        !test::same_run(made.session->state(), run) ||
        !test::same_state(made.session->view().board(), expected.state.sector))
        return false;
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    if (made.session->selection().candidate() ||
        !test::same_run(made.session->state(), run))
        return false;
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    tap(VK_SPACE);
    if (!test::same_run(made.session->state(), expected.state) ||
        test::read_bytes(path) != saved)
        return false;
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1100, 900,
                 SWP_NOACTIVATE | SWP_NOZORDER);
    UpdateWindow(hwnd);
    if (FAILED(made.session->error()) || made.session->checkpoint_failed() ||
        port.play(AudioCue::delivery) != AudioPlayResult::unavailable ||
        api.count != count)
        return false;
    made.session.reset();
    if (api.callback || !api.valid)
        return false;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    const auto apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(apartment))
        return 2;
    const auto catalog = TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    if (!catalog)
        return 3;
    int result = 0;
    {
        test::CheckpointDirectory directory;
        for (int mode = 0; mode < 4; ++mode)
            if (!check(*catalog, argv[1],
                       directory.path / (std::to_wstring(mode) + L".bin"),
                       mode))
            {
                std::cerr << "Audio window mode " << mode << '\n';
                result = 4;
                break;
            }
    }
    CoUninitialize();
    return result;
}
