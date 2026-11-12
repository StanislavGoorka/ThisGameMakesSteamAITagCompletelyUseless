#include "../support/run_checks.hpp"
#include "application/checkpoint_decode.hpp"
#include "application/checkpoint_encode.hpp"
#include "application/run_turn_events.hpp"
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
struct Step
{
    Command command;
    RunState state;
    std::vector<DomainEvent> events;
};
bool apply(RunState &run, const Command &command,
           std::vector<Step> *trace = nullptr)
{
    const auto before = run;
    auto preview = preview_run_turn(run, command);
    auto emitted = resolve_run_turn_events(run, command);
    auto &actual = emitted.turn;
    const auto core = resolve_run_turn(run, command);
    if (!actual.accepted || !preview.accepted ||
        actual.phase != preview.phase ||
        !test::same_run(actual.state, preview.state) ||
        !test::same_run(actual.state, core.state) ||
        actual.phase != core.phase || !core.accepted ||
        !test::same_run(run, before))
        return false;
    std::vector<DomainEventPayload> expected{ActionCommitted{command}};
    for (int i = 0; i < 49; ++i)
        if (!before.sector.powered_nodes[i] &&
            actual.state.sector.powered_nodes[i])
            expected.emplace_back(PacketDelivered{node(i)});
    if (sector_outcome(actual.state.sector) == SectorOutcome::completed)
        expected.emplace_back(SectorCompleted{});
    if (actual.phase == RunPhase::won || actual.phase == RunPhase::lost)
        expected.emplace_back(
            RunEnded{actual.phase == RunPhase::won ? RunEndReason::won
                     : actual.state.sector.player_integrity == 0
                         ? RunEndReason::death
                         : RunEndReason::overload});
    if (emitted.events.size() != expected.size())
        return false;
    for (std::size_t i = 0; i < expected.size(); ++i)
        if (emitted.events[i] != DomainEvent{{run.seed, run.sector_number,
                                              actual.state.sector.turns_taken,
                                              static_cast<std::uint8_t>(i)},
                                             expected[i]})
            return false;
    // Repeated previews have no event channel and cannot change a pending list.
    const auto pending = emitted.events;
    for (int i = 0; i < 3; ++i)
        if (!test::same_run(preview_run_turn(run, command).state, core.state) ||
            emitted.events != pending || !test::same_run(run, before))
            return false;
    if (trace)
        trace->push_back({command, actual.state, emitted.events});
    preview.state.sector.pulses.clear();
    if (!test::same_run(run, before))
        return false;
    run = std::move(actual.state);
    return true;
}
bool solve(RunState &run, bool create_debt, std::vector<Step> *trace)
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
            if (!apply(run, OverdriveCommand{node(at(1))}, trace))
                return false;
    for (int i = 1; i < 13; ++i)
    {
        const auto target = node(at(i));
        const int desired = direction(at(i), at(i - 1)) |
                            (i < 12 ? direction(at(i), at(i + 1)) : 0);
        int rotations = 0;
        while (run.sector.ports[at(i)].bits() != desired && rotations++ < 4)
            if (!apply(run, RotateCommand{target}, trace))
                return false;
        if (run.sector.ports[at(i)].bits() != desired ||
            !apply(run, MoveCommand{target}, trace))
            return false;
        if (create_debt && i == 11)
            if (!apply(run, OverdriveCommand{target}, trace))
                return false;
    }
    while (run_phase(run) == RunPhase::playing &&
           run.sector.turns_taken < TurnIndex{48})
        if (!apply(run, WaitCommand{}, trace))
            return false;
    return sector_outcome(run.sector) == SectorOutcome::completed;
}
bool solve(RunState &run, bool create_debt)
{
    // The second sector includes a paid debt pulse. Capture the actual boundary
    // before any contact wait so replay cannot silently lose it.
    const auto checkpoint = capture_checkpoint(run);
    if (!checkpoint)
        return false;
    const auto bytes = encode_checkpoint(*checkpoint);
    const auto decoded = bytes ? decode_checkpoint(*bytes) : std::nullopt;
    const auto restored = decoded ? restore_checkpoint(*decoded) : std::nullopt;
    if (!restored || !test::same_run(run, *restored))
        return false;
    std::vector<Step> trace;
    if (!run.sector.pulses.empty())
        if (!apply(run, WaitCommand{}, &trace))
            return false;
    if (!solve(run, create_debt, &trace))
        return false;
    for (int repeat = 0; repeat < 2; ++repeat)
    {
        auto replay = *restored;
        for (const auto &step : trace)
        {
            const auto result = resolve_run_turn_events(replay, step.command);
            if (!result.turn.accepted || result.events != step.events ||
                !test::same_run(result.turn.state, step.state))
                return false;
            replay = result.turn.state;
        }
        if (!test::same_run(replay, run))
            return false;
    }
    return true;
}
bool terminal_refuses(const RunState &run, RunPhase phase)
{
    const auto before = run;
    const auto turn = resolve_run_turn(run, WaitCommand{});
    const auto emitted = resolve_run_turn_events(run, WaitCommand{});
    const auto next = advance_run(run);
    auto reward = run;
    return !turn.accepted && !emitted.turn.accepted && emitted.events.empty() &&
           test::same_run(emitted.turn.state, before) && turn.phase == phase &&
           !next.accepted && next.phase == phase &&
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
    if (!solve(run, false) || run.sector.player_integrity != 2 ||
        !run.sector.pulses.empty() || !choose_run_module(run, second))
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
