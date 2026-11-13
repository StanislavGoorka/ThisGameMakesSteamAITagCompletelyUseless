#include "platform/windows/direct2d_target.hpp"
#include "platform/windows/native_window.hpp"
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_copy_constructible_v<windows::Direct2DTarget>);
static_assert(!std::is_move_constructible_v<windows::Direct2DTarget>);

bool matches(windows::Direct2DTarget &device, HWND window)
{
    RECT client{};
    if (!GetClientRect(window, &client) || !device.factory() ||
        !device.target())
        return false;
    const auto size = device.target()->GetPixelSize();
    return device.target()->GetHwnd() == window &&
           size.width == static_cast<UINT32>(client.right) &&
           size.height == static_cast<UINT32>(client.bottom);
}

int main()
{
    const auto invalid = windows::Direct2DTarget::create(nullptr);
    if (invalid.device ||
        invalid.error != HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE))
        return 1;
    const TextRecord record{"title", "[AI PLACEHOLDER] Device test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    for (int repeat = 0; repeat < 4; ++repeat)
    {
        auto one = windows::NativeWindow::create(catalog, "title");
        auto two = windows::NativeWindow::create(catalog, "title");
        if (!one.window || !two.window)
            return 2;
        auto first = windows::Direct2DTarget::create(one.window->handle());
        auto second = windows::Direct2DTarget::create(two.window->handle());
        if (!first.device || !second.device)
        {
            std::cerr << "Direct2D creation failed: " << first.error << ", "
                      << second.error << '\n';
            return 3;
        }
        if (first.error != S_OK || second.error != S_OK ||
            !matches(*first.device, one.window->handle()) ||
            !matches(*second.device, two.window->handle()) ||
            first.device->target() == second.device->target())
            return 4;
        first.device.reset();
        if (!IsWindow(one.window->handle()) ||
            !matches(*second.device, two.window->handle()))
            return 5;
        second.device.reset();
        const auto stale = one.window->handle();
        one.window.reset();
        const auto rejected = windows::Direct2DTarget::create(stale);
        if (rejected.device ||
            rejected.error != HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE))
            return 6;
    }
    return 0;
}
