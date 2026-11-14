#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

class RecordingAudio final : public AudioOutput
{
  public:
    unsigned calls = 0;
    int mode = 0;
    AudioPlayResult play(AudioCue cue) override
    {
        ++calls;
        if (cue != AudioCue::delivery || mode == 3)
            throw std::runtime_error("Synthetic audio exception");
        return mode == 0   ? AudioPlayResult::played
               : mode == 1 ? AudioPlayResult::muted
                           : AudioPlayResult::unavailable;
    }
};
class GatedAchievement final : public AchievementSink
{
  public:
    explicit GatedAchievement(const std::filesystem::path &path) : durable(path)
    {
    }
    bool failing = true;
    windows::LocalAchievementSink durable;
    AchievementSinkResult unlock(AchievementId id) override
    {
        return failing ? AchievementSinkResult::unavailable
                       : durable.unlock(id);
    }
};

bool scenario(const std::filesystem::path &root, const TextCatalog &catalog,
              const std::filesystem::path &directory, int mode)
{
    const auto profile_path = directory / L"profile.bin";
    const auto checkpoint_path = directory / L"checkpoint.bin";
    LocalProfile profile{0, 2, 37, true};
    auto run = start_run(RunSeed{1});
    windows::LocalCheckpointStore checkpoints{checkpoint_path};
    if (!windows::save_profile_file(profile_path, profile) ||
        !checkpoints.save(*capture_checkpoint(run)))
        return false;
    const auto saved = test::read_bytes(checkpoint_path);
    const auto original_profile = test::read_bytes(profile_path);
    run.sector.receiver_nodes[48] = true;
    run.sector.packet =
        Packet{*NodeId::from_value(48), *NodeId::from_value(48)};
    auto window =
        windows::NativeWindow::create(catalog, "developer-window-title");
    if (!window.window)
        return false;
    auto audio = std::make_unique<RecordingAudio>();
    auto *recorded = audio.get();
    recorded->mode = mode;
    GatedAchievement sink{profile_path};
    AchievementNoticeSink notices{sink};
    const AssetManifest assets{presentation::atlas_reference()};
    auto made = windows::RunWindow::create(*window.window, run, root, assets,
                                           {}, &catalog, false, &notices,
                                           &checkpoints, std::move(audio));
    if (!made.session)
        return false;
    const auto hwnd = window.window->handle();
    const auto tap = [hwnd](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    SetWindowPos(hwnd, nullptr, -32000, -32000, 1000, 800,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    const auto unchanged = [&]
    {
        return recorded->calls == 0 &&
               test::same_run(made.session->state(), run) &&
               test::read_bytes(checkpoint_path) == saved &&
               test::read_bytes(profile_path) == original_profile;
    };
    for (int i = 0; i < 3; ++i)
    {
        tap(VK_TAB);
        tap(VK_SPACE);
        if (!unchanged())
            return false;
        tap(VK_ESCAPE);
    }
    // Pick a genuinely rejected direction from the actual generated board.
    bool rejected = false;
    for (auto [key, logical] : {std::pair{VK_UP, InputKey::north},
                                {VK_RIGHT, InputKey::east},
                                {VK_DOWN, InputKey::south},
                                {VK_LEFT, InputKey::west}})
    {
        const auto command =
            map_key_command(logical, run.sector.player_position);
        if (!command || !resolve_run_turn(run, *command).accepted)
        {
            tap(key);
            rejected = unchanged();
            break;
        }
    }
    if (!rejected)
        return false;
    const auto expected = resolve_run_turn_events(run, WaitCommand{});
    SendMessageW(hwnd, WM_KEYDOWN, VK_SPACE, 0);
    for (int i = 0; i < 20; ++i)
        SendMessageW(hwnd, WM_KEYDOWN, VK_SPACE, i % 2 ? 1LL << 30 : 0);
    SendMessageW(hwnd, WM_KEYUP, VK_SPACE, 0);
    if (!expected.turn.accepted || recorded->calls != 1 ||
        !test::same_run(made.session->state(), expected.turn.state) ||
        !test::same_state(made.session->view().board(),
                          expected.turn.state.sector) ||
        !made.session->achievements_pending() ||
        FAILED(made.session->error()) || made.session->checkpoint_failed() ||
        test::read_bytes(checkpoint_path) != saved ||
        test::read_bytes(profile_path) != original_profile)
        return false;
    sink.failing = false;
    tap(VK_RETURN);
    profile.achievement_bits = 1;
    const auto encoded = encode_profile(profile);
    if (!encoded ||
        !std::ranges::equal(test::read_bytes(profile_path), *encoded) ||
        test::read_bytes(directory / L"profile.bin.bak") != original_profile ||
        made.session->achievements_pending() || recorded->calls != 1 ||
        !test::same_run(made.session->state(), expected.turn.state) ||
        test::read_bytes(checkpoint_path) != saved)
        return false;
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    tap(VK_SPACE);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_SPACE, 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, VK_SPACE, 0);
    tap(VK_TAB);
    tap(VK_SPACE);
    tap(VK_ESCAPE);
    if (recorded->calls != 1 || FAILED(made.session->error()) ||
        !test::same_run(made.session->state(), expected.turn.state))
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
    if (!catalog)
        return 2;
    bool passed = true;
    {
        test::CheckpointDirectory scratch;
        for (int mode = 0; mode < 4; ++mode)
        {
            const auto directory = scratch.path / std::to_wstring(mode);
            std::filesystem::create_directory(directory);
            passed &= scenario(argv[1], *catalog, directory, mode);
        }
    }
    CoUninitialize();
    return passed ? 0 : 3;
}
