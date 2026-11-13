#include "platform/windows/native_window.hpp"
#include <thread>
using namespace closed_circuit;

int main()
{
    const TextRecord record{"title", "[AI PLACEHOLDER] Message test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    for (int repeat = 0; repeat < 4; ++repeat)
    {
        auto created = windows::NativeWindow::create(catalog, "title");
        if (!created.window || created.window->focused())
            return 1;
        auto &window = *created.window;
        const auto handle = window.handle();
        SetFocus(handle);
        if (GetFocus() != handle || !window.focused())
            return 2;
        SetFocus(nullptr);
        if (window.focused())
            return 3;
        // The pump must wait for a future message, rather than polling.
        std::jthread closer(
            [handle]
            {
                Sleep(30);
                PostMessageW(handle, WM_CLOSE, 0, 0);
            });
        const auto result = window.run_message_loop();
        if (result.error || result.exit_code || window.handle() ||
            window.focused() || IsWindow(handle))
            return 4;
        if (window.run_message_loop().error != ERROR_INVALID_WINDOW_HANDLE)
            return 5;
        // Exercise the real GetMessage -1 path with a destroyed HWND.
        const auto failed = windows::dispatch_messages(handle);
        if (failed.error != ERROR_INVALID_WINDOW_HANDLE || !failed.exit_code)
            return 6;
    }
    PostQuitMessage(37);
    const auto quit = windows::dispatch_messages();
    return quit.exit_code == 37 && quit.error == ERROR_SUCCESS ? 0 : 7;
}
