#pragma once

#include "core/identifiers.hpp"
#include "core/port_mask.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

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

// A pulse does not block the player or a packet. Valid states have unique
// pulse IDs and at most one pulse per node.
struct Pulse
{
    EntityId id;
    NodeId position;

    [[nodiscard]] constexpr bool
    operator==(const Pulse &) const noexcept = default;
};

struct GameState
{
    static constexpr std::size_t node_count = NodeId::max_value + 1;
    static constexpr std::uint8_t initial_player_integrity = 3;

    std::array<PortMask, node_count> ports{};
    std::array<bool, node_count> rotatable_nodes{};
    // Occupancy by blocking units only; packets and pulses do not block
    // player movement.
    std::array<bool, node_count> blocked_nodes{};
    NodeId player_position;
    // Zero means a dead player. No action restores integrity.
    std::uint8_t player_integrity{initial_player_integrity};
    TurnIndex turns_taken{};
    // The single active packet; empty while no packet is on the board.
    std::optional<Packet> packet{};
    // Delivery record: targets that already received a packet.
    std::array<bool, node_count> powered_nodes{};
    // Receivers of the sector: the nodes that packets are sent to. Their
    // NodeId sets the order in which they are targeted.
    std::array<bool, node_count> receiver_nodes{};
    // Packet starting node; empty until the sector supplies an entry.
    std::optional<NodeId> sector_entry{};
    // Value-owned units; their positions are the only pulse occupancy record.
    std::vector<Pulse> pulses{};

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
