#include "replay/replay_runner.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <span>
#include <vector>

namespace
{

using namespace closed_circuit;
using namespace closed_circuit::developer;

NodeId node(unsigned value)
{
    return *NodeId::from_value(value);
}

bool same_state(const GameState &left, const GameState &right)
{
    for (std::size_t index = 0; index < GameState::node_count; ++index)
    {
        if (left.ports[index].bits() != right.ports[index].bits())
        {
            return false;
        }
    }
    return left.modules == right.modules && left.reserve == right.reserve &&
           left.overload_debt == right.overload_debt &&
           left.rotatable_nodes == right.rotatable_nodes &&
           left.blocked_nodes == right.blocked_nodes &&
           left.player_position == right.player_position &&
           left.player_integrity == right.player_integrity &&
           left.turns_taken == right.turns_taken &&
           left.packet == right.packet &&
           left.powered_nodes == right.powered_nodes &&
           left.receiver_nodes == right.receiver_nodes &&
           left.sector_entry == right.sector_entry &&
           left.sector_exit == right.sector_exit && left.pulses == right.pulses;
}

bool check(bool condition, const char *message)
{
    if (!condition)
    {
        std::cerr << message << '\n';
    }
    return condition;
}

bool unknown_versions_and_empty_replays()
{
    const std::array<Command, 1> commands{WaitCommand{}};
    for (const auto version : {0U, 1U, 2U, 3U, 4U, 6U, UINT32_MAX})
    {
        if (!check(!run_replay(version, RunSeed{0}, commands),
                   "Unknown replay version must have no result"))
        {
            return false;
        }
    }
    struct Vector
    {
        RunSeed seed;
        std::uint64_t hash;
    };
    constexpr std::array vectors{
        Vector{RunSeed{0}, 4293707461699098855ULL},
        Vector{RunSeed{UINT64_MAX}, 1832336761815696689ULL}};
    for (const auto &vector : vectors)
    {
        const auto result = run_replay(5, vector.seed, {});
        if (!check(result && result->initial_hash == vector.hash &&
                       result->steps.empty() &&
                       same_state(result->state, seeded_fixture(vector.seed)),
                   "Empty replay must retain known initial state and hash"))
        {
            return false;
        }
    }
    return true;
}

// Compare each prefix, so intermediate full states as well as trace entries
// have explicit expectations independent of the runner's loop.
bool winning_corpus()
{
    const RunSeed seed{UINT64_MAX};
    const std::vector<Command> commands{
        MoveCommand{node(48)}, RotateCommand{node(0)}, WaitCommand{},
        WaitCommand{},         WaitCommand{},          MoveCommand{node(1)},
        MoveCommand{node(8)},  MoveCommand{node(1)},   RotateCommand{node(1)},
        WaitCommand{}};
    const auto original_commands = commands;
    auto expected = seeded_fixture(seed);
    auto direct = expected;
    std::vector<ReplayStep> expected_steps;
    for (std::size_t index = 0; index < commands.size(); ++index)
    {
        const bool accepted = index >= 2 && index <= 6;
        if (accepted)
        {
            expected.turns_taken =
                TurnIndex{static_cast<std::uint32_t>(expected.turns_taken) + 1};
            if (index == 2)
            {
                expected.player_integrity = 2;
                expected.pulses.clear();
                expected.powered_nodes[1] = true;
                expected.reserve = 3;
            }
            else if (index == 3)
            {
                expected.packet = Packet{node(1), node(8)};
            }
            else if (index == 4)
            {
                expected.packet.reset();
                expected.powered_nodes[8] = true;
            }
            else
            {
                expected.player_position = node(index == 5 ? 1 : 8);
            }
        }
        const auto outcome =
            index >= 6 ? SectorOutcome::completed : SectorOutcome::ongoing;
        expected_steps.push_back({accepted, outcome, state_hash(expected)});
        const auto turn = resolve_turn(direct, commands[index]);
        direct = turn.state;
        const auto result =
            run_replay(5, seed, std::span{commands}.first(index + 1));
        if (!check(result && result->initial_hash == 1832336761815696689ULL &&
                       result->steps == expected_steps &&
                       same_state(result->state, expected) &&
                       same_state(direct, expected) &&
                       turn.accepted == accepted && turn.outcome == outcome &&
                       commands == original_commands,
                   "Replay prefix must match explicit states and resolver"))
        {
            return false;
        }
    }
    return true;
}

bool rotations_and_refusals_continue()
{
    const std::array<Command, 4> commands{RotateCommand{node(48)},
                                          RotateCommand{node(1)},
                                          MoveCommand{node(48)}, WaitCommand{}};
    const auto before = commands;
    const auto result = run_replay(5, RunSeed{0}, commands);
    auto expected = seeded_fixture(RunSeed{0});
    expected.ports[1] = *PortMask::from_bits(12);
    expected.player_integrity = 2;
    expected.pulses.clear();
    expected.powered_nodes[1] = true;
    expected.reserve = 3;
    expected.turns_taken = TurnIndex{1};
    const auto after_rotation = state_hash(expected);
    expected.turns_taken = TurnIndex{2};
    expected.packet = Packet{node(1), node(8)};
    const std::vector<ReplayStep> steps{
        {false, SectorOutcome::ongoing, 4293707461699098855ULL},
        {true, SectorOutcome::ongoing, after_rotation},
        {false, SectorOutcome::ongoing, after_rotation},
        {true, SectorOutcome::ongoing, state_hash(expected)}};
    auto direct = seeded_fixture(RunSeed{0});
    for (std::size_t index = 0; index < commands.size(); ++index)
    {
        const auto turn = resolve_turn(direct, commands[index]);
        if (!check(ReplayStep{turn.accepted, turn.outcome,
                              state_hash(turn.state)} == steps[index],
                   "Rotation corpus must agree with direct resolver"))
        {
            return false;
        }
        direct = turn.state;
    }
    return check(result && result->steps == steps && commands == before &&
                     same_state(result->state, expected) &&
                     same_state(direct, expected),
                 "Invalid actions must retain entries and allow later turns");
}

bool overload_and_terminal_refusals()
{
    std::vector<Command> commands(48, WaitCommand{});
    commands.push_back(MoveCommand{node(1)});
    commands.push_back(RotateCommand{node(1)});
    commands.push_back(WaitCommand{});
    const auto before = commands;
    const auto result = run_replay(5, RunSeed{UINT64_MAX}, commands);
    if (!check(result && result->steps.size() == commands.size(),
               "Replay must retain all attempts beyond sector end"))
    {
        return false;
    }
    auto direct = seeded_fixture(RunSeed{UINT64_MAX});
    for (std::size_t index = 0; index < commands.size(); ++index)
    {
        const auto turn = resolve_turn(direct, commands[index]);
        if (!check(result->steps[index] ==
                       ReplayStep{index < 48,
                                  index < 47 ? SectorOutcome::ongoing
                                             : SectorOutcome::overload,
                                  state_hash(turn.state)},
                   "Overload trace must match direct resolver and limit"))
        {
            return false;
        }
        direct = turn.state;
    }
    auto expected = seeded_fixture(RunSeed{UINT64_MAX});
    expected.turns_taken = TurnIndex{48};
    expected.player_integrity = 2;
    expected.pulses.clear();
    expected.powered_nodes[1] = true;
    expected.reserve = 3;
    expected.powered_nodes[8] = true;
    return check(same_state(result->state, expected) &&
                     same_state(direct, expected) && commands == before &&
                     result->steps[47].hash == result->steps[48].hash &&
                     result->steps[48].hash == result->steps[49].hash &&
                     result->steps[49].hash == result->steps[50].hash,
                 "Terminal commands must preserve full final state and hash");
}

bool overdrive_corpus()
{
    const std::array<Command, 5> commands{
        OverdriveCommand{node(1)}, OverdriveCommand{node(1)},
        OverdriveCommand{node(1)}, OverdriveCommand{node(1)},
        OverdriveCommand{node(1)}};
    const auto result = run_replay(5, RunSeed{0}, commands);
    const std::vector<ReplayStep> expected{
        {true, SectorOutcome::ongoing, 13800570292826013649ULL},
        {true, SectorOutcome::ongoing, 998399918303298855ULL},
        {true, SectorOutcome::ongoing, 12007390772207791631ULL},
        {true, SectorOutcome::ongoing, 8740532863352877095ULL},
        {false, SectorOutcome::ongoing, 8740532863352877095ULL}};
    auto state = seeded_fixture(RunSeed{0});
    state.ports[1] = *PortMask::from_bits(6);
    state.player_integrity = 2;
    state.pulses.clear();
    state.powered_nodes[1] = state.powered_nodes[8] = true;
    state.turns_taken = TurnIndex{4};
    state.reserve = 0;
    state.overload_debt = 4;
    return check(
        result && result->initial_hash == 4293707461699098855ULL &&
            result->steps == expected && same_state(result->state, state),
        "Overdrive replay must match independent hashes and full state");
}
bool replay_values_are_independent()
{
    std::vector<Command> commands{MoveCommand{node(48)}};
    const auto before = commands;
    SplitMix64 external_rng{RunSeed{9}};
    auto expected_rng = external_rng;
    auto first = run_replay(5, RunSeed{0}, commands);
    auto second = run_replay(5, RunSeed{0}, commands);
    if (!check(first && second && first->steps == second->steps &&
                   same_state(first->state, second->state) &&
                   commands == before,
               "Repeated replay must agree without changing commands"))
    {
        return false;
    }
    first->state.pulses[0].position = node(48);
    first->state.pulses.clear();
    first->state.packet = Packet{node(48), node(0)};
    first->state.sector_entry.reset();
    first->state.sector_exit.reset();
    first->state.ports[1] = *PortMask::from_bits(0);
    first->state.reserve = 0;
    first->state.overload_debt = 9;
    first->steps.clear();
    first->initial_hash = 0;
    commands[0] = WaitCommand{};
    const auto third = run_replay(5, RunSeed{0}, before);
    const auto changed_commands = run_replay(5, RunSeed{0}, commands);
    return check(third && changed_commands &&
                     second->initial_hash == third->initial_hash &&
                     second->steps == third->steps &&
                     same_state(second->state, third->state) &&
                     same_state(second->state, seeded_fixture(RunSeed{0})) &&
                     !changed_commands->steps.empty() &&
                     changed_commands->steps[0].accepted &&
                     external_rng.next_u64() == expected_rng.next_u64(),
                 "Replay results must independently own state and trace");
}

} // namespace

int main()
{
    return overdrive_corpus() && unknown_versions_and_empty_replays() &&
                   winning_corpus() && rotations_and_refusals_continue() &&
                   overload_and_terminal_refusals() &&
                   replay_values_are_independent()
               ? 0
               : 1;
}
