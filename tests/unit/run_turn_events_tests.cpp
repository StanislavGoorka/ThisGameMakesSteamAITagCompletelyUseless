#include "../support/run_checks.hpp"
#include "application/run_turn_events.hpp"
using namespace closed_circuit;

template <typename T>
concept HasEvents = requires(T value) { value.events; };
static_assert(!HasEvents<RunResult>);

NodeId node(int value)
{
    return *NodeId::from_value(value);
}

RunState fixture(int number = 1)
{
    RunState run{.seed = RunSeed{91},
                 .sector_number = *SectorNumber::from_value(number),
                 .sector = GameState{.player_position = node(0)}};
    run.sector.ports[0] = *PortMask::from_bits(2);
    run.sector.ports[1] = *PortMask::from_bits(8);
    run.sector.rotatable_nodes[0] = true;
    run.sector.receiver_nodes[1] = run.sector.receiver_nodes[7] = true;
    run.sector.sector_exit = node(1);
    run.sector.packet = Packet{node(0), node(1)};
    return run;
}

bool check(const RunState &run, const Command &command,
           const std::vector<DomainEventPayload> &expected, bool accepted,
           RunPhase phase)
{
    const auto before = run;
    const auto before_command = command;
    const auto core = resolve_run_turn(run, command);
    std::vector<DomainEvent> previous;
    for (unsigned repeat = 0; repeat < 3; ++repeat)
    {
        const auto preview = preview_run_turn(run, command);
        auto actual = resolve_run_turn_events(run, command);
        if (actual.turn.accepted != accepted || actual.turn.phase != phase ||
            core.accepted != accepted || core.phase != phase ||
            !test::same_run(actual.turn.state, core.state) ||
            !test::same_run(preview.state, core.state) ||
            preview.accepted != accepted || preview.phase != phase ||
            !test::same_run(run, before) || command != before_command ||
            actual.events.size() != expected.size() ||
            (repeat != 0 && actual.events != previous))
            return false;
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            const DomainEventKey key{run.seed, run.sector_number,
                                     core.state.sector.turns_taken,
                                     static_cast<std::uint8_t>(i)};
            if (actual.events[i].key != key ||
                actual.events[i].payload != expected[i])
                return false;
        }
        previous = actual.events;
        // Owning results: modifying one output cannot affect another output,
        // the input or a later repeated computation.
        actual.events.clear();
        actual.turn.state.sector.pulses.push_back(
            {*EntityId::from_value(99), node(48)});
        actual.turn.state.sector.powered_nodes.fill(false);
        if (!test::same_run(run, before) ||
            !test::same_run(preview.state, core.state))
            return false;
    }
    return true;
}

int main()
{
    const Command move = MoveCommand{node(1)};
    const Command wait = WaitCommand{};
    const DomainEventPayload delivery = PacketDelivered{node(1)};
    const DomainEventPayload complete = SectorCompleted{};
    for (const Command &command : {move, wait, Command{RotateCommand{node(0)}},
                                   Command{OverdriveCommand{node(0)}}})
    {
        std::vector<DomainEventPayload> events{ActionCommitted{command}};
        if (command == move || command == wait)
            events.push_back(delivery);
        if (!check(fixture(), command, events, true, RunPhase::playing))
            return 1;
    }
    for (const Command &command :
         {Command{MoveCommand{node(48)}}, Command{RotateCommand{node(48)}},
          Command{OverdriveCommand{node(48)}}})
        if (!check(fixture(), command, {}, false, RunPhase::playing))
            return 1;
    auto no_reserve = fixture();
    no_reserve.sector.reserve = 0;
    if (!check(no_reserve, OverdriveCommand{node(0)}, {}, false,
               RunPhase::playing))
        return 1;
    auto no_delivery = fixture();
    no_delivery.sector.powered_nodes[1] = true;
    if (!check(no_delivery, wait, {ActionCommitted{wait}}, true,
               RunPhase::playing))
        return 1;
    no_delivery.sector.packet.reset();
    if (!check(no_delivery, wait, {ActionCommitted{wait}}, true,
               RunPhase::playing))
        return 1;

    for (int number = 1; number <= 3; ++number)
    {
        auto run = fixture(number);
        run.sector.turns_taken = TurnIndex{47};
        run.sector.powered_nodes[7] = true;
        std::vector<DomainEventPayload> events{ActionCommitted{move}, delivery,
                                               complete};
        const auto phase =
            number == 3 ? RunPhase::won : RunPhase::choosing_reward;
        if (number == 3)
            events.emplace_back(RunEnded{RunEndReason::won});
        if (!check(run, move, events, true, phase))
            return 1;
        const auto ended = resolve_run_turn(run, move);
        if (!check(ended.state, wait, {}, false, phase))
            return 1;
        if (number != 3)
        {
            auto ready = ended.state;
            ready.reward_claimed = true;
            if (!check(ready, wait, {}, false, RunPhase::ready_for_next))
                return 1;
        }
        // Last-turn delivery is real even when staying off the exit loses.
        if (!check(run, wait,
                   {ActionCommitted{wait}, delivery,
                    RunEnded{RunEndReason::overload}},
                   true, RunPhase::lost))
            return 1;
        // Delivery precedes damage. Death prevents sector completion even on
        // the exit with every receiver powered, including the final sector.
        run.sector.player_integrity = 1;
        run.sector.pulses = {{*EntityId::from_value(7), node(1)}};
        if (!check(run, move,
                   {ActionCommitted{move}, delivery,
                    RunEnded{RunEndReason::death}},
                   true, RunPhase::lost))
            return 1;
        const auto lost = resolve_run_turn(run, move);
        if (!check(lost.state, wait, {}, false, RunPhase::lost))
            return 1;
    }
    return 0;
}
