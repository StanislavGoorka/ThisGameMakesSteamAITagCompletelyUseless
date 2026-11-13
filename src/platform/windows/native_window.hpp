#pragma once
#include "application/keyboard_commands.hpp"
#include "application/text_catalog.hpp"
#include "platform/windows/text_utf16.hpp"
#include "presentation/board_projection.hpp"
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
    [[nodiscard]] virtual std::optional<NodeId>
    node_at(float x, float y, float width, float height) const noexcept
    {
        return presentation::board_node_at(x, y, width, height);
    }
};
struct WindowCommandHandler
{
    virtual ~WindowCommandHandler() = default;
    virtual void key_available() noexcept = 0;
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
    void set_command_handler(WindowCommandHandler *handler) noexcept
    {
        pending_key_.reset();
        command_handler_ = handler;
    }
    [[nodiscard]] std::optional<Command> take_command(NodeId player)
    {
        const auto key = take_key();
        return key ? map_key_command(*key, player) : std::nullopt;
    }
    [[nodiscard]] std::optional<InputKey> take_key() noexcept
    {
        const auto key = std::exchange(pending_key_, std::nullopt);
        return handle_ && focused_ ? key : std::nullopt;
    }
    [[nodiscard]] std::optional<NodeId> pointed() const noexcept
    {
        return pointed_;
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
        case VK_TAB:
            return InputKey::plan;
        case VK_RETURN:
            return InputKey::confirm;
        case VK_ESCAPE:
            return InputKey::cancel;
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
        if (owner && (message == WM_SIZE || message == WM_MOUSELEAVE))
            owner->set_point(std::nullopt);
        if (owner && message == WM_MOUSEMOVE)
        {
            RECT client{};
            std::optional<NodeId> point;
            if (owner->focused_ && GetClientRect(handle, &client))
                point = owner->node_at(static_cast<short>(LOWORD(lparam)),
                                       static_cast<short>(HIWORD(lparam)),
                                       static_cast<float>(client.right),
                                       static_cast<float>(client.bottom));
            TRACKMOUSEEVENT tracking{sizeof(TRACKMOUSEEVENT), TME_LEAVE, handle,
                                     0};
            TrackMouseEvent(&tracking);
            owner->set_point(point);
        }
        if (owner && (message == WM_SETFOCUS || message == WM_KILLFOCUS))
        {
            owner->focused_ = message == WM_SETFOCUS;
            owner->input_.set_focus(owner->focused_);
            if (!owner->focused_)
            {
                owner->pending_key_.reset();
                owner->pointed_.reset();
                if (owner->command_handler_)
                    owner->command_handler_->key_available();
            }
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
                {
                    owner->pending_key_ = key;
                    if (owner->command_handler_)
                        owner->command_handler_->key_available();
                }
                return 0;
            }
        }
        if (owner && message == WM_DESTROY)
        {
            owner->focused_ = false;
            owner->input_.set_focus(false);
            owner->pending_key_.reset();
            owner->pointed_.reset();
            if (owner->command_handler_)
                owner->command_handler_->key_available();
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
    void set_point(std::optional<NodeId> point) noexcept
    {
        if (pointed_ == point)
            return;
        pointed_ = point;
        if (command_handler_)
            command_handler_->key_available();
    }
    [[nodiscard]] std::optional<NodeId> node_at(float x, float y, float width,
                                                float height) const noexcept
    {
        return painter_ ? painter_->node_at(x, y, width, height)
                        : presentation::board_node_at(x, y, width, height);
    }
    std::optional<NodeId> pointed_;
    HINSTANCE instance_ = nullptr;
    ATOM atom_ = 0;
    HWND handle_ = nullptr;
    bool focused_ = false;
    bool loop_active_ = false;
    KeyboardInput input_;
    std::optional<InputKey> pending_key_;
    WindowPainter *painter_ = nullptr;
    WindowCommandHandler *command_handler_ = nullptr;
};

inline WindowCreateResult NativeWindow::create(const TextCatalog &catalog,
                                               std::string_view title_id)
{
    const auto title = read_text_utf16(catalog, title_id);
    if (!title.text)
        return {nullptr, title.error};
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
    if (!CreateWindowExW(0, MAKEINTRESOURCEW(owner->atom_), title.text->c_str(),
                         WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640,
                         480, nullptr, nullptr, owner->instance_, owner.get()))
        return {nullptr, GetLastError()};
    return {std::move(owner), ERROR_SUCCESS};
}
} // namespace closed_circuit::windows
