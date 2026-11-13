#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <d2d1.h>
#include <memory>
#include <wrl/client.h>

namespace closed_circuit::windows
{
class Direct2DTarget;
struct Direct2DCreateResult
{
    std::unique_ptr<Direct2DTarget> device;
    HRESULT error;
};

// Thread-affine COM resources. The caller owns the HWND and outlives us.
class Direct2DTarget
{
  public:
    Direct2DTarget(const Direct2DTarget &) = delete;
    Direct2DTarget &operator=(const Direct2DTarget &) = delete;
    [[nodiscard]] ID2D1HwndRenderTarget *target() const noexcept
    {
        return target_.Get();
    }
    [[nodiscard]] ID2D1Factory *factory() const noexcept
    {
        return factory_.Get();
    }
    [[nodiscard]] HRESULT resize(UINT32 width, UINT32 height) noexcept
    {
        if (!IsWindow(window_))
        {
            target_.Reset();
            return HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE);
        }
        if (!width || !height)
        {
            target_.Reset();
            return S_FALSE;
        }
        const auto size = D2D1::SizeU(width, height);
        const auto result =
            target_ ? target_->Resize(size)
                    : factory_->CreateHwndRenderTarget(
                          D2D1::RenderTargetProperties(),
                          D2D1::HwndRenderTargetProperties(window_, size),
                          target_.GetAddressOf());
        if (FAILED(result))
            target_.Reset();
        return result;
    }
    // The future drawing caller supplies the actual EndDraw result.
    // Tests inject only the status; no image is drawn or presented here.
    [[nodiscard]] HRESULT handle_draw_result(HRESULT result) noexcept
    {
        if (result == D2DERR_RECREATE_TARGET)
            target_.Reset();
        return result;
    }
    [[nodiscard]] static Direct2DCreateResult create(HWND window)
    {
        if (!IsWindow(window))
            return {nullptr, HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE)};
        RECT client{};
        if (!GetClientRect(window, &client))
            return {nullptr, HRESULT_FROM_WIN32(GetLastError())};
        if (client.right <= 0 || client.bottom <= 0)
            return {nullptr, E_INVALIDARG};
        auto owner =
            std::unique_ptr<Direct2DTarget>(new Direct2DTarget(window));
        auto result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                        owner->factory_.GetAddressOf());
        if (FAILED(result))
            return {nullptr, result};
        result = owner->resize(static_cast<UINT32>(client.right),
                               static_cast<UINT32>(client.bottom));
        if (FAILED(result))
            return {nullptr, result};
        return {std::move(owner), S_OK};
    }

  private:
    explicit Direct2DTarget(HWND window) noexcept : window_(window)
    {
    }
    HWND window_;
    Microsoft::WRL::ComPtr<ID2D1Factory> factory_;
    Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> target_;
};
} // namespace closed_circuit::windows
