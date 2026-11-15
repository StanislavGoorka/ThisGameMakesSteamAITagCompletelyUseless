#pragma once
#include "application/action_preview.hpp"
#include "application/action_selection.hpp"
#include "application/audio_output.hpp"
#include "application/checkpoint_boundary.hpp"
#include "application/checkpoint_store.hpp"
#include "application/rotation_binding_store.hpp"
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
           CheckpointStore *checkpoints = nullptr,
           std::unique_ptr<AudioOutput> audio = {},
           RotationBindingStore *bindings = nullptr)
    {
        if ((show_start || notice_sink || checkpoints || bindings) &&
            !hud_catalog)
            return {nullptr, E_INVALIDARG};
        if (bindings)
            for (auto id :
                 {"rotation-binding-help", "rotation-binding-prompt",
                  "rotation-binding-conflict", "rotation-binding-unavailable",
                  "rotation-binding-saved"})
                if (!hud_catalog->find(id))
                    return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
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
        for (const auto status :
             {ResumeStatus::corrupt, ResumeStatus::unavailable})
            owner->resume_lines_[static_cast<std::size_t>(status)] =
                hud_catalog &&
                hud_catalog->find(presentation::resume_line(status)->id);
        owner->flow_ = SessionFlow{!show_start};
        owner->notice_sink_ = notice_sink;
        // Creation never writes: only later boundaries reach the store.
        owner->checkpoints_ = checkpoints;
        owner->audio_ = std::move(audio);
        owner->bindings_ = bindings;
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
    // Shows why a saved run was not resumed, until a run is started. Only
    // catalog lines are accepted; platform codes never reach the label.
    [[nodiscard]] HRESULT show_resume_status(ResumeStatus status) noexcept
    {
        const auto index = static_cast<std::size_t>(status);
        if (index >= resume_lines_.size())
            return E_INVALIDARG;
        if (presentation::resume_line(status) && !resume_lines_[index])
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        const auto previous = resume_status_;
        resume_status_ = status;
        try
        {
            show_live(run_);
            return S_OK;
        }
        catch (...)
        {
            resume_status_ = previous;
            return E_FAIL;
        }
    }
    void key_available() noexcept override
    {
        try
        {
            if (!window_.focused())
            {
                if (binding_capture_)
                {
                    binding_capture_ = false;
                    binding_line_ = "rotation-binding-help";
                }
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
                // Left/right pick the configuration the next confirm starts,
                // up/down the difficulty modifiers it starts with.
                if (*key == InputKey::east)
                    static_cast<void>(flow_.select_next(run_));
                else if (*key == InputKey::west)
                    static_cast<void>(flow_.select_previous(run_));
                else if (*key == InputKey::north)
                    static_cast<void>(flow_.select_next_modifiers(run_));
                else if (*key == InputKey::south)
                    static_cast<void>(flow_.select_previous_modifiers(run_));
                else if (*key == InputKey::confirm)
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
                        resume_status_ = ResumeStatus::none;
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
            play_deliveries(resolved.events);
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
    bool binding_key(std::uint32_t key) noexcept override
    {
        if (!bindings_ || (!binding_capture_ && key != VK_F2))
            return false;
        try
        {
            if (key == VK_F2)
            {
                binding_capture_ = true;
                binding_line_ = "rotation-binding-prompt";
                selection_.reset();
                reward_selection_.reset();
                window_.discard_point();
            }
            else if (key == VK_ESCAPE)
            {
                binding_capture_ = false;
                binding_line_ = "rotation-binding-help";
            }
            else if (const auto candidate = RotationBinding::from_letter(key))
            {
                bool saved = false;
                try
                {
                    saved = bindings_->save(*candidate);
                }
                catch (...)
                {
                }
                binding_line_ = saved ? "rotation-binding-saved"
                                      : "rotation-binding-unavailable";
                if (saved)
                {
                    window_.set_rotation_binding(*candidate);
                    binding_capture_ = false;
                }
            }
            else
                binding_line_ = "rotation-binding-conflict";
            show_live(run_);
            error_ = S_OK;
        }
        catch (...)
        {
            error_ = E_FAIL;
        }
        return true;
    }

  private:
    void play_deliveries(std::span<const DomainEvent> events) noexcept
    {
        if (!audio_)
            return;
        for (const auto &event : events)
            if (std::holds_alternative<PacketDelivered>(event.payload))
                try
                {
                    static_cast<void>(audio_->play(AudioCue::delivery));
                }
                catch (...)
                {
                    // A committed turn and its durable services must continue.
                }
    }
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
        if (bindings_)
            lines.push_back({binding_line_, std::nullopt});
        if (auto line = presentation::resume_line(resume_status_))
            lines.push_back(*line);
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
        // Overlay lines replace the message, so the session message leads
        // and the labels of the configuration and modifiers to be started
        // follow it.
        const auto message =
            lifecycle_ ? presentation::session_message(flow_.screen(run), run)
                       : std::nullopt;
        std::vector<TextLine> lines;
        if (message)
        {
            lines.push_back(*message);
            for (const auto &line : presentation::session_lines(
                     flow_.screen(run), flow_.selected(run),
                     flow_.selected_modifiers(run)))
                lines.push_back(line);
        }
        view_->set_board(run.sector, {}, message, lines, status_lines());
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
    std::unique_ptr<AudioOutput> audio_;
    RotationBindingStore *bindings_ = nullptr;
    bool binding_capture_ = false;
    std::string_view binding_line_ = "rotation-binding-help";
    bool checkpoint_failed_ = false;
    // Indexed by ResumeStatus: whether the catalog holds that marked line.
    std::array<bool, 4> resume_lines_{};
    ResumeStatus resume_status_ = ResumeStatus::none;
    std::uint64_t attempt_ = 1;
    std::unique_ptr<BoardView> view_;
    HRESULT error_ = S_OK;
};
} // namespace closed_circuit::windows
