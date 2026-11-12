#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "platform/windows/checkpoint_read.hpp"
#include "platform/windows/checkpoint_stage.hpp"
#include <algorithm>
#include <iostream>

using namespace closed_circuit;
namespace
{
bool cases()
{
    test::CheckpointDirectory directory;
    const auto staging = directory.path / L"run.tmp";
    const auto main = directory.path / L"run";
    const auto backup = directory.path / L"run.bak";
    const auto &original = test::checkpoint_vectors[0];
    test::write_bytes(main, original.bytes);
    test::write_bytes(backup, original.bytes);
    for (const auto &v : test::checkpoint_vectors)
    {
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            if (!windows::stage_checkpoint_file(staging, v.checkpoint) ||
                !std::ranges::equal(test::read_bytes(staging), v.bytes) ||
                windows::read_checkpoint_file(staging).checkpoint !=
                    v.checkpoint)
                return false;
        }
        for (const std::size_t size : {0u, 17u, 4096u})
        {
            test::write_bytes(staging, std::vector<std::uint8_t>(size, 0x80));
            if (!windows::stage_checkpoint_file(staging, v.checkpoint) ||
                !std::ranges::equal(test::read_bytes(staging), v.bytes))
                return false;
        }
    }
    const auto before = test::read_bytes(staging);
    auto invalid = original.checkpoint;
    invalid.player_integrity = 0;
    const auto absent = directory.path / L"absent.tmp";
    if (windows::stage_checkpoint_file(staging, invalid) ||
        test::read_bytes(staging) != before ||
        windows::stage_checkpoint_file(absent, invalid) ||
        std::filesystem::exists(absent) ||
        windows::stage_checkpoint_file(directory.path, original.checkpoint) ||
        windows::stage_checkpoint_file(directory.path / L"missing" / L"run.tmp",
                                       original.checkpoint))
        return false;
    {
        windows::FileHandle lock(CreateFileW(staging.c_str(), GENERIC_READ, 0,
                                             nullptr, OPEN_EXISTING, 0,
                                             nullptr));
        if (!lock ||
            windows::stage_checkpoint_file(staging, original.checkpoint))
            return false;
    }
    return test::read_bytes(staging) == before &&
           std::ranges::equal(test::read_bytes(main), original.bytes) &&
           std::ranges::equal(test::read_bytes(backup), original.bytes) &&
           windows::stage_checkpoint_file(staging, original.checkpoint) &&
           std::ranges::equal(test::read_bytes(staging), original.bytes);
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
    std::cerr << "Checkpoint staging mismatch\n";
    return 1;
}
