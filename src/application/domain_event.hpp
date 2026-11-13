#pragma once

#include "core/command.hpp"
#include "core/run_state.hpp"
#include <optional>
#include <type_traits>
#include <variant>

namespace closed_circuit
{
// Stable internal protocol IDs, independent of variant alternative indexes.
enum class DomainEventKind : std::uint8_t
{
    action_committed = 1,
    packet_delivered = 2,
    sector_completed = 3,
    run_ended = 4
};

enum class RunEndReason : std::uint8_t
{
    won = 1,
    death = 2,
    overload = 3
};

// Occurrence within one logical run. A seed is not a unique run instance ID.
struct DomainEventKey
{
    RunSeed seed;
    SectorNumber sector;
    TurnIndex turn;
    std::uint8_t ordinal;
    constexpr auto operator<=>(const DomainEventKey &) const noexcept = default;
};

struct ActionCommitted
{
    Command command;
    // Target of the active packet whose BFS route this rotation changed.
    // Empty unless a nonempty, different route was observed before movement.
    std::optional<NodeId> reconfigured_packet_target = std::nullopt;
    constexpr bool operator==(const ActionCommitted &) const noexcept = default;
};
struct PacketDelivered
{
    NodeId target;
    constexpr bool operator==(const PacketDelivered &) const noexcept = default;
};
struct SectorCompleted
{
    constexpr bool operator==(const SectorCompleted &) const noexcept = default;
};
struct RunEnded
{
    RunEndReason reason;
    constexpr bool operator==(const RunEnded &) const noexcept = default;
};

using DomainEventPayload =
    std::variant<ActionCommitted, PacketDelivered, SectorCompleted, RunEnded>;

struct DomainEvent
{
    DomainEventKey key;
    DomainEventPayload payload;
    constexpr bool operator==(const DomainEvent &) const noexcept = default;

    [[nodiscard]] constexpr DomainEventKind kind() const noexcept
    {
        return std::visit(
            [](const auto &value)
            {
                using Value = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<Value, ActionCommitted>)
                    return DomainEventKind::action_committed;
                else if constexpr (std::is_same_v<Value, PacketDelivered>)
                    return DomainEventKind::packet_delivered;
                else if constexpr (std::is_same_v<Value, SectorCompleted>)
                    return DomainEventKind::sector_completed;
                else
                    return DomainEventKind::run_ended;
            },
            payload);
    }
};
} // namespace closed_circuit
