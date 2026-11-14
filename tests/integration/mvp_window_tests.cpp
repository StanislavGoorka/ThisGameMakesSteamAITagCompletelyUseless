#include "../support/mvp_window.hpp"
using namespace closed_circuit;
bool scenario(const std::filesystem::path &root)
{
    test::CheckpointDirectory owned;
    test::MvpWindow launch(root, owned.path);
    auto run = start_run(RunSeed{1});
    if (launch.resume.status != ResumeStatus::none ||
        !test::mvp_files(owned.path).empty() ||
        launch.session->screen() != SessionScreen::start)
        return test::mvp_fail("fresh launch");
    launch.tap(VK_RETURN);
    if (!launch.matches(run) || !launch.checkpoint(run))
        return test::mvp_fail("start boundary");
    for (int sector = 1; sector <= sector_count; ++sector)
    {
        if (run.sector_number.value() != sector || !launch.quiet(run) ||
            !launch.solve(run))
            return test::mvp_fail("sector witness");
        if (sector < sector_count && !launch.advance(run))
            return false;
    }
    const LocalProfile expected{31};
    if (run_phase(run) != RunPhase::won ||
        launch.session->screen() != SessionScreen::result ||
        windows::read_profile_file(owned.path / L"profile.bin").profile !=
            expected)
        return test::mvp_fail("won profile");
    const auto bytes = test::read_bytes(owned.path / L"profile.bin");
    const auto backup = test::read_bytes(owned.path / L"profile.bin.bak");
    launch.tap(VK_RETURN);
    return launch.matches(start_run(run.seed)) &&
           launch.checkpoint(start_run(run.seed)) &&
           test::read_bytes(owned.path / L"profile.bin") == bytes &&
           test::read_bytes(owned.path / L"profile.bin.bak") == backup;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    bool passed = false;
    try
    {
        passed = scenario(argv[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    CoUninitialize();
    return passed ? 0 : 2;
}
