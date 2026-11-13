#pragma once
#include "platform/windows/atlas_renderer.hpp"
#include "platform/windows/direct2d_target.hpp"
#include "platform/windows/native_window.hpp"
#include "presentation/hud_text.hpp"
#include "presentation/scene_projection.hpp"
#include <stdexcept>

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
           const std::filesystem::path &root, const AssetManifest &manifest,
           std::unique_ptr<TextRenderer> label = {},
           const TextCatalog *hud_catalog = nullptr)
    {
        auto device = Direct2DTarget::create(window.handle());
        if (!device.device)
            return {nullptr, device.error};
        auto atlas =
            AtlasRenderer::create(device.device->target(), root, manifest);
        if (!atlas.renderer)
            return {nullptr, atlas.error};
        auto owner = std::unique_ptr<BoardView>(new BoardView(
            window, board, root, manifest, std::move(device.device),
            std::move(atlas.renderer), std::move(label)));
        if (hud_catalog)
        {
            owner->hud_catalog_ = *hud_catalog;
            auto text = owner->make_hud(board);
            if (!text.renderer)
                return {nullptr, text.error};
            owner->label_ = std::move(text.renderer);
        }
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
    [[nodiscard]] float board_top() const noexcept
    {
        return label_ ? label_->extent().height + 8 : 0;
    }
    [[nodiscard]] std::wstring_view label_text() const noexcept
    {
        return label_ ? label_->text() : std::wstring_view{};
    }
    [[nodiscard]] std::optional<NodeId>
    node_at(float x, float y, float width, float height) const noexcept override
    {
        if (!device_->target())
            return std::nullopt;
        float dpi_x = 0, dpi_y = 0;
        device_->target()->GetDpi(&dpi_x, &dpi_y);
        if (dpi_x <= 0 || dpi_y <= 0)
            return std::nullopt;
        return presentation::board_node_at(
            x * 96 / dpi_x, y * 96 / dpi_y - board_top(), width * 96 / dpi_x,
            height * 96 / dpi_y - board_top());
    }
    [[nodiscard]] const std::vector<NodeId> &packet_route() const noexcept
    {
        return route_;
    }
    void set_board(GameState board, std::vector<NodeId> route = {})
    {
        if (hud_catalog_)
        {
            auto text = make_hud(board);
            if (!text.renderer)
                throw std::runtime_error("HUD layout failed");
            label_ = std::move(text.renderer);
        }
        board_ = std::move(board);
        route_ = std::move(route);
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
            std::vector<presentation::TileDraw> draws;
            if (size.height > board_top())
            {
                auto scene = presentation::scene_draws(
                    board_, size.width, size.height - board_top(), route_);
                if (!scene)
                {
                    error_ = E_INVALIDARG;
                    return;
                }
                draws = std::move(*scene);
                for (auto &draw : draws)
                    draw.y += board_top();
            }
            handle_draw_result(atlas_->draw(draws, label_.get()));
            if (error_ == S_OK)
                ++frames_;
        }
        catch (...)
        {
            error_ = E_FAIL; // Never unwind through a Windows callback.
        }
    }

  private:
    [[nodiscard]] TextCreateResult make_hud(const GameState &board) const
    {
        const auto hud =
            presentation::hud_lines(presentation::hud_values(board));
        const std::array lines{TextLine{"developer-window-title", std::nullopt},
                               hud[0], hud[1], hud[2]};
        return TextRenderer::create(*hud_catalog_, lines);
    }
    BoardView(NativeWindow &window, GameState board, std::filesystem::path root,
              AssetManifest manifest, std::unique_ptr<Direct2DTarget> device,
              std::unique_ptr<AtlasRenderer> atlas,
              std::unique_ptr<TextRenderer> label)
        : window_(window), board_(std::move(board)), root_(std::move(root)),
          manifest_(std::move(manifest)), device_(std::move(device)),
          atlas_(std::move(atlas)), label_(std::move(label))
    {
    }
    NativeWindow &window_;
    GameState board_;
    std::vector<NodeId> route_;
    std::filesystem::path root_;
    AssetManifest manifest_;
    std::unique_ptr<Direct2DTarget> device_;
    std::unique_ptr<AtlasRenderer> atlas_;
    std::unique_ptr<TextRenderer> label_;
    std::optional<TextCatalog> hud_catalog_;
    HRESULT error_ = S_OK;
    unsigned frames_ = 0;
};
} // namespace closed_circuit::windows
