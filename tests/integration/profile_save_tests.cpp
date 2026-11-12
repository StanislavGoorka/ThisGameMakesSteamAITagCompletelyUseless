#include "../support/checkpoint_files.hpp"
#include "../support/profile_vectors.hpp"
#include "platform/windows/profile_save.hpp"
using namespace closed_circuit;

bool matches(const std::filesystem::path &path, std::size_t i)
{
    return test::read_bytes(path) ==
               std::vector<std::uint8_t>(test::profile_vectors[i].begin(),
                                         test::profile_vectors[i].end()) &&
           windows::read_profile_file(path).profile == test::profile_values[i];
}

int main()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"profil \u017c\u00f3\u0142\u0107.dat";
    auto backup = main;
    backup += L".bak";
    auto staging = main;
    staging += L".tmp";
    auto backup_staging = backup;
    backup_staging += L".tmp";
    for (std::size_t i = 0; i < test::profile_values.size(); ++i)
    {
        if (!windows::save_profile_file(main, test::profile_values[i]) ||
            !matches(main, i) || std::filesystem::exists(staging) ||
            std::filesystem::exists(backup_staging) ||
            (i == 0 ? std::filesystem::exists(backup)
                    : !matches(backup, i - 1)))
            return 1;
    }

    // Each publication path can be blocked; release and retry must work.
    for (const auto &locked_path : {main, staging, backup, backup_staging})
    {
        test::write_bytes(main, test::profile_vectors[1]);
        test::write_bytes(backup, test::profile_vectors[0]);
        test::write_bytes(staging,
                          std::span{test::profile_vectors[3]}.first(7));
        test::write_bytes(backup_staging,
                          std::span{test::profile_vectors[3]}.first(9));
        {
            windows::FileHandle lock(
                CreateFileW(locked_path.c_str(), GENERIC_READ, 0, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!lock ||
                windows::save_profile_file(main, test::profile_values[2]))
                return 1;
        }
        if (!matches(main, 1) || !matches(backup, 0) ||
            !windows::save_profile_file(main, test::profile_values[2]) ||
            !matches(main, 2) || !matches(backup, 1))
            return 1;
    }
    // Allow validation of main, then deny its final replacement. Backup may
    // advance to the previous main, but neither valid copy can disappear.
    test::write_bytes(main, test::profile_vectors[1]);
    test::write_bytes(backup, test::profile_vectors[0]);
    {
        windows::FileHandle lock(
            CreateFileW(main.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!lock || windows::save_profile_file(main, test::profile_values[2]))
            return 1;
    }
    if (!matches(main, 1) || !matches(backup, 1) ||
        !windows::save_profile_file(main, test::profile_values[2]) ||
        !matches(main, 2) || !matches(backup, 1))
        return 1;

    auto invalid = test::profile_values[0];
    invalid.master_volume = 101;
    if (windows::save_profile_file(main, invalid) ||
        windows::stage_profile_file(staging, invalid) ||
        std::filesystem::exists(staging) || !matches(main, 2) ||
        !matches(backup, 1))
        return 1;
    const std::array<std::uint8_t, 3> broken{1, 2, 3};
    test::write_bytes(main, broken);
    if (windows::save_profile_file(main, test::profile_values[0]) ||
        test::read_bytes(main) !=
            std::vector<std::uint8_t>(broken.begin(), broken.end()) ||
        !matches(backup, 1))
        return 1;
    test::write_bytes(staging, broken);
    if (windows::publish_profile_file(staging, main) || !matches(backup, 1))
        return 1;
    test::write_bytes(main, test::profile_vectors[0]);
    test::write_bytes(staging, test::profile_vectors[1]);
    const auto other = directory.path / L"other";
    std::filesystem::create_directory(other);
    if (windows::publish_profile_file(main, main) ||
        windows::publish_profile_file(staging, other / L"profile") ||
        !matches(main, 0) || !matches(staging, 1) ||
        windows::save_profile_file(other / L"missing" / L"profile", invalid) ||
        windows::save_profile_file(other / L"missing" / L"profile",
                                   test::profile_values[0]))
        return 1;
    std::filesystem::remove(staging);
    std::filesystem::create_directory(staging);
    if (windows::save_profile_file(main, test::profile_values[2]) ||
        !matches(main, 0) || !matches(backup, 1))
        return 1;
    std::filesystem::remove(staging);
    if (!windows::save_profile_file(main, test::profile_values[2]) ||
        !matches(main, 2) || !matches(backup, 0))
        return 1;
    return 0;
}
