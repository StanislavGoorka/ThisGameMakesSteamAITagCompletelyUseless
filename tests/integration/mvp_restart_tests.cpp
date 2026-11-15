#include "../support/mvp_window.hpp"
using namespace closed_circuit;
namespace
{
bool begin(const std::filesystem::path &root, const std::filesystem::path &dir)
{
    test::MvpWindow window(root, dir);
    auto run = start_run(RunSeed{1});
    if (window.resume.status != ResumeStatus::none ||
        !test::mvp_files(dir).empty())
        return test::mvp_fail("worker fresh");
    window.tap(VK_RETURN);
    if (!window.solve(run) || !window.advance(run))
        return false;
    const auto saved = test::mvp_files(dir);
    return window.apply(run, WaitCommand{}, VK_SPACE) &&
           window.apply(run, WaitCommand{}, VK_SPACE) &&
           run.sector.turns_taken == TurnIndex{2} &&
           test::mvp_files(dir) == saved;
}
bool finish(const std::filesystem::path &root, const std::filesystem::path &dir)
{
    const auto saved = test::mvp_files(dir);
    const auto checkpoint =
        windows::read_checkpoint_file(dir / L"checkpoint.bin");
    const auto expected = checkpoint.checkpoint
                              ? restore_checkpoint(*checkpoint.checkpoint)
                              : std::nullopt;
    // Sector 1 was earned by the first process; its profile is on disk.
    const auto earlier = windows::read_profile_file(dir / L"profile.bin");
    test::MvpWindow window(root, dir);
    if (!expected || !earlier.profile ||
        window.resume.status != ResumeStatus::resumed ||
        !window.matches(*expected) || test::mvp_files(dir) != saved ||
        expected->sector_number.value() != 2 ||
        expected->sector.turns_taken != TurnIndex{0})
        return test::mvp_fail("worker resume boundary");
    auto run = *expected;
    if (!window.quiet(run) || !window.solve(run) || window.notices.pending())
        return test::mvp_fail("worker continuation");
    // Finishing sector 2 adds nothing but a newly earned swift_sector; the
    // checkpoint stays as saved.
    auto continued = test::mvp_files(dir);
    const auto profile = windows::read_profile_file(dir / L"profile.bin");
    const bool swift_new =
        window.swift != 0 && (earlier.profile->achievement_bits & 0x20) == 0;
    if (swift_new)
    {
        if (!profile.profile || profile.profile->achievement_bits !=
                                    (earlier.profile->achievement_bits | 0x20))
            return test::mvp_fail("worker swift");
        for (const auto *name : {L"profile.bin", L"profile.bin.bak"})
            continued[name] = saved.at(name);
    }
    if (continued != saved)
        return test::mvp_fail("worker continuation files");
    for (int number = 3; number <= sector_count; ++number)
        if (!window.advance(run) || !window.solve(run) ||
            run.sector_number.value() != number)
            return test::mvp_fail("worker suffix");
    // Restored history: all MVP IDs but run_without_overdrive, plus swift
    // when any sector of either process completed within 16 turns. The last
    // write adds completed_run.
    const auto mask = static_cast<std::uint16_t>(
        27 | earlier.profile->achievement_bits | window.swift);
    return run_phase(run) == RunPhase::won &&
           window.session->screen() == SessionScreen::result &&
           (earlier.profile->achievement_bits & ~0x2b) == 0 &&
           windows::read_profile_file(dir / L"profile.bin").profile ==
               LocalProfile{mask} &&
           windows::read_profile_file(dir / L"profile.bin.bak").profile ==
               LocalProfile{static_cast<std::uint16_t>(mask & ~16)};
}
bool read_only(const std::filesystem::path &root,
               const std::filesystem::path &dir)
{
    const auto before = test::mvp_files(dir);
    test::MvpWindow window(root, dir);
    return window.resume.status == ResumeStatus::none &&
           window.session->screen() == SessionScreen::start &&
           (windows::read_profile_file(dir / L"profile.bin")
                .profile.value_or(LocalProfile{0, 4})
                .achievement_bits |
            0x20) == 0x3b &&
           test::mvp_files(dir) == before;
}
bool launch(std::wstring_view mode, const std::filesystem::path &root,
            const std::filesystem::path &dir)
{
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (!length || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) + L"\" " +
                   std::wstring(mode) + L" \"" + root.native() + L"\" \"" +
                   dir.native() + L"\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION info{};
    if (!CreateProcessW(executable.data(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &info))
        return false;
    windows::FileHandle process(info.hProcess), thread(info.hThread);
    if (WaitForSingleObject(process.get(), 20000) != WAIT_OBJECT_0)
    {
        TerminateProcess(process.get(), 99);
        static_cast<void>(WaitForSingleObject(process.get(), 5000));
        return false;
    }
    DWORD code = 99;
    return GetExitCodeProcess(process.get(), &code) && code == 0;
}
bool scenario(const std::filesystem::path &root)
{
    test::CheckpointDirectory owned;
    const auto reference = owned.path / L"reference";
    const auto restarted = owned.path / L"restart ż";
    std::filesystem::create_directory(reference);
    std::filesystem::create_directory(restarted);
    test::MvpFiles boundary;
    {
        // Independent uninterrupted HWND run supplies exact boundary bytes.
        test::MvpWindow window(root, reference);
        auto run = start_run(RunSeed{1});
        window.tap(VK_RETURN);
        if (!window.solve(run) || !window.advance(run))
            return false;
        boundary = test::mvp_files(reference);
    }
    if (!launch(L"--begin", root, restarted) ||
        test::mvp_files(restarted) != boundary ||
        !launch(L"--finish", root, restarted))
        return test::mvp_fail("process boundary or continuation");
    const auto ended = test::mvp_files(restarted);
    return ended.size() == 2 && launch(L"--read", root, restarted) &&
           test::mvp_files(restarted) == ended;
}
} // namespace
int wmain(int argc, wchar_t **argv)
{
    if ((argc != 2 && argc != 4) ||
        FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    bool passed = false;
    try
    {
        if (argc == 2)
            passed = scenario(argv[1]);
        else if (std::wstring_view(argv[1]) == L"--begin")
            passed = begin(argv[2], argv[3]);
        else if (std::wstring_view(argv[1]) == L"--finish")
            passed = finish(argv[2], argv[3]);
        else if (std::wstring_view(argv[1]) == L"--read")
            passed = read_only(argv[2], argv[3]);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    CoUninitialize();
    return passed ? 0 : 2;
}
