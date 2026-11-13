#include "../support/checkpoint_files.hpp"
#include "platform/windows/checkpoint_location.hpp"
using namespace closed_circuit;

int main()
{
    // The real base is only queried; tests never create the user's directory.
    const auto system = windows::local_profile_base();
    if (system.error != S_OK || !system.path.is_absolute() ||
        !std::filesystem::is_directory(system.path))
        return 1;
    test::CheckpointDirectory owned;
    const auto base = owned.path / L"checkpoint_ż";
    std::filesystem::create_directory(base);
    const auto location = windows::checkpoint_location_in(base);
    if (location.error != S_OK ||
        location.path != base / L"ClosedCircuit" / L"checkpoint.bin" ||
        std::filesystem::exists(location.path))
        return 2;
    const auto profile = windows::profile_location_in(base);
    if (profile.error != S_OK ||
        profile.path.parent_path() != location.path.parent_path() ||
        profile.path == location.path)
        return 3;
    const std::vector<std::uint8_t> saved{7, 0, 255, 3};
    const std::vector<std::uint8_t> settings{1, 4, 9};
    test::write_bytes(location.path, saved);
    test::write_bytes(profile.path, settings);
    for (int i = 0; i < 3; ++i)
    {
        const auto repeat = windows::checkpoint_location_in(base);
        if (repeat.error != S_OK || repeat.path != location.path ||
            test::read_bytes(repeat.path) != saved ||
            test::read_bytes(profile.path) != settings)
            return 4;
    }
    // Resolving the checkpoint first must create the same shared directory.
    const auto other = owned.path / L"other";
    std::filesystem::create_directory(other);
    const auto first = windows::checkpoint_location_in(other);
    const auto second = windows::profile_location_in(other);
    if (first.error != S_OK || second.error != S_OK ||
        first.path.parent_path() != second.path.parent_path() ||
        !std::filesystem::is_directory(first.path.parent_path()) ||
        std::filesystem::exists(first.path))
        return 5;
    const auto collision = owned.path / L"collision";
    std::filesystem::create_directory(collision);
    test::write_bytes(collision / L"ClosedCircuit", saved);
    const auto blocked = windows::checkpoint_location_in(collision);
    if (blocked.error != HRESULT_FROM_WIN32(ERROR_DIRECTORY) ||
        !blocked.path.empty() ||
        test::read_bytes(collision / L"ClosedCircuit") != saved)
        return 6;
    for (const auto &bad :
         {std::filesystem::path{}, std::filesystem::path{L"relative"},
          owned.path / L"missing"})
    {
        const auto rejected = windows::checkpoint_location_in(bad);
        if (SUCCEEDED(rejected.error) || !rejected.path.empty() ||
            std::filesystem::exists(bad / L"ClosedCircuit"))
            return 7;
    }
    return 0;
}
