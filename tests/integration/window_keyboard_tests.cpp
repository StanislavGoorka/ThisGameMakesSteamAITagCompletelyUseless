#include "platform/windows/native_window.hpp"
using namespace closed_circuit;

int main()
{
    const TextRecord record{"title", "[AI PLACEHOLDER] Keyboard test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto created = windows::NativeWindow::create(catalog, "title");
    if (!created.window)
        return 1;
    auto &window = *created.window;
    const auto handle = window.handle();
    const auto player = *NodeId::from_value(24);
    SendMessageW(handle, WM_KEYDOWN, VK_SPACE, 1);
    if (window.take_command(player))
        return 2;
    SetFocus(handle);
    const WPARAM keys[]{VK_UP, VK_RIGHT, VK_DOWN, VK_LEFT, 'R', 'O', VK_SPACE};
    const Command expected[]{MoveCommand{*NodeId::from_value(17)},
                             MoveCommand{*NodeId::from_value(25)},
                             MoveCommand{*NodeId::from_value(31)},
                             MoveCommand{*NodeId::from_value(23)},
                             RotateCommand{player},
                             OverdriveCommand{player},
                             WaitCommand{}};
    for (unsigned i = 0; i < 7; ++i)
    {
        SendMessageW(handle, WM_KEYDOWN, keys[i], 5);
        if (window.take_command(player) != expected[i] ||
            window.take_command(player))
            return 3;
        for (int repeat = 0; repeat < 100; ++repeat)
        {
            SendMessageW(handle, WM_KEYDOWN, keys[i], (1LL << 30) | 1);
            SendMessageW(handle, WM_KEYDOWN, keys[i], 1);
            if (window.take_command(player))
                return 4;
        }
        SendMessageW(handle, WM_KEYUP, keys[i], 1);
        SendMessageW(handle, WM_KEYDOWN, keys[i], 1);
        if (window.take_command(player) != expected[i])
            return 11;
        SendMessageW(handle, WM_KEYUP, keys[i], 1);
        SendMessageW(handle, WM_KEYDOWN, keys[i], 1);
        SetFocus(nullptr); // Drop an unconsumed action.
        SetFocus(handle);
        if (window.take_command(player))
            return 5;
        SendMessageW(handle, WM_KEYDOWN, keys[i], (1LL << 30) | 1);
        if (window.take_command(player))
            return 6;
        SendMessageW(handle, WM_KEYUP, keys[i], 1);
    }
    SendMessageW(handle, WM_KEYDOWN, VK_SPACE, 1);
    SendMessageW(handle, WM_KEYDOWN, 'R', 1);
    if (window.take_command(player) != Command{WaitCommand{}} ||
        window.take_command(player))
        return 7;
    SendMessageW(handle, WM_KEYUP, 'R', 1);
    SendMessageW(handle, WM_KEYDOWN, 'R', 1);
    SendMessageW(handle, WM_CLOSE, 0, 0);
    if (window.take_command(player))
        return 8;
    auto other = windows::NativeWindow::create(catalog, "title");
    if (!other.window)
        return 9;
    SetFocus(other.window->handle());
    SendMessageW(other.window->handle(), WM_KEYDOWN, 'Z', 1);
    SendMessageW(other.window->handle(), WM_KEYUP, 'Z', 1);
    if (other.window->take_command(player))
        return 10;
    SendMessageW(other.window->handle(), WM_KEYDOWN, VK_UP, 1);
    if (other.window->take_command(*NodeId::from_value(0)) ||
        other.window->take_command(player))
        return 12;
    return 0;
}
