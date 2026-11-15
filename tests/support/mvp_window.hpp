#pragma once
#include "checkpoint_files.hpp"
#include "developer_text.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include "platform/windows/resume_window.hpp"
#include "run_checks.hpp"
#include <iostream>
#include <map>

namespace closed_circuit::test
{
using MvpFiles = std::map<std::filesystem::path, std::vector<std::uint8_t>>;
inline MvpFiles mvp_files(const std::filesystem::path &directory)
{
    MvpFiles result;
    for (const auto &entry : std::filesystem::directory_iterator(directory))
        result[entry.path().filename()] = read_bytes(entry.path());
    return result;
}
inline bool mvp_fail(std::string_view stage)
{
    std::cerr << stage << '\n';
    return false;
}
class MvpWindow
{
  public:
    MvpWindow(const std::filesystem::path &root,
              const std::filesystem::path &directory)
        : directory(directory), checkpoints(directory / L"checkpoint.bin"),
          sink(directory / L"profile.bin"), notices(sink)
    {
        window =
            windows::NativeWindow::create(catalog, "developer-window-title")
                .window;
        if (!window)
            throw std::runtime_error("Cannot create test HWND");
        resume = windows::resume_checkpoint_file(directory / L"checkpoint.bin");
        session = windows::create_resumed_window(*window, resume, RunSeed{1},
                                                 root, assets, catalog,
                                                 &notices, &checkpoints)
                      .session;
        if (!session)
            throw std::runtime_error("Cannot create test session");
        SetWindowPos(window->handle(), nullptr, -32000, -32000, 1000, 800,
                     SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(window->handle());
        SendMessageW(window->handle(), WM_SETFOCUS, 0, 0);
    }
    ~MvpWindow()
    {
        SendMessageW(window->handle(), WM_CLOSE, 0, 0);
        session.reset();
    }
    void tap(WPARAM key)
    {
        SendMessageW(window->handle(), WM_KEYDOWN, key, 0);
        SendMessageW(window->handle(), WM_KEYUP, key, 0);
    }
    bool matches(const RunState &run) const
    {
        return same_run(session->state(), run) &&
               same_state(session->view().board(), run.sector) &&
               !session->checkpoint_failed() &&
               !session->achievements_pending() && SUCCEEDED(session->error());
    }
    bool checkpoint(const RunState &run) const
    {
        const auto captured = capture_checkpoint(run);
        const auto bytes =
            captured ? encode_checkpoint(*captured) : std::nullopt;
        return bytes && read_bytes(directory / L"checkpoint.bin") ==
                            std::vector(bytes->begin(), bytes->end());
    }
    bool quiet(const RunState &run)
    {
        const auto before = mvp_files(directory);
        tap(VK_TAB);
        tap(VK_SPACE);
        const auto preview = preview_run_turn(run, WaitCommand{});
        if (!same_run(session->state(), run) ||
            !same_state(session->view().board(), preview.state.sector) ||
            mvp_files(directory) != before)
            return mvp_fail("preview side effect");
        tap(VK_ESCAPE);
        // Every generated sector starts at node zero; west is illegal.
        tap(VK_LEFT);
        return matches(run) && mvp_files(directory) == before;
    }
    bool apply(RunState &run, const Command &command, WPARAM key,
               std::optional<NodeId> target = {})
    {
        if (target)
        {
            // Use the same hit-test port as mouse input, including actual DPI.
            RECT client{};
            GetClientRect(window->handle(), &client);
            bool found = false;
            for (int y = 0; y < client.bottom && !found; ++y)
                for (int x = 0; x < client.right; ++x)
                    if (session->view().node_at(
                            static_cast<float>(x), static_cast<float>(y),
                            static_cast<float>(client.right),
                            static_cast<float>(client.bottom)) == target)
                    {
                        SendMessageW(window->handle(), WM_MOUSEMOVE, 0,
                                     MAKELPARAM(x, y));
                        found = true;
                        break;
                    }
            if (!found)
                return mvp_fail("target hit test");
        }
        const auto expected = resolve_run_turn(run, command);
        tap(key);
        if (!expected.accepted || !matches(expected.state))
            return mvp_fail("full turn state");
        run = expected.state;
        return true;
    }
    bool solve(RunState &run)
    {
        // Test-only witness for the generator's two perimeter routes.
        const bool top = run.sector.receiver_nodes[4];
        const auto at = [top](int i)
        {
            return top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                       : (i <= 6 ? i * 7 : 42 + i - 6);
        };
        const auto direction = [](int from, int to) {
            return to == from + 1 ? 2 : to == from - 1 ? 8 : to > from ? 4 : 1;
        };
        const auto saved = read_bytes(directory / L"checkpoint.bin");
        for (int i = 1; i < 13; ++i)
        {
            const auto target = *NodeId::from_value(at(i));
            const auto desired = direction(at(i), at(i - 1)) |
                                 (i < 12 ? direction(at(i), at(i + 1)) : 0);
            int rotations = 0;
            while (run.sector.ports[at(i)].bits() != desired && rotations++ < 4)
                if (!apply(run, RotateCommand{target}, 'R', target))
                    return false;
            if (run.sector.ports[at(i)].bits() != desired ||
                !apply(run, MoveCommand{target},
                       at(i) - at(i - 1) == 1 ? VK_RIGHT : VK_DOWN))
                return false;
        }
        while (run_phase(run) == RunPhase::playing)
            if (!apply(run, WaitCommand{}, VK_SPACE))
                return false;
        if (sector_outcome(run.sector) != SectorOutcome::completed)
            return mvp_fail("sector completion");
        // Expected swift_sector (ID6) from the reference run, not the
        // evaluator: this sector completed within 16 turns.
        if (static_cast<std::uint32_t>(run.sector.turns_taken) <= 16)
            swift = 0x20;
        return run_phase(run) == RunPhase::won
                   ? !std::filesystem::exists(directory / L"checkpoint.bin") &&
                         !std::filesystem::exists(directory /
                                                  L"checkpoint.bin.bak")
                   : read_bytes(directory / L"checkpoint.bin") == saved;
    }
    bool advance(RunState &run)
    {
        const auto before = mvp_files(directory);
        tap(VK_RIGHT);
        const auto candidate = session->reward_candidate();
        if (mvp_files(directory) != before ||
            (candidate
                 ? !choose_run_module(run, *candidate)
                 : run.sector.modules.size() != ModuleLoadout::slot_limit))
            return mvp_fail("reward selection");
        const auto expected = advance_run(run);
        tap(VK_RETURN);
        if (!expected.accepted || !matches(expected.state) ||
            !checkpoint(expected.state) ||
            read_bytes(directory / L"checkpoint.bin.bak") !=
                before.at(L"checkpoint.bin"))
            return mvp_fail("boundary checkpoint");
        run = expected.state;
        return true;
    }
    std::filesystem::path directory;
    const TextCatalog catalog = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    const AssetManifest assets{presentation::atlas_reference()};
    windows::LocalCheckpointStore checkpoints;
    windows::LocalAchievementSink sink;
    AchievementNoticeSink notices;
    windows::CheckpointResume resume;
    std::unique_ptr<windows::NativeWindow> window;
    std::unique_ptr<windows::RunWindow> session;
    std::uint16_t swift = 0;
};
} // namespace closed_circuit::test
