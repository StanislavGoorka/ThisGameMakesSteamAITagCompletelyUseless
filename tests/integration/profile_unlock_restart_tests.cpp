#include "../support/checkpoint_files.hpp"
#include "platform/windows/profile_unlock.hpp"
#include <string_view>
using namespace closed_circuit;

LocalProfile expected(unsigned count)
{
    return {static_cast<std::uint16_t>((1u << count) - 1), 3, 19, true};
}

int worker(std::wstring_view mode, const std::filesystem::path &path,
           unsigned count)
{
    using Result = windows::ProfileFileUnlockResult;
    if (mode == L"--bad")
        return windows::unlock_profile_file(
                   path, AchievementId::first_sector) == Result::read_error
                   ? 0
                   : 2;
    if (mode == L"--new" || mode == L"--again")
    {
        if (count == 0 || windows::unlock_profile_file(
                              path, static_cast<AchievementId>(count)) !=
                              (mode == L"--new" ? Result::unlocked
                                                : Result::already_unlocked))
            return 2;
    }
    else if (mode != L"--read")
        return 2;
    return windows::read_profile_file(path).profile == expected(count) ? 0 : 2;
}

bool launch(std::wstring_view mode, const std::filesystem::path &path,
            unsigned count)
{
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) + L"\" " +
                   std::wstring(mode) + L" \"" + path.native() + L"\" " +
                   std::to_wstring(count);
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

bool matches(const std::filesystem::path &path, const LocalProfile &profile)
{
    const auto bytes = *encode_profile(profile);
    return windows::read_profile_file(path).profile == profile &&
           test::read_bytes(path) ==
               std::vector<std::uint8_t>(bytes.begin(), bytes.end());
}

bool cases()
{
    test::CheckpointDirectory fixture;
    const auto path = fixture.path / L"profil \u0141\u00f3d\u017a.dat";
    const auto backup = std::filesystem::path(path.native() + L".bak");
    if (!windows::save_profile_file(path, expected(0)) ||
        !launch(L"--read", path, 0))
        return false;
    for (unsigned count = 1; count <= 5; ++count)
    {
        if (!launch(L"--new", path, count) || !launch(L"--read", path, count) ||
            !matches(path, expected(count)) ||
            !matches(backup, expected(count - 1)))
            return false;
        const auto before = test::read_bytes(path);
        const auto before_backup = test::read_bytes(backup);
        if (!launch(L"--again", path, count) ||
            !launch(L"--read", path, count) ||
            test::read_bytes(path) != before ||
            test::read_bytes(backup) != before_backup)
            return false;
    }
    const auto good = test::read_bytes(path);
    const auto old_backup = test::read_bytes(backup);
    const std::array<std::uint8_t, 3> broken{1, 2, 3};
    test::write_bytes(path, broken);
    if (!launch(L"--bad", path, 0) ||
        test::read_bytes(path) !=
            std::vector<std::uint8_t>(broken.begin(), broken.end()) ||
        test::read_bytes(backup) != old_backup)
        return false;
    test::write_bytes(path, good);
    {
        windows::FileHandle lock(CreateFileW(path.c_str(), GENERIC_READ, 0,
                                             nullptr, OPEN_EXISTING,
                                             FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!lock || !launch(L"--bad", path, 0))
            return false;
    }
    return test::read_bytes(path) == good &&
           test::read_bytes(backup) == old_backup && launch(L"--read", path, 5);
}

int wmain(int argc, wchar_t **argv)
{
    if (argc == 4 && std::wstring_view(argv[3]).size() == 1 &&
        argv[3][0] >= L'0' && argv[3][0] <= L'5')
        return worker(argv[1], argv[2],
                      static_cast<unsigned>(argv[3][0] - L'0'));
    return argc == 1 && cases() ? 0 : 1;
}
