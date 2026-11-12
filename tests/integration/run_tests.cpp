#include "../support/run_checks.hpp"
#include "core/run_reward.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
bool apply(RunState &run, const Command &command)
{
    const auto before = run;
    auto preview = preview_run_turn(run, command);
    auto actual = resolve_run_turn(run, command);
    if (!actual.accepted || !preview.accepted ||
        actual.phase != preview.phase ||
        !test::same_run(actual.state, preview.state) ||
        !test::same_run(run, before))
        return false;
    preview.state.sector.pulses.clear();
    if (!test::same_run(run, before))
        return false;
    run = std::move(actual.state);
    return true;
}
bool solve(RunState &run, bool create_debt)
{
    // Test-only witness for P028's two perimeter paths. Every action uses run
    // API.
    const bool top = run.sector.receiver_nodes[4];
    const auto at = [top](int i)
    {
        return top ? (i <= 6 ? i : 6 + (i - 6) * 7)
                   : (i <= 6 ? i * 7 : 42 + i - 6);
    };
    const auto direction = [](int from, int to) {
        return to == from + 1 ? 2 : to == from - 1 ? 8 : to > from ? 4 : 1;
    };
    if (create_debt)
        for (int i = 0; i < 2; ++i)
            if (!apply(run, OverdriveCommand{node(at(1))}))
                return false;
    for (int i = 1; i < 13; ++i)
    {
        const auto target = node(at(i));
        const int desired = direction(at(i), at(i - 1)) |
                            (i < 12 ? direction(at(i), at(i + 1)) : 0);
        int rotations = 0;
        while (run.sector.ports[at(i)].bits() != desired && rotations++ < 4)
            if (!apply(run, RotateCommand{target}))
                return false;
        if (run.sector.ports[at(i)].bits() != desired ||
            !apply(run, MoveCommand{target}))
            return false;
        if (create_debt && i == 11)
            if (!apply(run, OverdriveCommand{target}))
                return false;
    }
    while (run_phase(run) == RunPhase::playing &&
           run.sector.turns_taken < TurnIndex{48})
        if (!apply(run, WaitCommand{}))
            return false;
    return sector_outcome(run.sector) == SectorOutcome::completed;
}
bool terminal_refuses(const RunState &run, RunPhase phase)
{
    const auto before = run;
    const auto turn = resolve_run_turn(run, WaitCommand{});
    const auto next = advance_run(run);
    auto reward = run;
    return !turn.accepted && turn.phase == phase && !next.accepted &&
           next.phase == phase &&
           !choose_run_module(reward, ModuleId::rotation_range) &&
           test::same_run(turn.state, before) &&
           test::same_run(next.state, before) &&
           test::same_run(reward, before) && test::same_run(run, before);
}
bool complete(std::uint64_t seed, bool range_first, RunState &final)
{
    auto run = start_run(RunSeed{seed});
    if (!solve(run, true) || run.sector.overload_debt != 3 ||
        run.sector.player_integrity != 3 ||
        run_phase(run) != RunPhase::choosing_reward)
        return false;
    const auto first =
        range_first ? ModuleId::rotation_range : ModuleId::terminal_rotation;
    const auto second =
        range_first ? ModuleId::terminal_rotation : ModuleId::rotation_range;
    if (!choose_run_module(run, first))
        return false;
    auto boundary = advance_run(run);
    if (!boundary.accepted || boundary.state.sector_number.value() != 2 ||
        boundary.state.sector.overload_debt != 0 ||
        boundary.state.sector.pulses.size() != 1 ||
        boundary.state.sector.player_integrity != 3 ||
        boundary.state.sector.modules.size() != 1)
        return false;
    // A lethal contact fixture on a real debt boundary; HP1 is explicitly
    // injected.
    auto lethal = boundary.state;
    lethal.sector.player_integrity = 1;
    if (!apply(lethal, WaitCommand{}) || lethal.sector.player_integrity != 0 ||
        !terminal_refuses(lethal, RunPhase::lost))
        return false;
    run = boundary.state;
    if (!apply(run, WaitCommand{}) || run.sector.player_integrity != 2 ||
        !run.sector.pulses.empty() || !solve(run, false) ||
        !choose_run_module(run, second))
        return false;
    boundary = advance_run(run);
    if (!boundary.accepted || boundary.state.sector_number.value() != 3 ||
        boundary.state.sector.player_integrity != 2 ||
        boundary.state.sector.modules.size() != 2 ||
        boundary.state.sector.turns_taken != TurnIndex{0} ||
        !boundary.state.sector.pulses.empty())
        return false;
    run = std::move(boundary.state);
    if (!solve(run, false) || run.sector.player_integrity != 2 ||
        !terminal_refuses(run, RunPhase::won))
        return false;
    final = std::move(run);
    return true;
}
bool cases()
{
    for (const std::uint64_t seed : {0ull, 1ull, 2ull, UINT64_MAX})
        for (const bool order : {false, true})
        {
            auto first = start_run(RunSeed{seed}), second = first;
            if (!complete(seed, order, first) ||
                !complete(seed, order, second) ||
                !test::same_run(first, second))
                return false;
            const auto before = second;
            first.sector.pulses.push_back({*EntityId::from_value(99), node(0)});
            first.sector.modules = {};
            if (!test::same_run(second, before))
                return false;
            auto overload = start_run(RunSeed{seed});
            for (int i = 0; i < 48; ++i)
                if (!apply(overload, WaitCommand{}))
                    return false;
            if (!terminal_refuses(overload, RunPhase::lost) ||
                overload.sector.turns_taken != TurnIndex{48})
                return false;
        }
    return true;
}
} // namespace
int main()
{
    if (!cases())
    {
        std::cerr << "Three-sector integration mismatch\n";
        return 1;
    }
    return 0;
}
