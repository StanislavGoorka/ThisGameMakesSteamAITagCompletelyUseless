#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/run_checks.hpp"
#include "platform/windows/checkpoint_read.hpp"
#include <algorithm>
#include <iostream>

using namespace closed_circuit;
namespace
{
bool cases()
{
    test::CheckpointDirectory directory;
    const auto path = directory.path / L"run.checkpoint";
    if (windows::read_checkpoint_file(path).error != ERROR_FILE_NOT_FOUND ||
        windows::read_checkpoint_file(directory.path).checkpoint)
        return false;
    for (const auto &v : test::checkpoint_vectors)
    {
        test::write_bytes(path, v.bytes);
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto result = windows::read_checkpoint_file(path);
            if (result.error != ERROR_SUCCESS ||
                result.checkpoint != v.checkpoint ||
                !std::ranges::equal(test::read_bytes(path), v.bytes))
                return false;
            const auto restored = restore_checkpoint(*result.checkpoint);
            const auto expected = restore_checkpoint(v.checkpoint);
            if (!restored || !expected || !test::same_run(*restored, *expected))
                return false;
        }
        for (std::size_t length = 0; length < v.bytes.size(); ++length)
        {
            const auto truncated = std::span{v.bytes}.first(length);
            test::write_bytes(path, truncated);
            const auto result = windows::read_checkpoint_file(path);
            if (result.checkpoint || result.error != ERROR_INVALID_DATA ||
                !std::ranges::equal(test::read_bytes(path), truncated))
                return false;
        }
        auto bad = v.bytes;
        bad[12] ^= 1;
        test::write_bytes(path, bad);
        if (windows::read_checkpoint_file(path).error != ERROR_INVALID_DATA ||
            !std::ranges::equal(test::read_bytes(path), bad))
            return false;
    }
    for (const std::size_t length : {41u, 4096u, 1048576u})
    {
        const std::vector<std::uint8_t> large(length, 0x43);
        test::write_bytes(path, large);
        if (windows::read_checkpoint_file(path).error != ERROR_INVALID_DATA ||
            test::read_bytes(path) != large)
            return false;
    }
    test::write_bytes(path, test::checkpoint_vectors[0].bytes);
    {
        windows::FileHandle lock(CreateFileW(
            path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        const auto result = windows::read_checkpoint_file(path);
        if (!lock || result.checkpoint ||
            result.error != ERROR_SHARING_VIOLATION)
            return false;
    }
    return std::ranges::equal(test::read_bytes(path),
                              test::checkpoint_vectors[0].bytes) &&
           std::filesystem::remove(path);
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
    std::cerr << "Checkpoint file read mismatch\n";
    return 1;
}
