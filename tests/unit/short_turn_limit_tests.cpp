#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_checkpoint.hpp"
#include "application/session_flow.hpp"
#include "core/turn_preview.hpp"
#include "corpus/baseline_witness.hpp"
#include "corpus/final_witness.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>

using namespace closed_circuit;
using namespace closed_circuit::developer;

namespace
{
bool check(bool condition, const char *message)
{
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

NodeId node(int value)
{
    return *NodeId::from_value(value);
}

const auto short_limit = *DifficultyModifiers::from_value(1);

// Exit 0 and the player's node 1 share an edge; both receivers are powered.
GameState ready_board(bool modified, std::uint32_t turn)
{
    GameState state{.player_position = node(1),
                    .sector_entry = node(8),
                    .sector_exit = node(0)};
    state.ports[0] = *PortMask::from_bits(PortMask::east_bit);
    state.ports[1] = *PortMask::from_bits(PortMask::west_bit);
    state.receiver_nodes[24] = state.receiver_nodes[48] = true;
    state.powered_nodes = state.receiver_nodes;
    state.turns_taken = TurnIndex{turn};
    if (modified)
        state.difficulty = short_limit;
    return state;
}

bool boundaries()
{
    if (!check(sector_turn_limit(ready_board(false, 0)) == TurnIndex{48} &&
                   sector_turn_limit(ready_board(true, 0)) == TurnIndex{32},
               "Limit must be 48, or 32 with short_turn_limit"))
        return false;
    constexpr std::uint32_t turns[]{
        0, 31, 32, 33, 47, 48, 49, std::numeric_limits<std::uint32_t>::max()};
    for (const bool modified : {false, true})
    {
        const std::uint32_t limit = modified ? 32 : 48;
        for (const auto turn : turns)
            for (int position = 0; position < 3; ++position)
            {
                auto state = ready_board(modified, turn);
                // On the exit, off the exit, or dead on the exit.
                state.player_position = node(position == 1 ? 1 : 0);
                if (position == 2)
                    state.player_integrity = 0;
                auto expected = turn >= limit ? SectorOutcome::overload
                                              : SectorOutcome::ongoing;
                if (position == 0 && turn <= limit)
                    expected = SectorOutcome::completed;
                if (position == 2)
                    expected = SectorOutcome::death;
                if (sector_outcome(state) != expected)
                {
                    std::cerr << "modified=" << modified << " turn=" << turn
                              << " position=" << position << '\n';
                    return false;
                }
            }
    }
    return true;
}

bool turns()
{
    for (const bool modified : {false, true})
    {
        // Completing on the last turn wins; waiting on it overloads.
        const std::uint32_t last = modified ? 31 : 47;
        const auto state = ready_board(modified, last);
        const auto win = resolve_turn(state, MoveCommand{node(0)});
        const auto wait = resolve_turn(state, WaitCommand{});
        const auto preview = preview_turn(state, WaitCommand{});
        if (!check(win.accepted && win.outcome == SectorOutcome::completed &&
                       wait.accepted &&
                       wait.outcome == SectorOutcome::overload &&
                       wait.state.turns_taken == TurnIndex{last + 1} &&
                       preview.outcome == wait.outcome &&
                       test::same_state(preview.state, wait.state),
                   "The last turn must win or overload"))
            return false;
        // A refused command keeps the last turn available.
        const auto refused = resolve_turn(state, MoveCommand{node(2)});
        if (!check(!refused.accepted && test::same_state(refused.state, state),
                   "A refused command must not use the last turn"))
            return false;
        // The modified board overloads on turn 32; the plain one goes on.
        const auto early = ready_board(modified, 31);
        const auto later = resolve_turn(early, WaitCommand{});
        if (!check(later.outcome == (modified ? SectorOutcome::overload
                                              : SectorOutcome::ongoing),
                   "Turn 32 must overload only with short_turn_limit"))
            return false;
        if (modified &&
            !check(!resolve_turn(later.state, WaitCommand{}).accepted,
                   "An overloaded sector refuses commands"))
            return false;
    }
    return true;
}

// Every corpus certificate (13..22 normal turns) still completes with bit1.
bool certificates()
{
    std::size_t normal_maximum = 0;
    std::size_t final_maximum = 0;
    for (std::uint64_t index = 0; index <= 1000; ++index)
    {
        const RunSeed seed{index == 1000 ? UINT64_MAX : index};
        const auto initial = sector_fixture(seed);
        const auto witness = build_baseline_witness(seed, initial);
        auto modified = initial;
        modified.difficulty = short_limit;
        if (!witness || witness->size() > 32 ||
            !validate_solution(modified, *witness))
        {
            std::cerr << "Normal certificate fails at index " << index << '\n';
            return false;
        }
        normal_maximum = std::max(normal_maximum, witness->size());
        if (index > 256 && index != 1000)
            continue;
        const auto final_initial =
            *build_sector_state(seed, SectorVariant::final);
        const auto final_witness = build_final_witness(seed, final_initial);
        auto final_modified = final_initial;
        final_modified.difficulty = short_limit;
        if (!final_witness || final_witness->size() > 32 ||
            !validate_solution(final_modified, *final_witness))
        {
            std::cerr << "Final certificate fails at index " << index << '\n';
            return false;
        }
        final_maximum = std::max(final_maximum, final_witness->size());
    }
    std::cout << "maximum certificate turns: normal " << normal_maximum
              << ", final " << final_maximum << '\n';
    return true;
}

bool persistence()
{
    for (const auto seed : {RunSeed{0}, RunSeed{1}, RunSeed{UINT64_MAX}})
        for (const auto config : start_config_catalog)
        {
            const auto run = *start_run(seed, config, short_limit);
            const auto checkpoint = capture_checkpoint(run);
            const auto bytes =
                checkpoint ? encode_checkpoint(*checkpoint) : std::nullopt;
            if (!check(bytes && (*bytes)[31] == 1 &&
                           decode_checkpoint(*bytes) == checkpoint,
                       "Bit1 must round-trip through byte 31"))
                return false;
            auto restored = *restore_checkpoint(*checkpoint);
            if (!check(test::same_run(restored, run) &&
                           sector_turn_limit(restored.sector) == TurnIndex{32},
                       "Restore must keep short_turn_limit"))
                return false;
            // Resume, lose by overload on turn 32, restart with the bit.
            restored.sector.turns_taken = TurnIndex{32};
            SessionFlow resumed{true};
            if (!check(resumed.screen(restored) == SessionScreen::result &&
                           resumed.confirm(restored) &&
                           test::same_run(restored, run),
                       "Restart after resume must keep short_turn_limit"))
                return false;
        }
    return true;
}
} // namespace

int main()
{
    return boundaries() && turns() && certificates() && persistence() ? 0 : 1;
}
