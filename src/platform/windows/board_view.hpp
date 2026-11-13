#pragma once
#include "platform/windows/atlas_renderer.hpp"
#include "platform/windows/direct2d_target.hpp"
#include "platform/windows/native_window.hpp"
#include "presentation/scene_projection.hpp"

namespace closed_circuit::windows
{
class BoardView;
struct BoardViewResult
{
    std::unique_ptr<BoardView> view;
    HRESULT error;
};

// NativeWindow and the COM apartment outlive this thread-affine owner.
class BoardView final : public WindowPainter
{
  public:
    ~BoardView() override
    {
        window_.set_painter(nullptr);
    }
    [[nodiscard]] static BoardViewResult
    create(NativeWindow &window, const GameState &board,
           const std::filesystem::path &root, const AssetManifest &manifest)
    {
        auto device = Direct2DTarget::create(window.handle());
        if (!device.device)
            return {nullptr, device.error};
        auto atlas =
            AtlasRenderer::create(device.device->target(), root, manifest);
        if (!atlas.renderer)
            return {nullptr, atlas.error};
        auto owner = std::unique_ptr<BoardView>(
            new BoardView(window, board, root, manifest,
                          std::move(device.device), std::move(atlas.renderer)));
        window.set_painter(owner.get());
        return {std::move(owner), S_OK};
    }
    [[nodiscard]] HRESULT error() const noexcept
    {
        return error_;
    }
    [[nodiscard]] unsigned frames() const noexcept
    {
        return frames_;
    }
    [[nodiscard]] const GameState &board() const noexcept
    {
        return board_;
    }
    void set_board(GameState board)
    {
        board_ = std::move(board);
        if (window_.handle())
            InvalidateRect(window_.handle(), nullptr, FALSE);
    }
    void handle_draw_result(HRESULT result) noexcept
    {
        error_ = device_->handle_draw_result(result);
        if (result == D2DERR_RECREATE_TARGET)
        {
            atlas_.reset();
            if (window_.handle())
                InvalidateRect(window_.handle(), nullptr, FALSE);
        }
    }
    void paint(HWND window) noexcept override
    {
        try
        {
            RECT client{};
            if (!GetClientRect(window, &client))
            {
                error_ = HRESULT_FROM_WIN32(GetLastError());
                return;
            }
            error_ = device_->resize(client.right, client.bottom);
            if (error_ != S_OK)
            {
                atlas_.reset();
                return;
            }
            if (!atlas_)
            {
                auto created =
                    AtlasRenderer::create(device_->target(), root_, manifest_);
                error_ = created.error;
                atlas_ = std::move(created.renderer);
                if (!atlas_)
                    return;
            }
            const auto size = device_->target()->GetSize();
            const auto draws =
                presentation::scene_draws(board_, size.width, size.height);
            if (!draws)
            {
                error_ = E_INVALIDARG;
                return;
            }
            handle_draw_result(atlas_->draw(*draws));
            if (error_ == S_OK)
                ++frames_;
        }
        catch (...)
        {
            error_ = E_FAIL; // Never unwind through a Windows callback.
        }
    }

  private:
    BoardView(NativeWindow &window, GameState board, std::filesystem::path root,
              AssetManifest manifest, std::unique_ptr<Direct2DTarget> device,
              std::unique_ptr<AtlasRenderer> atlas)
        : window_(window), board_(std::move(board)), root_(std::move(root)),
          manifest_(std::move(manifest)), device_(std::move(device)),
          atlas_(std::move(atlas))
    {
    }
    NativeWindow &window_;
    GameState board_;
    std::filesystem::path root_;
    AssetManifest manifest_;
    std::unique_ptr<Direct2DTarget> device_;
    std::unique_ptr<AtlasRenderer> atlas_;
    HRESULT error_ = S_OK;
    unsigned frames_ = 0;
};
} // namespace closed_circuit::windows
