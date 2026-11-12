#include "../support/checkpoint_files.hpp"
#include "platform/windows/profile_unlock.hpp"
using namespace closed_circuit;
using Result = windows::ProfileFileUnlockResult;

std::array<std::filesystem::path, 4> paths(const std::filesystem::path &main)
{
    return {main, main.native() + L".bak", main.native() + L".tmp",
            main.native() + L".bak.tmp"};
}

bool cases()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"profil \u017c\u00f3\u0142\u0107.dat";
    const auto names = paths(main);
    const auto fresh = directory.path / L"fresh";
    if (windows::unlock_profile_file(fresh, AchievementId::first_delivery) !=
            Result::unlocked ||
        windows::read_profile_file(fresh).profile != LocalProfile{1} ||
        std::filesystem::exists(fresh.native() + L".bak"))
        return false;
    auto expected = LocalProfile{0, 3, 17, true};
    if (!windows::save_profile_file(main, expected))
        return false;
    for (unsigned raw = 1; raw <= 5; ++raw)
    {
        const auto previous = expected;
        const auto id = static_cast<AchievementId>(raw);
        expected.achievement_bits |=
            static_cast<std::uint16_t>(1u << (raw - 1));
        if (windows::unlock_profile_file(main, id) != Result::unlocked ||
            windows::read_profile_file(main).profile != expected ||
            windows::read_profile_file(names[1]).profile != previous)
            return false;
        const std::array<std::uint8_t, 3> partial{7, 8, 9};
        test::write_bytes(names[2], partial);
        test::write_bytes(names[3], partial);
        std::array<std::vector<std::uint8_t>, 4> before;
        for (std::size_t i = 0; i < 4; ++i)
            before[i] = test::read_bytes(names[i]);
        if (windows::unlock_profile_file(main, id) != Result::already_unlocked)
            return false;
        for (std::size_t i = 0; i < 4; ++i)
            if (test::read_bytes(names[i]) != before[i])
                return false;
    }
    const LocalProfile baseline{1, 2, 31, true};
    const auto bytes = *encode_profile(baseline);
    for (const auto &locked : names)
    {
        for (const auto &name : names)
            test::write_bytes(name, bytes);
        {
            windows::FileHandle lock(
                CreateFileW(locked.c_str(), GENERIC_READ, 0, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!lock ||
                windows::unlock_profile_file(main,
                                             AchievementId::first_sector) !=
                    (locked == main ? Result::read_error : Result::write_error))
                return false;
        }
        if (test::read_bytes(main) !=
                std::vector<std::uint8_t>(bytes.begin(), bytes.end()) ||
            windows::read_profile_file(names[1]).profile != baseline ||
            windows::unlock_profile_file(main, AchievementId::first_sector) !=
                Result::unlocked ||
            windows::read_profile_file(main).profile !=
                LocalProfile{3, 2, 31, true})
            return false;
    }
    // Main can be read while its final replacement is denied.
    test::write_bytes(main, bytes);
    {
        windows::FileHandle lock(
            CreateFileW(main.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!lock ||
            windows::unlock_profile_file(main, AchievementId::first_sector) !=
                Result::write_error)
            return false;
    }
    if (windows::read_profile_file(main).profile != baseline ||
        windows::unlock_profile_file(main, AchievementId::first_sector) !=
            Result::unlocked)
        return false;
    const std::array<std::uint8_t, 3> broken{1, 2, 3};
    test::write_bytes(main, broken);
    const auto backup_before = test::read_bytes(names[1]);
    if (windows::unlock_profile_file(main, AchievementId::first_sector) !=
            Result::read_error ||
        test::read_bytes(main) !=
            std::vector<std::uint8_t>(broken.begin(), broken.end()) ||
        test::read_bytes(names[1]) != backup_before ||
        windows::unlock_profile_file(directory.path / L"missing" / L"profile",
                                     AchievementId::first_sector) !=
            Result::read_error)
        return false;
    for (const unsigned raw : {0, 6, 255})
    {
        const auto absent = directory.path / L"absent";
        if (windows::unlock_profile_file(absent,
                                         static_cast<AchievementId>(raw)) !=
                Result::invalid_id ||
            std::filesystem::exists(absent) ||
            windows::unlock_profile_file(
                main, static_cast<AchievementId>(raw)) != Result::invalid_id ||
            test::read_bytes(main) !=
                std::vector<std::uint8_t>(broken.begin(), broken.end()) ||
            test::read_bytes(names[1]) != backup_before)
            return false;
    }
    return true;
}
int main()
{
    return cases() ? 0 : 1;
}
