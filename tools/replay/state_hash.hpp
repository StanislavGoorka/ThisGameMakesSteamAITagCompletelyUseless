#pragma once

#include "core/game_state.hpp"

#include <cstddef>
#include <cstdint>

namespace closed_circuit::developer
{

// Canonical encoding v1: fixed-width little-endian values, explicit optional
// tags, and stored pulse order. FNV-1a is a regression hash, not a security
// hash.
[[nodiscard]] inline std::uint64_t state_hash(const GameState &state) noexcept
{
    std::uint64_t hash = 14695981039346656037ULL;
    const auto byte = [&hash](std::uint8_t value)
    {
        hash ^= value;
        hash *= 1099511628211ULL;
    };
    const auto little_endian = [&byte](std::uint64_t value, std::size_t width)
    {
        for (std::size_t index = 0; index < width; ++index)
        {
            byte(static_cast<std::uint8_t>(value & 0xff));
            value >>= 8;
        }
    };
    const auto flags = [&byte](const auto &values)
    {
        for (const bool value : values)
        {
            byte(value ? 1 : 0);
        }
    };
    const auto optional_node = [&byte](const std::optional<NodeId> &value)
    {
        byte(value ? 1 : 0);
        if (value)
        {
            byte(value->value());
        }
    };

    little_endian(1, 4);
    for (const auto ports : state.ports)
    {
        byte(ports.bits());
    }
    flags(state.rotatable_nodes);
    flags(state.blocked_nodes);
    byte(state.player_position.value());
    byte(state.player_integrity);
    little_endian(static_cast<std::uint32_t>(state.turns_taken), 4);
    byte(state.packet ? 1 : 0);
    if (state.packet)
    {
        byte(state.packet->position.value());
        byte(state.packet->target.value());
    }
    flags(state.powered_nodes);
    flags(state.receiver_nodes);
    optional_node(state.sector_entry);
    optional_node(state.sector_exit);
    little_endian(static_cast<std::uint64_t>(state.pulses.size()), 8);
    for (const auto &pulse : state.pulses)
    {
        little_endian(pulse.id.value(), 4);
        byte(pulse.position.value());
    }
    return hash;
}

} // namespace closed_circuit::developer
