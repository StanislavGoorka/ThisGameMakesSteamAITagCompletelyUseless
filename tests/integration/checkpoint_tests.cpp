#include "../support/checkpoint_bytes.hpp"
#include "../support/checkpoint_vectors.hpp"
#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "corpus/fixed_seeds.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
bool rejected(std::span<const std::uint8_t> bytes)
{
    const std::vector<std::uint8_t> before(bytes.begin(), bytes.end());
    return !decode_checkpoint(bytes) && std::ranges::equal(bytes, before);
}
bool round_trip(const RunState &run)
{
    const auto before = run;
    const auto checkpoint = capture_checkpoint(run);
    if (!checkpoint)
        return false;
    const auto bytes = encode_checkpoint(*checkpoint);
    if (!bytes)
        return false;
    auto decoded = decode_checkpoint(*bytes);
    if (!decoded || *decoded != *checkpoint)
        return false;
    for (int repeat = 0; repeat < 2; ++repeat)
    {
        auto restored = restore_checkpoint(*decoded);
        if (!restored || !test::same_run(*restored, before) ||
            capture_checkpoint(*restored) != checkpoint ||
            encode_checkpoint(*decoded) != bytes)
            return false;
        const auto live = resolve_run_turn(run, WaitCommand{});
        const auto resumed = resolve_run_turn(*restored, WaitCommand{});
        if (!live.accepted || !resumed.accepted ||
            live.phase != resumed.phase ||
            !test::same_run(live.state, resumed.state) ||
            resumed.state.sector.overload_debt != before.sector.overload_debt ||
            resumed.state.sector.player_integrity !=
                before.sector.player_integrity -
                    (checkpoint->entry_debt_pulse ? 1 : 0) ||
            !resumed.state.sector.pulses.empty())
            return false;
        restored->sector.pulses.push_back(
            {*EntityId::from_value(77), *NodeId::from_value(48)});
        restored->sector.player_integrity = 0;
        if (!test::same_run(run, before) || *decoded != *checkpoint)
            return false;
    }
    return true;
}
bool boundaries()
{
    std::size_t count = 0;
    for (const auto seed : developer::sector_seeds)
    {
        const auto first = start_run(RunSeed{seed});
        if (!round_trip(first))
            return false;
        ++count;
        for (int number = 2; number <= 3; ++number)
            for (const std::uint32_t debt : {0u, 2u, 3u, 6u, UINT32_MAX})
            {
                // Completion/resources are explicit synthetic fixtures;
                // transition is real.
                auto previous = first;
                previous.sector_number = *SectorNumber::from_value(number - 1);
                previous.sector.player_position = *previous.sector.sector_exit;
                previous.sector.powered_nodes = previous.sector.receiver_nodes;
                previous.reward_claimed = true;
                previous.sector.player_integrity =
                    static_cast<std::uint8_t>(seed % 3 + 1);
                previous.sector.reserve = static_cast<std::uint8_t>(seed % 4);
                previous.sector.overload_debt = debt;
                if (seed & 1)
                    static_cast<void>(
                        previous.sector.modules.add(ModuleId::rotation_range));
                if (seed & 2)
                    static_cast<void>(previous.sector.modules.add(
                        ModuleId::terminal_rotation));
                const auto result = advance_run(previous);
                if (!result.accepted ||
                    result.state.sector_number.value() != number ||
                    result.state.sector.overload_debt !=
                        (debt >= 3 ? debt - 3 : debt) ||
                    result.state.sector.pulses.size() !=
                        (debt >= 3 ? 1u : 0u) ||
                    !round_trip(result.state))
                    return false;
                ++count;
            }
    }
    std::cout << "checkpoint_boundaries " << count << '\n';
    return count == 11000;
}
bool corruption()
{
    for (const auto &v : test::checkpoint_vectors)
    {
        for (std::size_t length = 0; length < 40; ++length)
            if (!rejected(std::span<const std::uint8_t>{v.bytes}.first(length)))
                return false;
        for (int extra = 1; extra <= 64; ++extra)
        {
            std::vector<std::uint8_t> bytes(v.bytes.begin(), v.bytes.end());
            bytes.resize(40 + extra, 0xa5);
            if (!rejected(bytes))
                return false;
        }
        for (std::size_t offset = 0; offset < 40; ++offset)
        {
            for (int bit = 0; bit < 8; ++bit)
            {
                auto bad = v.bytes;
                bad[offset] ^= static_cast<std::uint8_t>(1u << bit);
                if (!rejected(bad))
                    return false;
            }
            for (unsigned value = 0; value < 256; ++value)
                if (value != v.bytes[offset])
                {
                    auto bad = v.bytes;
                    bad[offset] = static_cast<std::uint8_t>(value);
                    if (!rejected(bad))
                        return false;
                }
        }
    }
    return true;
}
bool semantic_fields()
{
    for (const auto &v : test::checkpoint_vectors)
        for (std::size_t offset = 0; offset < 32; ++offset)
            for (unsigned value = 0; value < 256; ++value)
            {
                auto bytes = v.bytes;
                bytes[offset] = static_cast<std::uint8_t>(value);
                bool valid = true;
                if (offset < 12)
                    valid = value == v.bytes[offset];
                if (offset == 20 || offset == 21)
                    valid = value >= 1 && value <= 3;
                if (offset == 22)
                    valid = value <= 3;
                if ((offset >= 23 && offset <= 26) || offset == 29)
                {
                    const std::uint32_t debt =
                        std::uint32_t{bytes[23]} +
                        (std::uint32_t{bytes[24]} << 8) +
                        (std::uint32_t{bytes[25]} << 16) +
                        (std::uint32_t{bytes[26]} << 24);
                    valid = bytes[29] <= 1 &&
                            (bytes[29] == 0 ? debt <= 2 : debt <= 4294967292u);
                }
                if (offset == 27 || offset == 28)
                {
                    const unsigned pair = bytes[27] * 256u + bytes[28];
                    valid =
                        pair == 0 || pair == 256 || pair == 512 || pair == 258;
                }
                if (offset == 30 || offset == 31)
                    valid = value == 0;
                test::reseal(bytes);
                const auto before = bytes;
                const auto decoded = decode_checkpoint(bytes);
                if (decoded.has_value() != valid || bytes != before ||
                    (decoded && encode_checkpoint(*decoded) != bytes))
                    return false;
            }
    // Exhaust every slot pair; reject all but the four canonical encodings.
    for (unsigned a = 0; a < 256; ++a)
        for (unsigned b = 0; b < 256; ++b)
        {
            auto bytes = test::checkpoint_vectors[0].bytes;
            bytes[27] = static_cast<std::uint8_t>(a);
            bytes[28] = static_cast<std::uint8_t>(b);
            test::reseal(bytes);
            const bool valid = (a == 0 && b == 0) || (a == 1 && b == 0) ||
                               (a == 2 && b == 0) || (a == 1 && b == 2);
            if (decode_checkpoint(bytes).has_value() != valid)
                return false;
        }
    return true;
}
} // namespace
int main()
{
    if (!corruption() || !semantic_fields())
    {
        std::cerr << "Checkpoint malformed corpus mismatch\n";
        return 1;
    }
    if (!boundaries())
    {
        std::cerr << "Checkpoint boundary corpus mismatch\n";
        return 1;
    }
    return 0;
}
