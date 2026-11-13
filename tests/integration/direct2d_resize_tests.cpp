#include "platform/windows/direct2d_target.hpp"
#include "platform/windows/native_window.hpp"
using namespace closed_circuit;

bool size_is(windows::Direct2DTarget &device, UINT32 width, UINT32 height)
{
    if (!device.target())
        return false;
    const auto size = device.target()->GetPixelSize();
    return size.width == width && size.height == height;
}

int main()
{
    const TextRecord record{"title", "[AI PLACEHOLDER] Resize test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto window = windows::NativeWindow::create(catalog, "title");
    if (!window.window)
        return 1;
    auto created = windows::Direct2DTarget::create(window.window->handle());
    if (!created.device)
        return 2;
    auto &device = *created.device;
    const auto factory = device.factory();
    for (int repeat = 0; repeat < 4; ++repeat)
    {
        for (const auto size :
             {D2D1::SizeU(1, 1), D2D1::SizeU(321, 197), D2D1::SizeU(624, 441)})
        {
            if (device.resize(size.width, size.height) != S_OK ||
                !size_is(device, size.width, size.height) ||
                device.target()->GetHwnd() != window.window->handle())
                return 3;
        }
        for (const auto size :
             {D2D1::SizeU(0, 0), D2D1::SizeU(0, 12), D2D1::SizeU(12, 0)})
            if (device.resize(size.width, size.height) != S_FALSE ||
                device.target() || device.factory() != factory)
                return 4;
        if (device.resize(123, 87) != S_OK || !size_is(device, 123, 87))
            return 5;
        // Hold a test-only reference to prove a new COM target is created.
        Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> old = device.target();
        if (device.handle_draw_result(S_OK) != S_OK ||
            device.handle_draw_result(E_FAIL) != E_FAIL ||
            device.target() != old.Get())
            return 6;
        if (device.handle_draw_result(D2DERR_RECREATE_TARGET) !=
                D2DERR_RECREATE_TARGET ||
            device.target() || device.factory() != factory)
            return 7;
        if (device.resize(71, 93) != S_OK || !size_is(device, 71, 93) ||
            device.target() == old.Get() || device.factory() != factory)
            return 8;
    }
    // A real HWND resize supplies the exact client pixel dimensions.
    if (!SetWindowPos(window.window->handle(), nullptr, 0, 0, 450, 310,
                      SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE))
        return 9;
    RECT client{};
    if (!GetClientRect(window.window->handle(), &client) ||
        FAILED(device.resize(client.right, client.bottom)) ||
        !size_is(device, client.right, client.bottom))
        return 10;
    SendMessageW(window.window->handle(), WM_CLOSE, 0, 0);
    for (int retry = 0; retry < 2; ++retry)
        if (device.resize(10, 10) !=
                HRESULT_FROM_WIN32(ERROR_INVALID_WINDOW_HANDLE) ||
            device.target() || device.factory() != factory)
            return 11;
    return 0;
}
