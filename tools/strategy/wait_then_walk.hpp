#pragma once

#include "application/reward_selection.hpp"
#include "core/command.hpp"
#include "core/grid_adjacency.hpp"
#include "core/run_reward.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "core/run_turn.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace closed_circuit::developer
{
// BALANCE_EXPERIMENT_PLAN strategy: wait until the sector has its required
// deliveries, then walk the shortest legal route to the exit. No rotation,
// overdrive or module use; ties follow N, E, S, W. Not a player-facing aid.
[[nodiscard]] inline unsigned powered_receivers(const GameState &state) noexcept
{
    unsigned count = 0;
    for (std::size_t node = 0; node < GameState::node_count; ++node)
        count += state.receiver_nodes[node] && state.powered_nodes[node];
    return count;
}

// First step of the shortest route over connected, unblocked nodes, or
// nothing when the exit is unreachable or already reached.
[[nodiscard]] inline std::optional<NodeId> step_to_exit(const GameState &state)
{
    if (!state.sector_exit || *state.sector_exit == state.player_position)
        return std::nullopt;
    std::array<std::optional<NodeId>, GameState::node_count> previous{};
    previous[state.player_position.value()] = state.player_position;
    std::vector<NodeId> frontier{state.player_position};
    for (std::size_t index = 0; index < frontier.size(); ++index)
        for (const auto &next :
             connected_neighbors(state.ports, frontier[index]))
            if (next && !previous[next->value()] &&
                !state.blocked_nodes[next->value()])
            {
                previous[next->value()] = frontier[index];
                frontier.push_back(*next);
            }
    if (!previous[state.sector_exit->value()])
        return std::nullopt;
    auto node = *state.sector_exit;
    while (*previous[node.value()] != state.player_position)
        node = *previous[node.value()];
    return node;
}

[[nodiscard]] inline Command wait_then_walk(const GameState &state)
{
    if (powered_receivers(state) < state.delivery_requirement.value())
        return WaitCommand{};
    if (const auto step = step_to_exit(state))
    {
        const Command move = MoveCommand{*step};
        if (is_command_legal(state, move))
            return move;
    }
    return WaitCommand{};
}

enum class StrategyEnd : std::uint8_t
{
    won,
    death,
    overload,
    rejected
};

struct StrategyRun
{
    unsigned sectors_completed = 0;
    StrategyEnd end = StrategyEnd::rejected;
    // Turns taken when sector 1 completed; 0 when it did not.
    std::uint32_t first_sector_turn = 0;
};

// One run from the root seed (ID1, mask0). Rewards take the first offered
// module. Stops with rejected if the resolver ever refuses a bot command.
[[nodiscard]] inline StrategyRun play_wait_then_walk(RunSeed seed)
{
    auto run = start_run(seed);
    StrategyRun result;
    for (;;)
    {
        const auto phase = run_phase(run);
        if (phase == RunPhase::playing)
        {
            const auto next = resolve_run_turn(run, wait_then_walk(run.sector));
            if (!next.accepted)
                return result;
            const bool completed =
                next.phase != RunPhase::playing && next.phase != RunPhase::lost;
            if (completed)
            {
                ++result.sectors_completed;
                if (run.sector_number.value() == 1)
                    result.first_sector_turn = static_cast<std::uint32_t>(
                        next.state.sector.turns_taken);
            }
            run = next.state;
            continue;
        }
        if (phase == RunPhase::won)
        {
            result.end = StrategyEnd::won;
            return result;
        }
        if (phase == RunPhase::lost)
        {
            result.end = run.sector.player_alive() ? StrategyEnd::overload
                                                   : StrategyEnd::death;
            return result;
        }
        if (phase == RunPhase::choosing_reward)
        {
            const auto offer = reward_offer(run);
            if (!offer[0] || !choose_run_module(run, *offer[0]))
                return result;
        }
        const auto next = advance_run(run);
        if (!next.accepted)
            return result;
        run = next.state;
    }
}
} // namespace closed_circuit::developer
