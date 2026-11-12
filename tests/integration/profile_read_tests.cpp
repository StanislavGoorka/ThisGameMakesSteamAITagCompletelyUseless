#include "../support/checkpoint_files.hpp"
#include "../support/profile_vectors.hpp"
#include "platform/windows/profile_read.hpp"
using namespace closed_circuit;

int main()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"profil \u017c\u00f3\u0142\u0107.dat";
    const auto backup = directory.path / L"profile.bak";
    test::write_bytes(backup, test::profile_vectors[1]);
    const auto backup_bytes = test::read_bytes(backup);
    for (std::size_t i = 0; i < test::profile_vectors.size(); ++i)
    {
        const auto &fixture = test::profile_vectors[i];
        test::write_bytes(main, fixture);
        for (unsigned repeat = 0; repeat != 2; ++repeat)
        {
            const auto result = windows::read_profile_file(main);
            if (result.profile != test::profile_values[i] ||
                result.error != ERROR_SUCCESS ||
                test::read_bytes(main) !=
                    std::vector<std::uint8_t>(fixture.begin(), fixture.end()))
                return 1;
        }
        for (std::size_t size = 0; size < 24; ++size)
        {
            const auto bytes = std::span{fixture}.first(size);
            test::write_bytes(main, bytes);
            const auto result = windows::read_profile_file(main);
            if (result.profile || result.error != ERROR_INVALID_DATA ||
                test::read_bytes(main) !=
                    std::vector<std::uint8_t>(bytes.begin(), bytes.end()))
                return 1;
        }
        for (const std::size_t size : {25u, 64u, 1024u * 1024u})
        {
            std::vector<std::uint8_t> bytes(fixture.begin(), fixture.end());
            bytes.resize(size);
            test::write_bytes(main, bytes);
            const auto result = windows::read_profile_file(main);
            if (result.profile || result.error != ERROR_INVALID_DATA ||
                test::read_bytes(main) != bytes)
                return 1;
        }
        auto broken = fixture;
        broken[8] ^= 1;
        test::write_bytes(main, broken);
        const auto result = windows::read_profile_file(main);
        if (result.profile || result.error != ERROR_INVALID_DATA ||
            test::read_bytes(main) !=
                std::vector<std::uint8_t>(broken.begin(), broken.end()))
            return 1;
        test::write_bytes(main, fixture);
        {
            windows::FileHandle lock(
                CreateFileW(main.c_str(), GENERIC_READ, 0, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            const auto locked = windows::read_profile_file(main);
            if (!lock || locked.profile ||
                locked.error != ERROR_SHARING_VIOLATION)
                return 1;
        }
        if (test::read_bytes(backup) != backup_bytes)
            return 1;
    }
    std::filesystem::remove(main);
    const auto missing = windows::read_profile_file(main);
    const auto no_parent = windows::read_profile_file(main / L"absent");
    const auto folder = windows::read_profile_file(directory.path);
    if (missing.profile || missing.error != ERROR_FILE_NOT_FOUND ||
        no_parent.profile || no_parent.error != ERROR_PATH_NOT_FOUND ||
        folder.profile || folder.error != ERROR_ACCESS_DENIED ||
        std::filesystem::exists(main) ||
        test::read_bytes(backup) != backup_bytes)
        return 1;
    return 0;
}
