#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/run_checks.hpp"
#include "platform/windows/checkpoint_resume.hpp"
#include <map>

using namespace closed_circuit;
namespace
{
using Files = std::map<std::filesystem::path, std::vector<std::uint8_t>>;

Files snapshot(const std::filesystem::path &directory)
{
    Files files;
    for (const auto &entry : std::filesystem::directory_iterator(directory))
        files[entry.path().filename()] = test::read_bytes(entry.path());
    return files;
}

bool resumes(const windows::CheckpointResume &result,
             const RunCheckpoint &checkpoint)
{
    const auto expected = restore_checkpoint(checkpoint);
    return result.status == ResumeStatus::resumed && result.run && expected &&
           test::same_run(*result.run, *expected) &&
           result.run->sector.turns_taken == TurnIndex{0};
}

bool classification()
{
    using windows::CheckpointSource;
    const auto &v = test::checkpoint_vectors;
    for (const auto &item : v)
        for (const auto &recovery :
             {windows::CheckpointRecoveryResult{item.checkpoint,
                                                CheckpointSource::main,
                                                ERROR_SUCCESS, std::nullopt},
              windows::CheckpointRecoveryResult{
                  item.checkpoint, CheckpointSource::backup,
                  ERROR_FILE_NOT_FOUND, ERROR_SUCCESS},
              windows::CheckpointRecoveryResult{
                  item.checkpoint, CheckpointSource::backup, ERROR_INVALID_DATA,
                  ERROR_SUCCESS}})
            if (!resumes(windows::classify_checkpoint_recovery(recovery),
                         item.checkpoint))
                return false;
    struct Case
    {
        DWORD main;
        std::optional<DWORD> backup;
        ResumeStatus expected;
    };
    const Case cases[]{
        {ERROR_FILE_NOT_FOUND, ERROR_FILE_NOT_FOUND, ResumeStatus::none},
        {ERROR_PATH_NOT_FOUND, ERROR_PATH_NOT_FOUND, ResumeStatus::none},
        {ERROR_FILE_NOT_FOUND, ERROR_PATH_NOT_FOUND, ResumeStatus::none},
        {ERROR_INVALID_DATA, ERROR_FILE_NOT_FOUND, ResumeStatus::corrupt},
        {ERROR_FILE_NOT_FOUND, ERROR_INVALID_DATA, ResumeStatus::corrupt},
        {ERROR_INVALID_DATA, ERROR_INVALID_DATA, ResumeStatus::corrupt},
        {ERROR_SHARING_VIOLATION, std::nullopt, ResumeStatus::unavailable},
        {ERROR_ACCESS_DENIED, std::nullopt, ResumeStatus::unavailable},
        {ERROR_INVALID_DATA, ERROR_SHARING_VIOLATION,
         ResumeStatus::unavailable},
        {ERROR_FILE_NOT_FOUND, ERROR_ACCESS_DENIED, ResumeStatus::unavailable},
        {ERROR_INVALID_DATA, ERROR_NOT_READY, ResumeStatus::unavailable}};
    for (const auto &c : cases)
    {
        const auto result = windows::classify_checkpoint_recovery(
            {std::nullopt, CheckpointSource::none, c.main, c.backup});
        if (result.status != c.expected || result.run)
            return false;
    }
    // A value that cannot be restored is never presented as a resumed run.
    auto invalid = v[0].checkpoint;
    invalid.player_integrity = 0;
    const auto rejected = windows::classify_checkpoint_recovery(
        {invalid, CheckpointSource::main, ERROR_SUCCESS, std::nullopt});
    return rejected.status == ResumeStatus::corrupt && !rejected.run;
}

bool files()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"zapis \u017c";
    auto backup = main;
    backup += L".bak";
    const auto &v = test::checkpoint_vectors;
    auto bad = v[0].bytes;
    bad[39] ^= 1;
    const auto reads =
        [&](ResumeStatus expected, const RunCheckpoint *checkpoint)
    {
        for (int repeat = 0; repeat < 2; ++repeat)
        {
            const auto result = windows::resume_checkpoint_file(main);
            if (checkpoint ? !resumes(result, *checkpoint)
                           : result.status != expected || result.run)
                return false;
        }
        return true;
    };
    // Snapshots are taken outside any exclusive lock held by the test.
    const auto check =
        [&](ResumeStatus expected, const RunCheckpoint *checkpoint)
    {
        const auto before = snapshot(directory.path);
        return reads(expected, checkpoint) &&
               snapshot(directory.path) == before;
    };
    if (!check(ResumeStatus::none, nullptr))
        return false;
    test::write_bytes(main, v[0].bytes);
    if (!check(ResumeStatus::resumed, &v[0].checkpoint))
        return false;
    test::write_bytes(backup, v[3].bytes);
    if (!check(ResumeStatus::resumed, &v[0].checkpoint))
        return false;
    test::write_bytes(main, bad);
    if (!check(ResumeStatus::resumed, &v[3].checkpoint))
        return false;
    std::filesystem::remove(backup);
    if (!check(ResumeStatus::corrupt, nullptr))
        return false;
    std::filesystem::remove(main);
    test::write_bytes(backup, bad);
    if (!check(ResumeStatus::corrupt, nullptr))
        return false;
    test::write_bytes(main, v[1].bytes);
    test::write_bytes(backup, v[2].bytes);
    const auto unlocked = snapshot(directory.path);
    {
        windows::FileHandle lock(CreateFileW(
            main.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!lock || !reads(ResumeStatus::unavailable, nullptr))
            return false;
    }
    if (snapshot(directory.path) != unlocked)
        return false;
    if (!check(ResumeStatus::resumed, &v[1].checkpoint))
        return false;
    const auto missing =
        windows::resume_checkpoint_file(directory.path / L"none" / L"main");
    return missing.status == ResumeStatus::none && !missing.run &&
           !std::filesystem::exists(directory.path / L"none");
}
} // namespace

int main()
{
    try
    {
        if (!classification())
            return 1;
        return files() ? 0 : 2;
    }
    catch (...)
    {
        return 3;
    }
}
