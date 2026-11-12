#pragma once

#include "application/checkpoint_format.hpp"
#include "application/run_checkpoint.hpp"

namespace closed_circuit
{
[[nodiscard]] constexpr std::optional<CheckpointBytes>
encode_checkpoint(const RunCheckpoint &checkpoint) noexcept
{
    if (!is_checkpoint_valid(checkpoint))
        return std::nullopt;
    CheckpointBytes bytes{};
    bytes[0] = bytes[1] = bytes[2] = 0x43;
    bytes[3] = 0x50;
    checkpoint_detail::put_le(bytes, 4, 4, checkpoint_schema_version);
    checkpoint_detail::put_le(bytes, 8, 4, checkpoint_run_rules_version);
    checkpoint_detail::put_le(bytes, 12, 8,
                              static_cast<std::uint64_t>(checkpoint.seed));
    bytes[20] = checkpoint.sector_number.value();
    bytes[21] = checkpoint.player_integrity;
    bytes[22] = checkpoint.reserve;
    checkpoint_detail::put_le(bytes, 23, 4, checkpoint.overload_debt);
    for (std::size_t i = 0; i < ModuleLoadout::slot_limit; ++i)
        if (const auto id = checkpoint.modules.slots()[i])
            bytes[27 + i] = static_cast<std::uint8_t>(*id);
    bytes[29] = static_cast<std::uint8_t>(checkpoint.entry_debt_pulse);
    checkpoint_detail::put_le(
        bytes, 32, 8,
        checkpoint_detail::checksum(
            std::span<const std::uint8_t>{bytes}.first(32)));
    return bytes;
}
} // namespace closed_circuit
