#include "../support/checkpoint_files.hpp"
#include "platform/windows/approved_wave.hpp"
#include <algorithm>
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
using Wave = windows::ApprovedDeliveryWave;
static_assert(!std::is_default_constructible_v<Wave>);
static_assert(!std::is_constructible_v<Wave, std::vector<std::uint8_t>>);
static_assert(std::is_nothrow_move_constructible_v<Wave>);

bool matches(const Wave &wave, const std::vector<std::uint8_t> &original)
{
    const auto view = wave.view();
    return view.channels == 1 && view.sample_rate == 44100 &&
           view.block_align == 2 &&
           std::ranges::equal(view.data, std::span{original}.subspan(44));
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    const std::filesystem::path root{argv[1]};
    const auto &ref = windows::delivery_audio_reference();
    const auto original = test::read_bytes(root / ref.path());
    auto loaded = Wave::load(root);
    if (!loaded || !matches(*loaded, original))
        return 2;
    const auto original_pointer = loaded->view().data.data();
    auto copy = *loaded;
    if (!matches(copy, original) || copy.view().data.data() == original_pointer)
        return 3;
    auto moved = std::move(*loaded);
    loaded.reset();
    if (!matches(moved, original) || !matches(copy, original))
        return 4;
    auto assigned = copy;
    assigned = std::move(moved);
    if (!matches(assigned, original))
        return 5;
    copy = assigned;
    if (!matches(copy, original) ||
        copy.view().data.data() == assigned.view().data.data())
        return 6;
    if (Wave::load({}) || Wave::load(L".") ||
        Wave::load(root / L"missing-root"))
        return 7;
    test::CheckpointDirectory isolated;
    const auto isolated_root = std::filesystem::absolute(isolated.path);
    const auto target = isolated_root / ref.path();
    std::filesystem::create_directories(target.parent_path());
    if (Wave::load(isolated_root))
        return 8;
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        test::write_bytes(target, original);
        auto present = Wave::load(isolated_root);
        if (!present || !matches(*present, original))
            return 9;
        {
            windows::FileHandle exclusive(
                CreateFileW(target.c_str(), GENERIC_READ, 0, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
            if (!exclusive || Wave::load(isolated_root))
                return 10;
        }
        auto corrupt = original;
        // Alter metadata only; no samples are created or changed.
        corrupt[0] ^= 1;
        test::write_bytes(target, corrupt);
        if (Wave::load(isolated_root) || !matches(*present, original))
            return 11;
        test::write_bytes(target, std::span{original}.first(43));
        if (Wave::load(isolated_root))
            return 12;
        std::filesystem::remove(target);
        if (Wave::load(isolated_root) || !matches(*present, original))
            return 13;
    }
    if (test::read_bytes(root / ref.path()) != original ||
        !matches(assigned, original))
        return 14;
    return 0;
}
