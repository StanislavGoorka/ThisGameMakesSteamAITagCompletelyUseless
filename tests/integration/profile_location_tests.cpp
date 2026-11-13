#include "../support/checkpoint_files.hpp"
#include "platform/windows/profile_location.hpp"
using namespace closed_circuit;

int main()
{
    const auto system = windows::local_profile_base();
    if (system.error != S_OK || !system.path.is_absolute() ||
        !std::filesystem::is_directory(system.path))
        return 1;
    PWSTR expected = nullptr;
    const auto result =
        SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &expected);
    const bool same = SUCCEEDED(result) && expected &&
                      system.path == std::filesystem::path(expected);
    CoTaskMemFree(expected);
    if (!same)
        return 2;
    test::CheckpointDirectory owned;
    const auto base = owned.path / L"profile_\u017c";
    std::filesystem::create_directory(base);
    const auto location = windows::profile_location_in(base);
    if (location.error != S_OK ||
        location.path != base / L"ClosedCircuit" / L"profile.bin" ||
        std::filesystem::exists(location.path))
        return 3;
    const std::vector<std::uint8_t> bytes{1, 4, 9, 0, 255};
    test::write_bytes(location.path, bytes);
    for (int i = 0; i < 3; ++i)
    {
        const auto repeat = windows::profile_location_in(base);
        if (repeat.error != S_OK || repeat.path != location.path ||
            test::read_bytes(repeat.path) != bytes)
            return 4;
    }
    const auto collision = owned.path / L"collision";
    std::filesystem::create_directory(collision);
    test::write_bytes(collision / L"ClosedCircuit", bytes);
    const auto blocked = windows::profile_location_in(collision);
    if (blocked.error != HRESULT_FROM_WIN32(ERROR_DIRECTORY) ||
        !blocked.path.empty() ||
        test::read_bytes(collision / L"ClosedCircuit") != bytes)
        return 5;
    for (const auto &bad :
         {std::filesystem::path{}, std::filesystem::path{L"relative"},
          owned.path / L"missing"})
    {
        const auto rejected = windows::profile_location_in(bad);
        if (SUCCEEDED(rejected.error) || !rejected.path.empty())
            return 6;
    }
    return 0;
}
