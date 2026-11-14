#include "../support/checkpoint_files.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/profile_encode.hpp"
#include "core/run_start.hpp"
#include "platform/windows/rotation_binding_file.hpp"
using namespace closed_circuit;

int main()
{
    test::CheckpointDirectory scratch;
    const auto folder = scratch.path / L"ustawienia_\u017c";
    std::filesystem::create_directory(folder);
    const auto path = folder / L"rotation-key.bin";
    const auto staging = folder / L"rotation-key.bin.tmp";
    const auto profile = encode_profile(LocalProfile{3, 2, 37, true});
    const auto checkpoint =
        encode_checkpoint(*capture_checkpoint(start_run(RunSeed{1})));
    test::write_bytes(folder / L"profile.bin", *profile);
    test::write_bytes(folder / L"checkpoint.bin", *checkpoint);
    windows::LocalRotationBindingStore local{path};
    RotationBindingStore &port = local;
    const auto t = *RotationBinding::from_letter('T');
    if (windows::read_rotation_binding_file(path).error !=
            ERROR_FILE_NOT_FOUND ||
        std::filesystem::exists(path) || !port.save(t) ||
        windows::read_rotation_binding_file(path).binding != t ||
        std::filesystem::exists(staging))
        return 1;
    const auto saved = test::read_bytes(path);
    if (!std::ranges::equal(saved, encode_rotation_binding(t)))
        return 2;
    // A duplicate succeeds even if the staging name is unusable.
    std::filesystem::create_directory(staging);
    if (!port.save(t) || port.save(RotationBinding{}) ||
        test::read_bytes(path) != saved)
        return 3;
    std::filesystem::remove(staging);
    {
        windows::FileHandle lock(CreateFileW(
            path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!lock || windows::read_rotation_binding_file(path).binding ||
            port.save(RotationBinding{}) || std::filesystem::exists(staging))
            return 4;
    }
    // Read is possible, but publishing over the main name is denied.
    {
        windows::FileHandle lock(CreateFileW(path.c_str(), GENERIC_READ,
                                             FILE_SHARE_READ, nullptr,
                                             OPEN_EXISTING, 0, nullptr));
        if (!lock || port.save(RotationBinding{}) ||
            test::read_bytes(path) != saved ||
            !std::filesystem::exists(staging))
            return 5;
    }
    if (!port.save(RotationBinding{}) ||
        windows::read_rotation_binding_file(path).binding != RotationBinding{})
        return 6;
    auto corrupt = encode_rotation_binding(t);
    corrupt[8] ^= 1;
    test::write_bytes(path, corrupt);
    if (windows::read_rotation_binding_file(path).error != ERROR_INVALID_DATA ||
        port.save(RotationBinding{}) ||
        !std::ranges::equal(test::read_bytes(path), corrupt))
        return 7;
    for (std::size_t count : {0U, 15U})
    {
        test::write_bytes(path, std::span{corrupt}.first(count));
        if (windows::read_rotation_binding_file(path).binding || port.save(t))
            return 8;
    }
    const std::array<std::uint8_t, 17> oversized{};
    test::write_bytes(path, oversized);
    if (windows::read_rotation_binding_file(path).binding || port.save(t))
        return 9;
    windows::LocalRotationBindingStore missing{folder / L"missing" /
                                               L"binding.bin"};
    windows::LocalRotationBindingStore relative{L"binding.bin"};
    if (missing.save(t) || relative.save(t) ||
        !std::ranges::equal(test::read_bytes(folder / L"profile.bin"),
                            *profile) ||
        !std::ranges::equal(test::read_bytes(folder / L"checkpoint.bin"),
                            *checkpoint))
        return 10;
    return 0;
}
