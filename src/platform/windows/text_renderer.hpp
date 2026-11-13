#pragma once
#include "application/text_line.hpp"
#include "platform/windows/text_utf16.hpp"
#include <cmath>
#include <d2d1.h>
#include <dwrite.h>
#include <memory>
#include <wrl/client.h>

namespace closed_circuit::windows
{
class TextRenderer;
struct TextCreateResult
{
    std::unique_ptr<TextRenderer> renderer;
    HRESULT error;
};

class TextRenderer
{
  public:
    TextRenderer(const TextRenderer &) = delete;
    TextRenderer &operator=(const TextRenderer &) = delete;
    [[nodiscard]] std::wstring_view text() const noexcept
    {
        return text_;
    }
    [[nodiscard]] D2D1_SIZE_F extent() const noexcept
    {
        return extent_;
    }
    [[nodiscard]] static TextCreateResult create(const TextCatalog &catalog,
                                                 std::string_view id)
    {
        const TextLine line{id, std::nullopt};
        return create(catalog, std::span{&line, 1});
    }
    // Build a complete replacement before publishing it to a view.
    [[nodiscard]] static TextCreateResult
    create(const TextCatalog &catalog, std::span<const TextLine> lines)
    {
        if (lines.empty() || lines.size() > 16)
            return {nullptr, E_INVALIDARG};
        std::string block;
        for (const auto &line : lines)
        {
            const auto formatted = format_text_line(catalog, line);
            if (!formatted)
                return {nullptr, HRESULT_FROM_WIN32(ERROR_NOT_FOUND)};
            if (!block.empty())
                block += '\n';
            block += *formatted;
        }
        auto value = decode_text_utf16(block);
        if (!value.text)
            return {nullptr, HRESULT_FROM_WIN32(value.error)};
        auto owner = std::unique_ptr<TextRenderer>(new TextRenderer);
        owner->text_ = std::move(*value.text);
        auto result = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown **>(owner->factory_.GetAddressOf()));
        if (FAILED(result))
            return {nullptr, result};
        result = owner->factory_->CreateTextFormat(
            L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16, L"en-us",
            owner->format_.GetAddressOf());
        if (FAILED(result))
            return {nullptr, result};
        result = owner->format_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (FAILED(result))
            return {nullptr, result};
        result = owner->factory_->CreateTextLayout(
            owner->text_.data(), static_cast<UINT32>(owner->text_.size()),
            owner->format_.Get(), 1000000, 1000000,
            owner->layout_.GetAddressOf());
        if (FAILED(result))
            return {nullptr, result};
        DWRITE_TEXT_METRICS metrics{};
        result = owner->layout_->GetMetrics(&metrics);
        if (FAILED(result))
            return {nullptr, result};
        result = owner->layout_->SetMaxWidth(
            metrics.widthIncludingTrailingWhitespace);
        if (FAILED(result))
            return {nullptr, result};
        result = owner->layout_->SetMaxHeight(metrics.height);
        if (FAILED(result))
            return {nullptr, result};
        DWRITE_OVERHANG_METRICS ink{};
        result = owner->layout_->GetOverhangMetrics(&ink);
        if (FAILED(result))
            return {nullptr, result};
        owner->origin_ =
            D2D1::Point2F(std::max(0.f, ink.left), std::max(0.f, ink.top));
        owner->extent_ =
            D2D1::SizeF(std::ceil(metrics.widthIncludingTrailingWhitespace +
                                  owner->origin_.x + std::max(0.f, ink.right)),
                        std::ceil(metrics.height + owner->origin_.y +
                                  std::max(0.f, ink.bottom)));
        return {std::move(owner), S_OK};
    }
    // Caller owns BeginDraw/EndDraw. Brushes never cross resource domains.
    [[nodiscard]] HRESULT draw(ID2D1RenderTarget *target,
                               D2D1_RECT_F bounds) const noexcept
    {
        if (!target || !std::isfinite(bounds.left) ||
            !std::isfinite(bounds.top) || !std::isfinite(bounds.right) ||
            !std::isfinite(bounds.bottom) || bounds.left < 0 ||
            bounds.top < 0 || bounds.right <= bounds.left ||
            bounds.bottom <= bounds.top || bounds.right > 1000000 ||
            bounds.bottom > 1000000)
            return E_INVALIDARG;
        const auto size = target->GetSize();
        if (bounds.right > size.width || bounds.bottom > size.height)
            return E_INVALIDARG;
        if (bounds.right - bounds.left < extent_.width ||
            bounds.bottom - bounds.top < extent_.height)
            return S_FALSE;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        const auto result = target->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White), brush.GetAddressOf());
        if (FAILED(result))
            return result;
        D2D1_MATRIX_3X2_F previous{};
        target->GetTransform(&previous);
        target->SetTransform(D2D1::Matrix3x2F::Identity());
        target->DrawTextLayout(
            D2D1::Point2F(bounds.left + origin_.x, bounds.top + origin_.y),
            layout_.Get(), brush.Get());
        target->SetTransform(previous);
        return S_OK;
    }

  private:
    TextRenderer() = default;
    std::wstring text_;
    D2D1_SIZE_F extent_{};
    D2D1_POINT_2F origin_{};
    Microsoft::WRL::ComPtr<IDWriteFactory> factory_;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> format_;
    Microsoft::WRL::ComPtr<IDWriteTextLayout> layout_;
};
} // namespace closed_circuit::windows
