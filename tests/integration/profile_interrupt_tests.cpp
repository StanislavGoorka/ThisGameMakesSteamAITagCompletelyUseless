#include "../support/checkpoint_files.hpp"
#include "../support/profile_vectors.hpp"
#include "platform/windows/profile_read.hpp"
#include <algorithm>
#include <iostream>
#include <string_view>

namespace
{
BOOL WINAPI observed_write(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL WINAPI observed_flush(HANDLE);
BOOL WINAPI observed_move(LPCWSTR, LPCWSTR, DWORD);
} // namespace
// Test-only syscall interception in this single translation unit. All normal
// calls forward to Win32. Worker calls the real save function, not a copy of
// its protocol. Production headers expose no test hooks or interruption
// callbacks.
#define WriteFile observed_write
#define FlushFileBuffers observed_flush
#define MoveFileExW observed_move
#include "platform/windows/profile_save.hpp"
#undef MoveFileExW
#undef FlushFileBuffers
#undef WriteFile

using namespace closed_circuit;
namespace
{
HANDLE ready_event = nullptr;
int stop_phase = -1;
std::filesystem::path stop_destination;

[[noreturn]] void pause_for_kill()
{
    if (!SetEvent(ready_event))
        ExitProcess(3);
    // Parent has a 10s bounded wait. An orphan worker exits on its own.
    Sleep(20000);
    ExitProcess(3);
}
BOOL WINAPI observed_write(HANDLE file, LPCVOID bytes, DWORD size,
                           LPDWORD count, LPOVERLAPPED overlapped)
{
    if (ready_event && stop_phase == 0 && size == 24)
    {
        // Simulate interruption with a real partial file and still-open handle.
        const auto ok = WriteFile(file, bytes, 9, count, overlapped);
        if (ok && *count == 9)
            pause_for_kill();
        return ok;
    }
    return WriteFile(file, bytes, size, count, overlapped);
}
BOOL WINAPI observed_flush(HANDLE file)
{
    const auto ok = FlushFileBuffers(file);
    if (ok && ready_event && stop_phase == 1)
        pause_for_kill();
    return ok;
}
BOOL WINAPI observed_move(LPCWSTR from, LPCWSTR to, DWORD flags)
{
    const auto ok = MoveFileExW(from, to, flags);
    if (ok && ready_event && (stop_phase == 2 || stop_phase == 3) &&
        std::filesystem::path(to) == stop_destination)
        pause_for_kill();
    return ok;
}

const auto run_name = L"zapis_\u017c\u00f3\u0142\u0107";
int worker(const std::filesystem::path &directory, int phase, std::size_t index,
           const wchar_t *event_name)
{
    windows::FileHandle event(
        OpenEventW(EVENT_MODIFY_STATE, FALSE, event_name));
    if (!event)
        return 2;
    ready_event = event.get();
    stop_phase = phase;
    const auto main = directory / run_name;
    stop_destination = main;
    if (phase == 2)
        stop_destination += L".bak";
    return windows::save_profile_file(
               main, test::profile_file_vectors[(index + 1) % 4].profile)
               ? 0
               : 2;
}

bool kill_at_phase(const std::filesystem::path &directory, int phase,
                   std::size_t index)
{
    const auto event_name = L"Local\\ClosedCircuit_profile_" +
                            std::to_wstring(GetCurrentProcessId()) + L"_" +
                            std::to_wstring(phase) + L"_" +
                            std::to_wstring(index);
    windows::FileHandle event(
        CreateEventW(nullptr, TRUE, FALSE, event_name.c_str()));
    if (!event || GetLastError() == ERROR_ALREADY_EXISTS)
        return false;
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) + L"\" --worker \"" +
                   directory.native() + L"\" " + std::to_wstring(phase) + L" " +
                   std::to_wstring(index) + L" \"" + event_name + L"\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION information{};
    if (!CreateProcessW(executable.data(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &information))
        return false;
    windows::FileHandle process(information.hProcess);
    windows::FileHandle thread(information.hThread);
    const HANDLE handles[]{event.get(), process.get()};
    const auto reached = WaitForMultipleObjects(2, handles, FALSE, 10000);
    const auto killed = TerminateProcess(process.get(), 99);
    const auto ended = WaitForSingleObject(process.get(), 10000);
    DWORD code = 0;
    return reached == WAIT_OBJECT_0 && killed && ended == WAIT_OBJECT_0 &&
           GetExitCodeProcess(process.get(), &code) && code == 99;
}

bool matches(const std::filesystem::path &path, const test::ProfileVector &v)
{
    const auto result = windows::read_profile_file(path);
    if (result.profile != v.profile ||
        !std::ranges::equal(test::read_bytes(path), v.bytes))
        return false;
    return true;
}
bool cases()
{
    test::CheckpointDirectory fixture;
    const auto directory = fixture.path / L"\u0141\u00f3d\u017a test";
    if (!std::filesystem::create_directory(directory))
        return false;
    const auto main = directory / run_name;
    auto backup = main;
    backup += L".bak";
    auto staging = main;
    staging += L".tmp";
    for (std::size_t index = 0; index < 4; ++index)
    {
        const auto &old = test::profile_file_vectors[index];
        const auto &next = test::profile_file_vectors[(index + 1) % 4];
        const auto &older = test::profile_file_vectors[(index + 2) % 4];
        for (int phase = 0; phase < 4; ++phase)
        {
            test::write_bytes(main, old.bytes);
            test::write_bytes(backup, older.bytes);
            if (!kill_at_phase(directory, phase, index) ||
                !matches(main, phase == 3 ? next : old) ||
                !matches(backup, phase >= 2 ? old : older))
                return false;
            if (phase == 0 &&
                !std::ranges::equal(test::read_bytes(staging),
                                    std::span{next.bytes}.first(9)))
                return false;
            if ((phase == 1 || phase == 2) && !matches(staging, next))
                return false;
            if (phase == 3 && std::filesystem::exists(staging))
                return false;
            if (!windows::save_profile_file(main, next.profile) ||
                !matches(main, next) ||
                !matches(backup, phase == 3 ? next : old) ||
                std::filesystem::exists(staging))
                return false;
        }
    }
    const auto &a = test::profile_file_vectors[0];
    const auto &b = test::profile_file_vectors[1];
    test::write_bytes(main, a.bytes);
    test::write_bytes(backup, b.bytes);
    // An unusable staging path is a real filesystem error, not a mock result.
    if (!std::filesystem::create_directory(staging) ||
        windows::save_profile_file(main, b.profile) || !matches(main, a) ||
        !matches(backup, b) || !std::filesystem::remove(staging) ||
        windows::save_profile_file(directory / L"missing" / L"run",
                                   b.profile) ||
        !windows::save_profile_file(main, b.profile) || !matches(main, b) ||
        !matches(backup, a))
        return false;
    std::cout << "profile_interruptions 16\n";
    return true;
}
} // namespace
int wmain(int argc, wchar_t **argv)
{
    try
    {
        if (argc == 6 && std::wstring_view(argv[1]) == L"--worker" &&
            std::wstring_view(argv[3]).size() == 1 && argv[3][0] >= L'0' &&
            argv[3][0] <= L'3' && std::wstring_view(argv[4]).size() == 1 &&
            argv[4][0] >= L'0' && argv[4][0] <= L'3')
            return worker(argv[2], argv[3][0] - L'0',
                          static_cast<std::size_t>(argv[4][0] - L'0'), argv[5]);
        if (argc == 1 && cases())
            return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    std::cerr << "Profile interruption mismatch\n";
    return 1;
}
