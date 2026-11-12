#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/run_checks.hpp"
#include "platform/windows/checkpoint_recover.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>

using namespace closed_circuit;
namespace
{
bool cases()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"zapis \u0142\u00f3d\u017a";
    auto backup = main;
    backup += L".bak";
    auto staging = main;
    staging += L".tmp";
    auto backup_staging = backup;
    backup_staging += L".tmp";
    const auto &primary = test::checkpoint_vectors[0];
    auto bad = primary.bytes;
    bad[0] = 0;
    // Valid staging is ignored even when neither published file can be used.
    test::write_bytes(staging, primary.bytes);
    test::write_bytes(backup_staging, primary.bytes);
    const DWORD errors[]{ERROR_SUCCESS, ERROR_FILE_NOT_FOUND,
                         ERROR_INVALID_DATA, ERROR_SHARING_VIOLATION};
    for (const auto &v : test::checkpoint_vectors)
        for (const auto main_error : errors)
            for (const auto backup_error : errors)
            {
                std::filesystem::remove(main);
                std::filesystem::remove(backup);
                if (main_error != ERROR_FILE_NOT_FOUND)
                    test::write_bytes(main, main_error == ERROR_INVALID_DATA
                                                ? bad
                                                : primary.bytes);
                if (backup_error != ERROR_FILE_NOT_FOUND)
                    test::write_bytes(backup, backup_error == ERROR_INVALID_DATA
                                                  ? bad
                                                  : v.bytes);
                {
                    windows::FileHandle main_lock(
                        main_error == ERROR_SHARING_VIOLATION
                            ? CreateFileW(main.c_str(), GENERIC_READ, 0,
                                          nullptr, OPEN_EXISTING, 0, nullptr)
                            : INVALID_HANDLE_VALUE);
                    windows::FileHandle backup_lock(
                        backup_error == ERROR_SHARING_VIOLATION
                            ? CreateFileW(backup.c_str(), GENERIC_READ, 0,
                                          nullptr, OPEN_EXISTING, 0, nullptr)
                            : INVALID_HANDLE_VALUE);
                    if ((main_error == ERROR_SHARING_VIOLATION && !main_lock) ||
                        (backup_error == ERROR_SHARING_VIOLATION &&
                         !backup_lock))
                        return false;
                    for (int repeat = 0; repeat < 2; ++repeat)
                    {
                        const auto result =
                            windows::recover_checkpoint_file(main);
                        const bool fallback =
                            main_error == ERROR_FILE_NOT_FOUND ||
                            main_error == ERROR_INVALID_DATA;
                        const auto expected =
                            main_error == ERROR_SUCCESS
                                ? std::optional{primary.checkpoint}
                            : fallback && backup_error == ERROR_SUCCESS
                                ? std::optional{v.checkpoint}
                                : std::nullopt;
                        const auto source =
                            main_error == ERROR_SUCCESS
                                ? windows::CheckpointSource::main
                            : expected ? windows::CheckpointSource::backup
                                       : windows::CheckpointSource::none;
                        if (result.checkpoint != expected ||
                            result.source != source ||
                            result.main_error != main_error ||
                            result.backup_error !=
                                (fallback ? std::optional{backup_error}
                                          : std::nullopt))
                            return false;
                        if (expected)
                        {
                            const auto restored =
                                restore_checkpoint(*result.checkpoint);
                            const auto original = restore_checkpoint(*expected);
                            if (!restored || !original ||
                                !test::same_run(*restored, *original))
                                return false;
                        }
                    }
                }
                // Check exact bytes of every file after handles close,
                // including corrupt files, valid backup, and both ignored
                // staging paths.
                for (const auto &[path, error, bytes] :
                     {std::tuple{main, main_error, primary.bytes},
                      std::tuple{backup, backup_error, v.bytes}})
                {
                    if (error == ERROR_FILE_NOT_FOUND)
                    {
                        if (std::filesystem::exists(path))
                            return false;
                    }
                    else if (!std::ranges::equal(
                                 test::read_bytes(path),
                                 error == ERROR_INVALID_DATA ? bad : bytes))
                        return false;
                }
                if (!std::ranges::equal(test::read_bytes(staging),
                                        primary.bytes) ||
                    !std::ranges::equal(test::read_bytes(backup_staging),
                                        primary.bytes))
                    return false;
            }
    const auto missing =
        windows::recover_checkpoint_file(directory.path / L"missing" / L"run");
    return !missing.checkpoint &&
           missing.source == windows::CheckpointSource::none &&
           missing.main_error == ERROR_PATH_NOT_FOUND &&
           missing.backup_error == static_cast<DWORD>(ERROR_PATH_NOT_FOUND);
}
} // namespace
int main()
{
    try
    {
        if (cases())
            return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << '\n';
    }
    std::cerr << "Checkpoint recovery mismatch\n";
    return 1;
}
