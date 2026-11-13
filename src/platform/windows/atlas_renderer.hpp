#pragma once
#include "platform/windows/png_read.hpp"
#include "platform/windows/text_renderer.hpp"
#include "presentation/board_projection.hpp"
#include <d2d1.h>
#include <memory>

namespace closed_circuit::windows
{
class AtlasRenderer;
struct AtlasCreateResult
{
    std::unique_ptr<AtlasRenderer> renderer;
    HRESULT error;
};

// Thread-affine bitmap and its exact resource domain live and die together.
class AtlasRenderer
{
  public:
    AtlasRenderer(const AtlasRenderer &) = delete;
    AtlasRenderer &operator=(const AtlasRenderer &) = delete;
    [[nodiscard]] static AtlasCreateResult
    create(ID2D1RenderTarget *target, const std::filesystem::path &root,
           std::span<const AssetReference> manifest)
    {
        if (!target)
            return {nullptr, E_INVALIDARG};
        const auto *reference =
            find_asset(manifest, presentation::atlas_reference().id());
        if (!reference)
            return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
        if (*reference != presentation::atlas_reference())
            return {nullptr, E_INVALIDARG};
        const auto decoded = read_png_asset(root, *reference);
        if (!decoded.image)
            return {nullptr, decoded.error};
        const auto &image = *decoded.image;
        if (image.width != presentation::atlas_width ||
            image.height != presentation::atlas_height)
            return {nullptr, WINCODEC_ERR_IMAGESIZEOUTOFRANGE};
        auto owner = std::unique_ptr<AtlasRenderer>(new AtlasRenderer);
        owner->target_ = target;
        const auto properties = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96, 96);
        const auto result = target->CreateBitmap(
            D2D1::SizeU(image.width, image.height), image.pixels.data(),
            image.stride, properties, owner->bitmap_.GetAddressOf());
        if (FAILED(result))
            return {nullptr, result};
        return {std::move(owner), S_OK};
    }
    [[nodiscard]] HRESULT draw(std::span<const presentation::TileDraw> tiles,
                               const TextRenderer *label = nullptr) noexcept
    {
        // Validate the whole frame before touching the previous frame.
        if (tiles.size() > 4096)
            return E_INVALIDARG;
        for (const auto &draw : tiles)
            if (!presentation::valid_tile(draw.tile) ||
                !std::isfinite(draw.x) || !std::isfinite(draw.y) ||
                !std::isfinite(draw.size) || std::abs(draw.x) > 1000000 ||
                std::abs(draw.y) > 1000000 || draw.size <= 0 ||
                draw.size > 1000000)
                return E_INVALIDARG;
        D2D1_MATRIX_3X2_F previous{};
        target_->GetTransform(&previous);
        target_->BeginDraw();
        target_->SetTransform(D2D1::Matrix3x2F::Identity());
        target_->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        for (const auto &draw : tiles)
        {
            const auto cx = draw.x + draw.size / 2;
            const auto cy = draw.y + draw.size / 2;
            // Exact quarter turns avoid trigonometric rounding at tile edges.
            const D2D1_MATRIX_3X2_F rotations[]{
                {1, 0, 0, 1, 0, 0},
                {0, 1, -1, 0, cx + cy, cy - cx},
                {-1, 0, 0, -1, 2 * cx, 2 * cy},
                {0, -1, 1, 0, cx - cy, cy + cx}};
            target_->SetTransform(rotations[draw.tile.quarter_turns]);
            const auto x = static_cast<float>(draw.tile.column * 17);
            const auto y = static_cast<float>(draw.tile.row * 17);
            target_->DrawBitmap(bitmap_.Get(),
                                D2D1::RectF(draw.x, draw.y, draw.x + draw.size,
                                            draw.y + draw.size),
                                1,
                                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                                D2D1::RectF(x, y, x + 16, y + 16));
        }
        HRESULT text_result = S_OK;
        const auto size = target_->GetSize();
        if (label && size.width > 8 &&
            size.height >= label->extent().height + 8)
            text_result = label->draw(
                target_.Get(),
                D2D1::RectF(4, 4, size.width - 4, label->extent().height + 4));
        target_->SetTransform(previous);
        const auto result = target_->EndDraw();
        return FAILED(result)        ? result
               : FAILED(text_result) ? text_result
                                     : S_OK;
    }

  private:
    AtlasRenderer() = default;
    Microsoft::WRL::ComPtr<ID2D1RenderTarget> target_;
    Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap_;
};
} // namespace closed_circuit::windows
