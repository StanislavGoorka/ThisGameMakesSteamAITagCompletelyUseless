#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include <algorithm>
#include <iostream>

using namespace closed_circuit;
namespace
{
bool matches(const std::filesystem::path &path, const test::CheckpointVector &v)
{
    return windows::read_checkpoint_file(path).checkpoint == v.checkpoint &&
           std::ranges::equal(test::read_bytes(path), v.bytes);
}
bool cases()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"run";
    const auto backup = directory.path / L"run.bak";
    const auto staging = directory.path / L"run.tmp";
    const auto backup_staging = directory.path / L"run.bak.tmp";
    const auto &a = test::checkpoint_vectors[0];
    const auto &b = test::checkpoint_vectors[1];
    const auto &c = test::checkpoint_vectors[2];
    if (!windows::save_checkpoint_file(main, a.checkpoint) ||
        !matches(main, a) || std::filesystem::exists(backup) ||
        std::filesystem::exists(staging))
        return false;
    if (!windows::save_checkpoint_file(main, b.checkpoint) ||
        !matches(main, b) || !matches(backup, a) ||
        std::filesystem::exists(backup_staging))
        return false;
    if (!windows::save_checkpoint_file(main, c.checkpoint) ||
        !matches(main, c) || !matches(backup, b) ||
        !windows::save_checkpoint_file(main, c.checkpoint) ||
        !matches(backup, c))
        return false;
    auto invalid = a.checkpoint;
    invalid.reserve = 4;
    if (windows::save_checkpoint_file(main, invalid) || !matches(main, c) ||
        !matches(backup, c) || std::filesystem::exists(staging))
        return false;
    auto bad = c.bytes;
    bad[0] = 0;
    test::write_bytes(main, bad);
    if (windows::save_checkpoint_file(main, a.checkpoint) ||
        !std::ranges::equal(test::read_bytes(main), bad) ||
        !matches(backup, c) || std::filesystem::exists(staging))
        return false;
    test::write_bytes(main, c.bytes);
    // A pre-existing backup survives first publication when main is absent.
    if (!std::filesystem::remove(main) ||
        !windows::save_checkpoint_file(main, b.checkpoint) ||
        !matches(main, b) || !matches(backup, c))
        return false;
    // Each path can independently deny the operation; retry uses the same slot.
    for (const auto &blocked : {staging, backup_staging, backup, main})
    {
        test::write_bytes(main, b.bytes);
        test::write_bytes(backup, c.bytes);
        test::write_bytes(staging, a.bytes);
        test::write_bytes(backup_staging, a.bytes);
        const bool blocks_main_publish = blocked == main;
        {
            // Let the main reader succeed, but deny deletion/rename. For the
            // other paths deny all access, including staging creation.
            windows::FileHandle lock(
                CreateFileW(blocked.c_str(), GENERIC_READ,
                            blocks_main_publish ? FILE_SHARE_READ : 0, nullptr,
                            OPEN_EXISTING, 0, nullptr));
            if (!lock || windows::save_checkpoint_file(main, a.checkpoint))
                return false;
        }
        if (!matches(main, b) ||
            !matches(backup, blocks_main_publish ? b : c) ||
            !windows::save_checkpoint_file(main, a.checkpoint) ||
            !matches(main, a) || !matches(backup, b) ||
            std::filesystem::exists(staging) ||
            std::filesystem::exists(backup_staging))
            return false;
    }
    {
        windows::FileHandle lock(CreateFileW(
            main.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!lock || windows::save_checkpoint_file(main, c.checkpoint))
            return false;
    }
    // Malformed staging and cross-directory publication are never promoted.
    test::write_bytes(staging, std::span{a.bytes}.first(17));
    std::filesystem::create_directory(directory.path / L"other");
    if (windows::publish_checkpoint_file(staging, main) || !matches(main, a) ||
        !windows::stage_checkpoint_file(staging, c.checkpoint) ||
        windows::publish_checkpoint_file(staging, staging) ||
        windows::publish_checkpoint_file(staging,
                                         directory.path / L"other" / L"run") ||
        !matches(staging, c) || !matches(backup, b))
        return false;
    if (windows::save_checkpoint_file(directory.path / L"missing" / L"run",
                                      a.checkpoint) ||
        windows::save_checkpoint_file(directory.path, a.checkpoint))
        return false;
    return matches(main, a) && matches(backup, b);
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
    std::cerr << "Checkpoint publication mismatch\n";
    return 1;
}
