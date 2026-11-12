#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/profile_vectors.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include "platform/windows/profile_save.hpp"
#include <string_view>
using namespace closed_circuit;

const auto profile_name = L"profil \u017c\u00f3\u0142\u0107.dat";

int worker(const std::filesystem::path &directory, bool write,
           std::size_t index)
{
    const auto main = directory / profile_name;
    auto backup = main;
    backup += L".bak";
    const auto next = (index + 1) % 4;
    if (write &&
        (!windows::save_profile_file(main, test::profile_values[index]) ||
         !windows::save_profile_file(main, test::profile_values[next])))
        return 2;
    return windows::read_profile_file(main).profile ==
                       test::profile_values[next] &&
                   windows::read_profile_file(backup).profile ==
                       test::profile_values[index] &&
                   test::read_bytes(main) ==
                       std::vector<std::uint8_t>(
                           test::profile_vectors[next].begin(),
                           test::profile_vectors[next].end()) &&
                   test::read_bytes(backup) ==
                       std::vector<std::uint8_t>(
                           test::profile_vectors[index].begin(),
                           test::profile_vectors[index].end())
               ? 0
               : 2;
}

bool launch(const std::filesystem::path &directory, bool write,
            std::size_t index)
{
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) +
                   (write ? L"\" --write \"" : L"\" --read \"") +
                   directory.native() + L"\" " + std::to_wstring(index);
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION information{};
    if (!CreateProcessW(executable.data(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &information))
        return false;
    windows::FileHandle process(information.hProcess);
    windows::FileHandle thread(information.hThread);
    if (WaitForSingleObject(process.get(), 10000) != WAIT_OBJECT_0)
    {
        TerminateProcess(process.get(), 99);
        static_cast<void>(WaitForSingleObject(process.get(), 10000));
        return false;
    }
    DWORD code = 99;
    return GetExitCodeProcess(process.get(), &code) && code == 0;
}

bool cases()
{
    test::CheckpointDirectory fixture;
    const auto directory = fixture.path / L"\u0141\u00f3d\u017a profil";
    if (!std::filesystem::create_directory(directory))
        return false;
    const auto profile = directory / profile_name;
    const auto run = directory / L"run.dat";
    const std::array<std::filesystem::path, 4> profile_paths{
        profile, profile.native() + L".bak", profile.native() + L".tmp",
        profile.native() + L".bak.tmp"};
    const std::array<std::filesystem::path, 4> run_paths{
        run, run.native() + L".bak", run.native() + L".tmp",
        run.native() + L".bak.tmp"};
    for (std::size_t i = 0; i < 4; ++i)
    {
        if (!launch(directory, true, i) ||
            !windows::save_checkpoint_file(
                run, test::checkpoint_vectors[i].checkpoint) ||
            !windows::save_checkpoint_file(
                run, test::checkpoint_vectors[(i + 1) % 4].checkpoint))
            return false;
        test::write_bytes(run_paths[2], test::checkpoint_vectors[i].bytes);
        test::write_bytes(run_paths[3], test::checkpoint_vectors[i].bytes);
        test::write_bytes(profile_paths[2],
                          std::span{test::profile_vectors[i]}.first(9));
        test::write_bytes(profile_paths[3],
                          std::span{test::profile_vectors[i]}.first(7));
        std::array<std::vector<std::uint8_t>, 4> before;
        for (std::size_t n = 0; n < 4; ++n)
            before[n] = test::read_bytes(profile_paths[n]);
        if (!launch(directory, false, i))
            return false;
        // Delete only these explicit run names inside this fixture's private
        // directory. Never delete the shared directory or a computed glob.
        for (const auto &path : run_paths)
            if (!std::filesystem::remove(path))
                return false;
        if (!launch(directory, false, i))
            return false;
        for (std::size_t n = 0; n < 4; ++n)
            if (test::read_bytes(profile_paths[n]) != before[n] ||
                std::filesystem::exists(run_paths[n]))
                return false;
    }
    return true;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc == 4 && std::wstring_view(argv[3]).size() == 1 &&
        argv[3][0] >= L'0' && argv[3][0] <= L'3')
    {
        const std::wstring_view mode{argv[1]};
        if (mode == L"--write" || mode == L"--read")
            return worker(argv[2], mode == L"--write",
                          static_cast<std::size_t>(argv[3][0] - L'0'));
    }
    return argc == 1 && cases() ? 0 : 1;
}
