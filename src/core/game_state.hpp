#pragma once

#include "core/identifiers.hpp"
#include "core/port_mask.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace closed_circuit
{

// A packet occupies a node without blocking it and keeps its target until
// delivery.
struct Packet
{
    NodeId position;
    NodeId target;

    [[nodiscard]] constexpr bool
    operator==(const Packet &) const noexcept = default;
};

struct GameState
{
    static constexpr std::size_t node_count = NodeId::max_value + 1;
    static constexpr std::uint8_t initial_player_integrity = 3;

    std::array<PortMask, node_count> ports{};
    std::array<bool, node_count> rotatable_nodes{};
    // Occupancy by blocking units only; packets do not block movement.
    std::array<bool, node_count> blocked_nodes{};
    NodeId player_position;
    // Zero means a dead player. No action restores integrity.
    std::uint8_t player_integrity{initial_player_integrity};
    TurnIndex turns_taken{};
    // The single active packet; empty while no packet is on the board.
    std::optional<Packet> packet{};

    [[nodiscard]] constexpr bool player_alive() const noexcept
    {
        return player_integrity > 0;
    }

    // Both input sequences follow NodeId order. An empty position signals
    // a missing or invalid starting node.
    [[nodiscard]] static constexpr std::optional<GameState>
    initial(std::span<const PortMask> ports,
            std::span<const bool> rotatable_nodes,
            std::optional<NodeId> player_position) noexcept
    {
        if (ports.size() != node_count ||
            rotatable_nodes.size() != node_count || !player_position)
        {
            return std::nullopt;
        }

        GameState state{.player_position = *player_position};
        std::ranges::copy(ports, state.ports.begin());
        std::ranges::copy(rotatable_nodes, state.rotatable_nodes.begin());
        return state;
    }
};

} // namespace closed_circuit
