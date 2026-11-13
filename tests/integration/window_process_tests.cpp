#include "platform/windows/file_handle.hpp"
#include <iostream>
#include <string>
using namespace closed_circuit;

struct WindowSearch
{
    DWORD process;
    HWND window = nullptr;
};
BOOL CALLBACK find_window(HWND handle, LPARAM parameter)
{
    auto &search = *reinterpret_cast<WindowSearch *>(parameter);
    DWORD process = 0;
    GetWindowThreadProcessId(handle, &process);
    if (process == search.process && IsWindowVisible(handle))
    {
        search.window = handle;
        return FALSE;
    }
    return TRUE;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        std::wstring command = L"\"" + std::wstring(argv[1]) + L"\"";
        STARTUPINFOW startup{};
        startup.cb = sizeof(startup);
        PROCESS_INFORMATION info{};
        if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, 0,
                            nullptr, nullptr, &startup, &info))
            return 2;
        windows::FileHandle process(info.hProcess), thread(info.hThread);
        WindowSearch search{info.dwProcessId};
        const auto deadline = GetTickCount64() + 5000;
        while (!search.window && GetTickCount64() < deadline &&
               WaitForSingleObject(process.get(), 0) == WAIT_TIMEOUT)
        {
            EnumWindows(find_window, reinterpret_cast<LPARAM>(&search));
            if (!search.window)
                Sleep(10);
        }
        bool valid = search.window != nullptr;
        if (valid)
        {
            wchar_t title[256]{};
            GetWindowTextW(search.window, title, 256);
            valid = std::wstring(title) == L"[AI PLACEHOLDER] Closed Circuit";
            // Sent focus notifications exercise dispatch without stealing the
            // user's foreground window from this separate process.
            SendMessageW(search.window, WM_SETFOCUS, 0, 0);
            SendMessageW(search.window, WM_KILLFOCUS, 0, 0);
            valid = PostMessageW(search.window, WM_CLOSE, 0, 0) && valid;
        }
        if (WaitForSingleObject(process.get(), 5000) != WAIT_OBJECT_0)
        {
            TerminateProcess(process.get(), 99);
            WaitForSingleObject(process.get(), 5000);
            return 3;
        }
        DWORD code = 99;
        if (!valid || !GetExitCodeProcess(process.get(), &code) || code ||
            IsWindow(search.window))
        {
            std::cerr << "Smoke failed: valid=" << valid << " exit=" << code
                      << " live=" << IsWindow(search.window) << '\n';
            return 4;
        }
    }
    return 0;
}
