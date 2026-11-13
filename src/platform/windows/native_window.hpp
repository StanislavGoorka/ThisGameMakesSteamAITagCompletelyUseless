#pragma once
#include "application/keyboard_commands.hpp"
#include "application/text_catalog.hpp"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <memory>
#include <utility>

namespace closed_circuit::windows
{
struct WindowPainter
{
    virtual ~WindowPainter() = default;
    virtual void paint(HWND window) noexcept = 0;
};
struct MessageLoopResult
{
    int exit_code;
    DWORD error;
};

// Optional filter is for narrow API consumers; the window loop uses nullptr
// so destruction of the main HWND never invalidates the GetMessage filter.
inline MessageLoopResult dispatch_messages(HWND filter = nullptr)
{
    MSG message{};
    for (;;)
    {
        const auto result = GetMessageW(&message, filter, 0, 0);
        if (result == -1)
            return {1, GetLastError()};
        if (result == 0)
            return {static_cast<int>(message.wParam), ERROR_SUCCESS};
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

class NativeWindow;
struct WindowCreateResult
{
    std::unique_ptr<NativeWindow> window;
    DWORD error;
};

// Thread-affine ownership: create and destroy on the same Win32 thread.
// Starts hidden, with no artwork, background brush or icon.
class NativeWindow
{
  public:
    NativeWindow(const NativeWindow &) = delete;
    NativeWindow &operator=(const NativeWindow &) = delete;
    ~NativeWindow()
    {
        if (handle_)
            DestroyWindow(handle_);
        if (atom_)
            UnregisterClassW(MAKEINTRESOURCEW(atom_), instance_);
    }
    [[nodiscard]] HWND handle() const noexcept
    {
        return handle_;
    }
    [[nodiscard]] bool focused() const noexcept
    {
        return focused_;
    }
    // Borrowed callback, detached by its owner before destruction.
    void set_painter(WindowPainter *painter) noexcept
    {
        painter_ = painter;
        if (handle_)
            InvalidateRect(handle_, nullptr, FALSE);
    }
    [[nodiscard]] std::optional<Command> take_command(NodeId player)
    {
        const auto key = std::exchange(pending_key_, std::nullopt);
        return key && handle_ && focused_ ? map_key_command(*key, player)
                                          : std::nullopt;
    }
    [[nodiscard]] MessageLoopResult run_message_loop()
    {
        if (!handle_ || loop_active_)
            return {1, ERROR_INVALID_WINDOW_HANDLE};
        loop_active_ = true;
        const auto result = dispatch_messages();
        loop_active_ = false;
        return result;
    }
    [[nodiscard]] static WindowCreateResult create(const TextCatalog &catalog,
                                                   std::string_view title_id);

  private:
    NativeWindow() = default;
    static std::optional<InputKey> translate_key(WPARAM key) noexcept
    {
        switch (key)
        {
        case VK_UP:
            return InputKey::north;
        case VK_RIGHT:
            return InputKey::east;
        case VK_DOWN:
            return InputKey::south;
        case VK_LEFT:
            return InputKey::west;
        case 'R':
            return InputKey::rotate;
        case 'O':
            return InputKey::overdrive;
        case VK_SPACE:
            return InputKey::wait;
        default:
            return std::nullopt;
        }
    }
    static LRESULT CALLBACK window_proc(HWND handle, UINT message,
                                        WPARAM wparam, LPARAM lparam)
    {
        if (message == WM_NCCREATE)
        {
            auto *owner = static_cast<NativeWindow *>(
                reinterpret_cast<const CREATESTRUCTW *>(lparam)
                    ->lpCreateParams);
            SetLastError(ERROR_SUCCESS);
            const auto previous = SetWindowLongPtrW(
                handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(owner));
            if (previous == 0 && GetLastError() != ERROR_SUCCESS)
                return FALSE;
            owner->handle_ = handle;
        }
        auto *owner = reinterpret_cast<NativeWindow *>(
            GetWindowLongPtrW(handle, GWLP_USERDATA));
        if (owner && message == WM_PAINT)
        {
            PAINTSTRUCT paint{};
            BeginPaint(handle, &paint);
            if (owner->painter_)
                owner->painter_->paint(handle);
            EndPaint(handle, &paint);
            return 0;
        }
        if (owner && owner->painter_ && message == WM_SIZE)
            InvalidateRect(handle, nullptr, FALSE);
        if (owner && (message == WM_SETFOCUS || message == WM_KILLFOCUS))
        {
            owner->focused_ = message == WM_SETFOCUS;
            owner->input_.set_focus(owner->focused_);
            if (!owner->focused_)
                owner->pending_key_.reset();
        }
        if (owner && (message == WM_KEYDOWN || message == WM_KEYUP))
        {
            if (const auto key = translate_key(wparam))
            {
                if (message == WM_KEYUP)
                    owner->input_.key_up(*key);
                else if (owner->input_.key_down(*key,
                                                (lparam & (1LL << 30)) != 0) &&
                         !owner->pending_key_)
                    owner->pending_key_ = key;
                return 0;
            }
        }
        if (owner && message == WM_DESTROY)
        {
            owner->focused_ = false;
            owner->input_.set_focus(false);
            owner->pending_key_.reset();
            if (owner->loop_active_)
                PostQuitMessage(0);
        }
        if (message == WM_NCDESTROY)
        {
            if (owner)
                owner->handle_ = nullptr;
            SetWindowLongPtrW(handle, GWLP_USERDATA, 0);
        }
        return DefWindowProcW(handle, message, wparam, lparam);
    }
    HINSTANCE instance_ = nullptr;
    ATOM atom_ = 0;
    HWND handle_ = nullptr;
    bool focused_ = false;
    bool loop_active_ = false;
    KeyboardInput input_;
    std::optional<InputKey> pending_key_;
    WindowPainter *painter_ = nullptr;
};

inline WindowCreateResult NativeWindow::create(const TextCatalog &catalog,
                                               std::string_view title_id)
{
    const auto *text = catalog.find(title_id);
    if (!text)
        return {nullptr, ERROR_NOT_FOUND};
    const auto length =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text->data(),
                            static_cast<int>(text->size()), nullptr, 0);
    if (!length)
        return {nullptr, GetLastError()};
    std::wstring title(length, L'\0');
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text->data(),
                             static_cast<int>(text->size()), title.data(),
                             length))
        return {nullptr, GetLastError()};
    auto owner = std::unique_ptr<NativeWindow>(new NativeWindow);
    owner->instance_ = GetModuleHandleW(nullptr);
    if (!owner->instance_)
        return {nullptr, GetLastError()};
    // Internal class identity, never a display string. Each owner has its own
    // class so destruction cannot unregister another live window's class.
    const auto name =
        L"ClosedCircuit.Native." +
        std::to_wstring(reinterpret_cast<std::uintptr_t>(owner.get()));
    WNDCLASSEXW description{};
    description.cbSize = sizeof(description);
    description.hInstance = owner->instance_;
    description.lpfnWndProc = window_proc;
    description.lpszClassName = name.c_str();
    owner->atom_ = RegisterClassExW(&description);
    if (!owner->atom_)
        return {nullptr, GetLastError()};
    if (!CreateWindowExW(0, MAKEINTRESOURCEW(owner->atom_), title.c_str(),
                         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640,
                         480, nullptr, nullptr, owner->instance_, owner.get()))
        return {nullptr, GetLastError()};
    return {std::move(owner), ERROR_SUCCESS};
}
} // namespace closed_circuit::windows
