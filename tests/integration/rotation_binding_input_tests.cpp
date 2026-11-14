#include "platform/windows/native_window.hpp"
using namespace closed_circuit;
class Capture final : public windows::WindowCommandHandler
{
  public:
    unsigned calls = 0;
    std::uint32_t last = 0;
    bool consume = false;
    void key_available() noexcept override
    {
    }
    bool binding_key(std::uint32_t key) noexcept override
    {
        ++calls;
        last = key;
        return consume;
    }
};
int main()
{
    const TextRecord title{"title", "[AI PLACEHOLDER] Binding input test",
                           TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = TextCatalog::from_records(std::span{&title, 1}, {},
                                                   TextAudience::developer);
    auto created = windows::NativeWindow::create(*catalog, "title");
    if (!created.window)
        return 1;
    auto &window = *created.window;
    const auto hwnd = window.handle();
    const auto player = *NodeId::from_value(24);
    const auto tap = [hwnd](WPARAM key)
    {
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
    };
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    for (unsigned key = 'A'; key <= 'Z'; ++key)
    {
        const auto binding = RotationBinding::from_letter(key);
        if (!binding)
            continue;
        window.set_rotation_binding(*binding);
        tap(key);
        if (window.rotation_binding() != binding ||
            window.take_command(player) != Command{RotateCommand{player}})
            return 2;
        if (key != 'R')
        {
            tap('R');
            if (window.take_command(player))
                return 3;
        }
        tap('O');
        if (window.take_command(player) != Command{OverdriveCommand{player}})
            return 4;
    }
    Capture capture;
    capture.consume = true;
    window.set_command_handler(&capture);
    SendMessageW(hwnd, WM_KEYDOWN, 'T', 0);
    for (int i = 0; i < 100; ++i)
        SendMessageW(hwnd, WM_KEYDOWN, 'T', i % 2 ? 1LL << 30 : 0);
    if (capture.calls != 1 || capture.last != 'T' ||
        window.take_command(player))
        return 5;
    window.set_rotation_binding(*RotationBinding::from_letter('T'));
    capture.consume = false;
    SendMessageW(hwnd, WM_KEYDOWN, 'T', 0);
    if (window.take_command(player) || capture.calls != 1)
        return 6;
    SendMessageW(hwnd, WM_KEYUP, 'T', 0);
    tap('T');
    if (window.take_command(player) != Command{RotateCommand{player}} ||
        capture.calls != 2)
        return 7;
    SendMessageW(hwnd, WM_KEYDOWN, 'T', 0);
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    if (window.take_command(player))
        return 8;
    const auto before = capture.calls;
    tap('T');
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, 'T', 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, 'T', 0);
    if (capture.calls != before || window.take_command(player))
        return 9;
    tap('T');
    if (window.take_command(player) != Command{RotateCommand{player}})
        return 10;
    // Detach the borrowed callback before its lifetime ends.
    window.set_command_handler(nullptr);
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    return 0;
}
