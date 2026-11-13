#pragma once
#include "application/action_preview.hpp"
#include "application/action_selection.hpp"
#include "core/run_turn.hpp"
#include "platform/windows/board_view.hpp"
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
           const TextCatalog *hud_catalog = nullptr, bool show_start = false)
    {
        if (show_start && !hud_catalog)
            return {nullptr, E_INVALIDARG};
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
    void key_available() noexcept override
    {
        try
        {
            if (!window_.focused())
            {
                selection_.reset();
                show_live(run_);
                return;
            }
            selection_.point(window_.pointed());
            const auto key = window_.take_key();
            if (!key)
                return;
            if (screen() != SessionScreen::run)
            {
                selection_.reset();
                if (*key == InputKey::confirm)
                {
                    auto next = run_;
                    auto flow = flow_;
                    if (flow.confirm(next))
                    {
                        view_->set_board(next.sector);
                        run_ = std::move(next);
                        flow_ = flow;
                        window_.discard_point();
                    }
                }
                error_ = S_OK;
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
            auto next = resolve_run_turn(run_, *command);
            if (!next.accepted)
            {
                view_->set_board(run_.sector);
                return;
            }
            // Copying into the view may throw; publish run_ only after that
            // copy succeeds. Both following move assignments are nonthrowing.
            show_live(next.state);
            run_ = std::move(next.state);
            if (screen() == SessionScreen::result)
            {
                selection_.reset();
                window_.discard_point();
            }
            else
                selection_.context_changed();
            error_ = S_OK;
        }
        catch (...)
        {
            error_ = E_FAIL;
        }
    }

  private:
    void show_live(const RunState &run)
    {
        view_->set_board(
            run.sector, {},
            lifecycle_ ? presentation::session_message(flow_.screen(run), run)
                       : std::nullopt);
    }
    void show_selection()
    {
        if (selection_.candidate())
        {
            auto preview = project_action(run_, *selection_.candidate());
            if (preview.result.accepted)
            {
                view_->set_board(std::move(preview.result.state.sector),
                                 std::move(preview.packet_route));
                error_ = S_OK;
                return;
            }
        }
        view_->set_board(run_.sector);
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
    std::unique_ptr<BoardView> view_;
    HRESULT error_ = S_OK;
};
} // namespace closed_circuit::windows
