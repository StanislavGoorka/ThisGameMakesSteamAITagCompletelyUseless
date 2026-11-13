#include "application/text_build_policy.hpp"
#include "developer_text.hpp"
#include "platform/windows/native_window.hpp"
using namespace closed_circuit;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int show)
{
    try
    {
        const auto catalog = TextCatalog::from_records(
            std::span{&windows::developer_title_record, 1}, {},
            build_text_audience);
        if (!catalog)
            return 1;
        auto created =
            windows::NativeWindow::create(*catalog, "developer-window-title");
        if (!created.window)
            return 2;
        ShowWindow(created.window->handle(), show);
        const auto result = created.window->run_message_loop();
        return result.error ? 3 : result.exit_code;
    }
    catch (...)
    {
        return 4;
    }
}
