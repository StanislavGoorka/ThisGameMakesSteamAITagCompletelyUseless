#include "application/text_build_policy.hpp"
#include "core/run_start.hpp"
#include "developer_text.hpp"
#include "platform/windows/run_window.hpp"
using namespace closed_circuit;

struct ComApartment
{
    HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ~ComApartment()
    {
        if (SUCCEEDED(result))
            CoUninitialize();
    }
};
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int show)
{
    try
    {
        ComApartment apartment;
        if (FAILED(apartment.result))
            return 5;
        const auto catalog = TextCatalog::from_records(
            windows::developer_text_records, {}, build_text_audience);
        if (!catalog)
            return 1;
        auto created =
            windows::NativeWindow::create(*catalog, "developer-window-title");
        if (!created.window)
            return 2;
        const auto run = start_run(RunSeed{0});
        const AssetManifest assets{presentation::atlas_reference()};
        // CMake supplies a UTF-8 checkout path; launching from another cwd
        // works.
        const std::filesystem::path root{CLOSED_CIRCUIT_CONTENT_ROOT};
        auto board = windows::RunWindow::create(*created.window, run, root,
                                                assets, {}, &*catalog);
        if (!board.session)
            return 6;
        ShowWindow(created.window->handle(), show);
        UpdateWindow(created.window->handle());
        const auto result = created.window->run_message_loop();
        if (FAILED(board.session->error()))
            return 7;
        return result.error ? 3 : result.exit_code;
    }
    catch (...)
    {
        return 4;
    }
}
