#include "../support/checkpoint_files.hpp"
#include "platform/windows/rotation_binding_startup.hpp"
#include <algorithm>
using namespace closed_circuit;

int main()
{
    test::CheckpointDirectory scratch;
    const auto location = windows::profile_location_in(scratch.path);
    if (FAILED(location.error))
        return 1;
    const std::array<std::uint8_t, 3> sentinel{11, 22, 33};
    test::write_bytes(location.path, sentinel);
    const auto checkpoint = location.path.parent_path() / L"checkpoint.bin";
    test::write_bytes(checkpoint, sentinel);
    const auto path = location.path.parent_path() / L"rotation-key.bin";
    const auto first = windows::rotation_binding_startup(location);
    if (first.path != path || FAILED(first.error) ||
        first.binding != RotationBinding{} || std::filesystem::exists(path))
        return 2;
    windows::LocalRotationBindingStore store{first.path};
    const auto t = *RotationBinding::from_letter('T');
    if (!store.save(t))
        return 3;
    const auto saved = test::read_bytes(path);
    for (int i = 0; i < 3; ++i)
    {
        const auto read = windows::rotation_binding_startup(location);
        if (read.path != path || read.binding != t || FAILED(read.error) ||
            test::read_bytes(path) != saved)
            return 4;
    }
    {
        windows::FileHandle lock(CreateFileW(
            path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        const auto read = windows::rotation_binding_startup(location);
        if (!lock || read.binding != RotationBinding{} || read.path != path ||
            read.error != HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
            return 5;
    }
    if (test::read_bytes(path) != saved)
        return 6;
    auto broken = encode_rotation_binding(t);
    broken[8] ^= 1;
    test::write_bytes(path, broken);
    // Even a valid staging is never an automatic fallback or repair.
    test::write_bytes(path.wstring() + L".tmp", encode_rotation_binding(t));
    const auto corrupt = windows::rotation_binding_startup(location);
    if (corrupt.binding != RotationBinding{} || corrupt.path != path ||
        corrupt.error != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) ||
        !std::ranges::equal(test::read_bytes(path), broken) || store.save(t))
        return 7;
    const auto invalid =
        windows::rotation_binding_startup({L"relative/profile.bin", S_OK});
    const auto unavailable =
        windows::rotation_binding_startup({location.path, E_ACCESSDENIED});
    const auto missing_parent = windows::rotation_binding_startup(
        {scratch.path / L"missing" / L"profile.bin", S_OK});
    if (!invalid.path.empty() || invalid.error != E_INVALIDARG ||
        !unavailable.path.empty() || unavailable.error != E_ACCESSDENIED ||
        missing_parent.binding != RotationBinding{} ||
        !FAILED(missing_parent.error) ||
        std::filesystem::exists(scratch.path / L"missing") ||
        !std::ranges::equal(test::read_bytes(location.path), sentinel) ||
        !std::ranges::equal(test::read_bytes(checkpoint), sentinel))
        return 8;
    return 0;
}
