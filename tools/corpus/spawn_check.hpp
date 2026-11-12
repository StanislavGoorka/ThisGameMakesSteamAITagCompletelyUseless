#pragma once

#include "core/game_state.hpp"

namespace closed_circuit::developer
{
enum class SpawnIssue : std::uint8_t
{
    none = 0,
    missing_endpoint = 1,
    inactive_node = 2,
    blocked_spawn = 3,
    duplicate_identity = 4,
    duplicate_position = 5,
    receiver_layout = 6
};

// Initial placements for this corpus. Pulse/player/packet sharing is legal;
// duplicate pulse IDs or positions and placement on blocking units are not.
[[nodiscard]] inline SpawnIssue initial_spawn_issue(const GameState &state)
{
    if (!state.sector_entry || !state.sector_exit ||
        state.sector_entry == state.sector_exit)
        return SpawnIssue::missing_endpoint;
    const auto placement = [&state](NodeId node)
    {
        if (state.ports[node.value()].bits() == 0)
            return SpawnIssue::inactive_node;
        return state.blocked_nodes[node.value()] ? SpawnIssue::blocked_spawn
                                                 : SpawnIssue::none;
    };
    for (const auto node :
         {*state.sector_entry, *state.sector_exit, state.player_position})
        if (const auto issue = placement(node); issue != SpawnIssue::none)
            return issue;
    std::size_t receivers = 0;
    for (std::size_t n = 0; n < GameState::node_count; ++n)
    {
        if (!state.receiver_nodes[n])
            continue;
        ++receivers;
        const auto node = *NodeId::from_value(n);
        if (node == state.sector_entry || node == state.sector_exit)
            return SpawnIssue::receiver_layout;
        if (const auto issue = placement(node); issue != SpawnIssue::none)
            return issue;
    }
    if (receivers != 2)
        return SpawnIssue::receiver_layout;
    if (state.packet)
    {
        if (const auto issue = placement(state.packet->position);
            issue != SpawnIssue::none)
            return issue;
        if (!state.receiver_nodes[state.packet->target.value()])
            return SpawnIssue::receiver_layout;
    }
    for (std::size_t i = 0; i < state.pulses.size(); ++i)
    {
        const auto &pulse = state.pulses[i];
        if (const auto issue = placement(pulse.position);
            issue != SpawnIssue::none)
            return issue;
        for (std::size_t j = 0; j < i; ++j)
        {
            if (state.pulses[j].id == pulse.id)
                return SpawnIssue::duplicate_identity;
            if (state.pulses[j].position == pulse.position)
                return SpawnIssue::duplicate_position;
        }
    }
    return SpawnIssue::none;
}
} // namespace closed_circuit::developer
