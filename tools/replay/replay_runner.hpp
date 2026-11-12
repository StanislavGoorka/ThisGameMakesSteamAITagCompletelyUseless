#pragma once

#include "core/turn_resolver.hpp"
#include "replay/seeded_fixture.hpp"
#include "replay/state_hash.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace closed_circuit::developer
{

inline constexpr std::uint32_t replay_version = 6;

struct ReplayStep
{
    bool accepted;
    SectorOutcome outcome;
    std::uint64_t hash;

    [[nodiscard]] bool operator==(const ReplayStep &) const noexcept = default;
};

struct ReplayResult
{
    std::uint64_t initial_hash;
    std::vector<ReplayStep> steps;
    GameState state;
};

// Version 6 binds the current rules, synthetic fixture and hash encoding v3.
// Every attempted command has an entry, including refusals after sector end.
[[nodiscard]] inline std::optional<ReplayResult>
run_replay(std::uint32_t version, RunSeed seed,
           std::span<const Command> commands)
{
    if (version != replay_version)
    {
        return std::nullopt;
    }

    auto state = seeded_fixture(seed);
    ReplayResult result{state_hash(state), {}, std::move(state)};
    result.steps.reserve(commands.size());
    for (const auto &command : commands)
    {
        auto turn = resolve_turn(result.state, command);
        result.steps.push_back(
            {turn.accepted, turn.outcome, state_hash(turn.state)});
        result.state = std::move(turn.state);
    }
    return result;
}

} // namespace closed_circuit::developer
