#include "../support/achievement_fakes.hpp"
#include "application/event_dispatcher.hpp"
#include "application/mvp_achievement_evaluator.hpp"
#include "application/run_turn_events.hpp"
using namespace closed_circuit;
DomainEvent event(DomainEventPayload payload, unsigned turn = 1,
                  unsigned ordinal = 0, int sector = 1)
{
    return {{RunSeed{1}, *SectorNumber::from_value(sector), TurnIndex{turn},
             static_cast<std::uint8_t>(ordinal)},
            payload};
}
bool delivery()
{
    MvpAchievementEvaluator evaluator;
    for (int sector = 1; sector <= 3; ++sector)
        for (int target = 0; target < 49; ++target)
            if (evaluator.evaluate(
                    event(PacketDelivered{*NodeId::from_value(target)}, 48, 1,
                          sector)) != 1)
                return false;
    for (const DomainEventPayload &payload :
         {DomainEventPayload{ActionCommitted{WaitCommand{}}},
          DomainEventPayload{SectorCompleted{}},
          DomainEventPayload{RunEnded{RunEndReason::won}},
          DomainEventPayload{RunEnded{RunEndReason::death}},
          DomainEventPayload{RunEnded{RunEndReason::overload}}})
        if ((evaluator.evaluate(event(payload)) & 1) != 0)
            return false;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    test::RecordingSink sink;
    sink.fail_id = AchievementId::first_delivery;
    EventDispatcher dispatcher(scope, evaluator, sink);
    const auto action = event(ActionCommitted{WaitCommand{}});
    const auto delivered = event(PacketDelivered{*NodeId::from_value(8)}, 1, 1);
    if (dispatcher.dispatch(scope, action) != DispatchResult::delivered ||
        dispatcher.dispatch(scope, delivered) != DispatchResult::retry ||
        sink.profile.achievement_bits != 0)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, delivered) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 1)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, delivered) == DispatchResult::delivered &&
           sink.calls == calls;
}
bool sector_completion()
{
    const auto node = [](int n) { return *NodeId::from_value(n); };
    for (int sector = 1; sector <= 3; ++sector)
        for (unsigned turn : {0u, 47u})
            for (int outcome = 0; outcome < 4; ++outcome)
            {
                RunState run{.seed = RunSeed{1},
                             .sector_number = *SectorNumber::from_value(sector),
                             .sector = GameState{.player_position = node(0)}};
                run.sector.ports[0] = *PortMask::from_bits(2);
                run.sector.ports[1] = *PortMask::from_bits(8);
                run.sector.packet = Packet{node(0), node(1)};
                run.sector.receiver_nodes[1] = run.sector.receiver_nodes[7] =
                    true;
                run.sector.powered_nodes[7] = outcome != 3;
                run.sector.sector_exit = node(1);
                run.sector.turns_taken = TurnIndex{turn};
                if (outcome == 1)
                {
                    run.sector.player_integrity = 1;
                    run.sector.pulses.push_back(
                        {*EntityId::from_value(1), node(1)});
                }
                const Command command = outcome == 2
                                            ? Command{WaitCommand{}}
                                            : Command{MoveCommand{node(1)}};
                const auto result = resolve_run_turn_events(run, command);
                MvpAchievementEvaluator evaluator;
                unsigned candidates = 0;
                for (const auto &e : result.events)
                    candidates |= evaluator.evaluate(e);
                if (!result.turn.accepted ||
                    ((candidates & 2) != 0) != (outcome == 0))
                    return false;
            }
    return true;
}
int main()
{
    return delivery() && sector_completion() ? 0 : 1;
}
