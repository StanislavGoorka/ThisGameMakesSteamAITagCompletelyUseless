#pragma once
#include "application/action_preview.hpp"
#include "application/action_selection.hpp"
#include "core/run_turn.hpp"
#include "platform/windows/board_view.hpp"
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
           std::unique_ptr<TextRenderer> label = {})
    {
        auto board = BoardView::create(window, initial.sector, root, manifest,
                                       std::move(label));
        if (!board.view)
            return {nullptr, board.error};
        auto owner = std::unique_ptr<RunWindow>(
            new RunWindow(window, std::move(initial), std::move(board.view)));
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
                view_->set_board(run_.sector);
                return;
            }
            selection_.point(window_.pointed());
            const auto key = window_.take_key();
            if (!key)
                return;
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
            view_->set_board(next.state.sector);
            run_ = std::move(next.state);
            selection_.context_changed();
            error_ = S_OK;
        }
        catch (...)
        {
            error_ = E_FAIL;
        }
    }

  private:
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
    ActionSelection selection_;
    std::unique_ptr<BoardView> view_;
    HRESULT error_ = S_OK;
};
} // namespace closed_circuit::windows
