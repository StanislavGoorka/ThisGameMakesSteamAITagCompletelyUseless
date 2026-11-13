#pragma once
#include "application/action_preview.hpp"
#include "application/action_selection.hpp"
#include "application/checkpoint_boundary.hpp"
#include "application/checkpoint_store.hpp"
#include "application/run_achievements.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_reward.hpp"
#include "core/run_transition.hpp"
#include "core/run_turn.hpp"
#include "platform/windows/board_view.hpp"
#include "presentation/achievement_text.hpp"
#include "presentation/checkpoint_text.hpp"
#include "presentation/reward_text.hpp"
#include "presentation/session_text.hpp"
#include <type_traits>

namespace closed_circuit::windows
{
class RunWindow;
struct RunWindowResult
{
    std::unique_ptr<RunWindow> session;
    HRESULT error;
};
static_assert(std::is_nothrow_move_assignable_v<RunState>);

// Thin adapter: the core resolver owns every rule and turn cost.
// NativeWindow and COM must outlive this session and its view.
class RunWindow final : public WindowCommandHandler
{
  public:
    ~RunWindow() override
    {
        window_.set_command_handler(nullptr);
    }
    [[nodiscard]] static RunWindowResult
    create(NativeWindow &window, RunState initial,
           const std::filesystem::path &root, const AssetManifest &manifest,
           std::unique_ptr<TextRenderer> label = {},
           const TextCatalog *hud_catalog = nullptr, bool show_start = false,
           AchievementNoticeSink *notice_sink = nullptr,
           CheckpointStore *checkpoints = nullptr)
    {
        if ((show_start || notice_sink || checkpoints) && !hud_catalog)
            return {nullptr, E_INVALIDARG};
        if (checkpoints && !hud_catalog->find("checkpoint-unsaved"))
            return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
        if (notice_sink)
            for (auto id :
                 {"achievement-first-delivery", "achievement-unavailable"})
                if (!hud_catalog->find(id))
                    return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
        if (hud_catalog)
            for (auto id : {"session-start", "session-won", "session-death",
                            "session-overload"})
                if (!hud_catalog->find(id))
                    return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
        auto board = BoardView::create(window, initial.sector, root, manifest,
                                       std::move(label), hud_catalog);
        if (!board.view)
            return {nullptr, board.error};
        auto owner = std::unique_ptr<RunWindow>(
            new RunWindow(window, std::move(initial), std::move(board.view)));
        owner->lifecycle_ = hud_catalog != nullptr;
        owner->flow_ = SessionFlow{!show_start};
        owner->notice_sink_ = notice_sink;
        // Creation never writes: only later boundaries reach the store.
        owner->checkpoints_ = checkpoints;
        if (notice_sink)
            owner->achievements_ = owner->make_achievements(
                owner->run_, 1,
                show_start ? RunHistoryOrigin::fresh_run
                           : RunHistoryOrigin::restored_checkpoint);
        owner->show_live(owner->run_);
        window.set_command_handler(owner.get());
        return {std::move(owner), S_OK};
    }
    [[nodiscard]] const RunState &state() const noexcept
    {
        return run_;
    }
    [[nodiscard]] const BoardView &view() const noexcept
    {
        return *view_;
    }
    [[nodiscard]] SessionScreen screen() const noexcept
    {
        return lifecycle_ ? flow_.screen(run_) : SessionScreen::run;
    }
    [[nodiscard]] const ActionSelection &selection() const noexcept
    {
        return selection_;
    }
    [[nodiscard]] HRESULT error() const noexcept
    {
        return FAILED(error_) ? error_ : view_->error();
    }
    [[nodiscard]] std::optional<ModuleId> reward_candidate() const noexcept
    {
        return reward_selection_.candidate(run_);
    }
    [[nodiscard]] bool achievements_pending() const noexcept
    {
        return achievements_ && achievements_->pending();
    }
    [[nodiscard]] bool checkpoint_failed() const noexcept
    {
        return checkpoint_failed_;
    }
    void key_available() noexcept override
    {
        try
        {
            if (!window_.focused())
            {
                selection_.reset();
                reward_selection_.reset();
                show_live(run_);
                return;
            }
            selection_.point(window_.pointed());
            const auto key = window_.take_key();
            if (!key)
                return;
            if (*key == InputKey::cancel && notice_sink_)
                notice_sink_->dismiss();
            if (*key == InputKey::confirm && achievements_pending())
            {
                static_cast<void>(achievements_->flush());
                show_live(run_, reward_candidate());
                error_ = S_OK;
                return;
            }
            if (screen() != SessionScreen::run)
            {
                selection_.reset();
                reward_selection_.reset();
                if (*key == InputKey::confirm)
                {
                    auto next = run_;
                    auto flow = flow_;
                    if (flow.confirm(next))
                    {
                        if (attempt_ == UINT64_MAX)
                            throw std::runtime_error("Attempt token exhausted");
                        auto delivery =
                            notice_sink_
                                ? make_achievements(next, attempt_ + 1,
                                                    RunHistoryOrigin::fresh_run)
                                : nullptr;
                        view_->set_board(next.sector, {}, std::nullopt, {},
                                         status_lines());
                        run_ = std::move(next);
                        flow_ = flow;
                        achievements_ = std::move(delivery);
                        ++attempt_;
                        window_.discard_point();
                        save_boundary();
                    }
                }
                show_live(run_);
                error_ = S_OK;
                return;
            }
            if (lifecycle_ && (run_phase(run_) == RunPhase::choosing_reward ||
                               run_phase(run_) == RunPhase::ready_for_next))
            {
                selection_.reset();
                reward_key(*key);
                return;
            }
            std::optional<Command> command;
            switch (*key)
            {
            case InputKey::plan:
                selection_.toggle();
                break;
            case InputKey::cancel:
                selection_.cancel();
                break;
            case InputKey::confirm:
                command = selection_.confirm();
                break;
            default:
                command = selection_.choose(*key, run_.sector.player_position);
                break;
            }
            if (!command)
            {
                show_selection();
                return;
            }
            auto resolved = resolve_run_turn_events(run_, *command);
            auto &next = resolved.turn;
            if (!next.accepted)
            {
                show_live(run_);
                return;
            }
            auto pending = achievements_
                               ? achievements_->prepare(resolved.events)
                               : std::vector<DomainEvent>{};
            // Copying into the view may throw; publish run_ only after that
            // copy succeeds. Both following move assignments are nonthrowing.
            show_live(next.state);
            run_ = std::move(next.state);
            if (achievements_)
                achievements_->publish(std::move(pending));
            reward_selection_.sync(run_);
            if (run_phase(run_) != RunPhase::playing)
            {
                selection_.reset();
                window_.discard_point();
            }
            else
                selection_.context_changed();
            save_boundary();
            if (achievements_)
                static_cast<void>(achievements_->flush());
            if (achievements_ || checkpoints_)
                show_live(run_);
            error_ = S_OK;
        }
        catch (...)
        {
            error_ = E_FAIL;
        }
    }

  private:
    [[nodiscard]] std::unique_ptr<RunAchievements>
    make_achievements(const RunState &run, std::uint64_t attempt,
                      RunHistoryOrigin origin)
    {
        auto first = run.sector_number;
        if (run_phase(run) == RunPhase::choosing_reward ||
            run_phase(run) == RunPhase::ready_for_next)
            first = *SectorNumber::from_value(first.value() + 1);
        return std::make_unique<RunAchievements>(
            DeliveryScope{run.seed, {attempt}, {1}, first}, *notice_sink_,
            origin);
    }
    [[nodiscard]] std::vector<TextLine> status_lines() const
    {
        std::vector<TextLine> lines;
        if (auto line = presentation::achievement_line(
                notice_sink_ ? notice_sink_->pending() : std::nullopt))
            lines.push_back(*line);
        if (auto line =
                presentation::achievement_storage_line(achievements_pending()))
            lines.push_back(*line);
        if (auto line =
                presentation::checkpoint_storage_line(checkpoint_failed_))
            lines.push_back(*line);
        return lines;
    }
    // Called after every published run change. A failure keeps the game
    // playable and stays visible until the next boundary succeeds.
    void save_boundary() noexcept
    {
        if (!checkpoints_)
            return;
        try
        {
            const auto boundary = checkpoint_boundary(run_);
            if (boundary.action == CheckpointAction::save)
                checkpoint_failed_ = !checkpoints_->save(*boundary.checkpoint);
            else if (boundary.action == CheckpointAction::clear)
                checkpoint_failed_ = !checkpoints_->clear();
        }
        catch (...)
        {
            checkpoint_failed_ = true;
        }
    }
    void reward_key(InputKey key)
    {
        auto selection = reward_selection_;
        selection.sync(run_);
        if (key == InputKey::west || key == InputKey::east)
            selection.move(run_, key == InputKey::east);
        else if (key == InputKey::cancel)
            selection.reset();
        else if (key == InputKey::confirm)
        {
            auto chosen = run_;
            if (run_phase(chosen) == RunPhase::choosing_reward)
            {
                const auto id = selection.candidate(chosen);
                if (!id || !choose_run_module(chosen, *id))
                {
                    show_live(run_, selection.candidate(run_), true);
                    return;
                }
            }
            auto next = advance_run(chosen);
            if (next.accepted)
            {
                show_live(next.state);
                run_ = std::move(next.state);
                reward_selection_.reset();
                window_.discard_point();
                save_boundary();
                if (checkpoints_)
                    show_live(run_);
                error_ = S_OK;
                return;
            }
            show_live(run_, selection.candidate(run_), true);
            return;
        }
        show_live(run_, selection.candidate(run_));
        reward_selection_ = selection;
        error_ = S_OK;
    }
    void show_live(const RunState &run, std::optional<ModuleId> candidate = {},
                   bool reward_error = false)
    {
        if (lifecycle_ && flow_.screen(run) == SessionScreen::run &&
            run_phase(run) == RunPhase::choosing_reward)
        {
            auto lines =
                presentation::reward_lines(reward_offer(run), candidate);
            if (reward_error)
                lines->push_back({"reward-error", std::nullopt});
            view_->set_board(run.sector, {}, std::nullopt, *lines,
                             status_lines());
            return;
        }
        if (lifecycle_ && flow_.screen(run) == SessionScreen::run &&
            run_phase(run) == RunPhase::ready_for_next)
        {
            view_->set_board(run.sector, {},
                             TextLine{"reward-continue", std::nullopt}, {},
                             status_lines());
            return;
        }
        view_->set_board(
            run.sector, {},
            lifecycle_ ? presentation::session_message(flow_.screen(run), run)
                       : std::nullopt,
            {}, status_lines());
    }
    void show_selection()
    {
        if (selection_.candidate())
        {
            auto preview = project_action(run_, *selection_.candidate());
            if (preview.result.accepted)
            {
                view_->set_board(std::move(preview.result.state.sector),
                                 std::move(preview.packet_route), std::nullopt,
                                 {}, status_lines());
                error_ = S_OK;
                return;
            }
        }
        view_->set_board(run_.sector, {}, std::nullopt, {}, status_lines());
        error_ = S_OK;
    }
    RunWindow(NativeWindow &window, RunState run,
              std::unique_ptr<BoardView> view)
        : window_(window), run_(std::move(run)), view_(std::move(view))
    {
    }
    NativeWindow &window_;
    RunState run_;
    SessionFlow flow_{true};
    bool lifecycle_ = false;
    ActionSelection selection_;
    RewardSelection reward_selection_;
    AchievementNoticeSink *notice_sink_ = nullptr;
    std::unique_ptr<RunAchievements> achievements_;
    CheckpointStore *checkpoints_ = nullptr;
    bool checkpoint_failed_ = false;
    std::uint64_t attempt_ = 1;
    std::unique_ptr<BoardView> view_;
    HRESULT error_ = S_OK;
};
} // namespace closed_circuit::windows
