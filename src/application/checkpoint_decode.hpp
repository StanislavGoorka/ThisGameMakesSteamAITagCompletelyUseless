#pragma once

#include "application/checkpoint_format.hpp"
#include "application/run_checkpoint.hpp"

namespace closed_circuit
{
namespace checkpoint_detail
{
[[nodiscard]] constexpr std::uint64_t
get_le(std::span<const std::uint8_t> bytes, std::size_t offset,
       std::size_t width) noexcept
{
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < width; ++i)
        value |= static_cast<std::uint64_t>(bytes[offset + i]) << (8 * i);
    return value;
}
} // namespace checkpoint_detail

[[nodiscard]] constexpr std::optional<RunCheckpoint>
decode_checkpoint(std::span<const std::uint8_t> bytes) noexcept
{
    // No field reads or variable-size allocations before the length gate.
    if (bytes.size() != CheckpointBytes{}.size())
        return std::nullopt;
    if (bytes[0] != 0x43 || bytes[1] != 0x43 || bytes[2] != 0x43 ||
        bytes[3] != 0x50 ||
        checkpoint_detail::get_le(bytes, 4, 4) != checkpoint_schema_version ||
        checkpoint_detail::get_le(bytes, 8, 4) !=
            checkpoint_run_rules_version ||
        checkpoint_detail::get_le(bytes, 32, 8) !=
            checkpoint_detail::checksum(bytes.first(32)) ||
        bytes[29] > 1)
        return std::nullopt;
    const auto number = SectorNumber::from_value(bytes[20]);
    if (!number)
        return std::nullopt;
    const auto modules = decode_module_slots({bytes[27], bytes[28]});
    const auto start_config = start_config_from_value(bytes[30]);
    const auto difficulty = DifficultyModifiers::from_value(bytes[31]);
    if (!modules || !start_config || !difficulty)
        return std::nullopt;
    RunCheckpoint checkpoint{
        static_cast<RunSeed>(checkpoint_detail::get_le(bytes, 12, 8)),
        *number,
        bytes[21],
        bytes[22],
        static_cast<std::uint32_t>(checkpoint_detail::get_le(bytes, 23, 4)),
        *modules,
        bytes[29] == 1,
        *start_config,
        *difficulty};
    if (!is_checkpoint_valid(checkpoint))
        return std::nullopt;
    return checkpoint;
}
} // namespace closed_circuit
