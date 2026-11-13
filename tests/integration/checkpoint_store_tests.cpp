#include "../support/checkpoint_files.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "platform/windows/local_checkpoint_store.hpp"
#include <algorithm>

using namespace closed_circuit;
namespace
{
bool holds(const std::filesystem::path &path, const test::CheckpointVector &v)
{
    return std::filesystem::exists(path) &&
           std::ranges::equal(test::read_bytes(path), v.bytes);
}
windows::FileHandle exclusive(const std::filesystem::path &path)
{
    return windows::FileHandle(CreateFileW(path.c_str(), GENERIC_READ, 0,
                                           nullptr, OPEN_EXISTING, 0, nullptr));
}
bool cases()
{
    test::CheckpointDirectory directory;
    const auto main = directory.path / L"checkpoint ż.bin";
    auto backup = main;
    backup += L".bak";
    auto staging = main;
    staging += L".tmp";
    windows::LocalCheckpointStore local{main};
    CheckpointStore &store = local;
    const auto &a = test::checkpoint_vectors[0];
    const auto &b = test::checkpoint_vectors[1];
    const auto &c = test::checkpoint_vectors[2];
    const auto empty = [&] {
        return !std::filesystem::exists(main) &&
               !std::filesystem::exists(backup);
    };
    for (int repeat = 0; repeat < 2; ++repeat)
        if (!store.clear() || !empty())
            return false;
    if (!store.save(a.checkpoint) || !holds(main, a) ||
        std::filesystem::exists(backup))
        return false;
    if (!store.save(b.checkpoint) || !holds(main, b) || !holds(backup, a))
        return false;
    for (int repeat = 0; repeat < 2; ++repeat)
        if (!store.clear() || !empty())
            return false;
    // A locked backup stops clear before main, so no older run is exposed.
    if (!store.save(a.checkpoint) || !store.save(c.checkpoint))
        return false;
    // Bytes are compared only after the test releases its exclusive locks.
    {
        const auto lock = exclusive(backup);
        if (!lock || store.clear())
            return false;
    }
    if (!holds(main, c) || !holds(backup, a))
        return false;
    {
        const auto lock = exclusive(main);
        if (!lock || store.clear() || std::filesystem::exists(backup))
            return false;
        // The same lock makes the previous main unreadable, so save refuses.
        if (store.save(b.checkpoint))
            return false;
    }
    if (!holds(main, c) || std::filesystem::exists(backup))
        return false;
    if (!store.clear() || !empty())
        return false;
    // Only a backup left by an interrupted clear is still removed.
    test::write_bytes(backup, a.bytes);
    if (!store.clear() || !empty())
        return false;
    // Invalid main blocks save without overwriting; clear ends that run.
    auto bad = b.bytes;
    bad[0] = 0;
    test::write_bytes(main, bad);
    test::write_bytes(backup, c.bytes);
    if (store.save(a.checkpoint) ||
        !std::ranges::equal(test::read_bytes(main), bad) || !holds(backup, c))
        return false;
    auto invalid = a.checkpoint;
    invalid.player_integrity = 0;
    test::write_bytes(main, b.bytes);
    if (store.save(invalid) || !holds(main, b) || !holds(backup, c))
        return false;
    test::write_bytes(staging, a.bytes);
    if (!store.clear() || !empty() || !holds(staging, a))
        return false;
    windows::LocalCheckpointStore missing{directory.path / L"none" / L"main"};
    return missing.clear() && !missing.save(a.checkpoint) &&
           !std::filesystem::exists(directory.path / L"none");
}
} // namespace

int main()
{
    try
    {
        return cases() ? 0 : 1;
    }
    catch (...)
    {
        return 2;
    }
}
