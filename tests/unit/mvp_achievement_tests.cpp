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
    for (int sector = 1; sector <= sector_count; ++sector)
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
    for (int sector = 1; sector <= sector_count; ++sector)
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
unsigned finish(MvpAchievementEvaluator &evaluator, int used_sector = 0,
                int first_sector = 1, RunEndReason reason = RunEndReason::won)
{
    unsigned candidates = 0;
    for (int sector = first_sector; sector <= sector_count; ++sector)
    {
        const Command command =
            sector == used_sector
                ? Command{OverdriveCommand{*NodeId::from_value(0)}}
                : Command{WaitCommand{}};
        candidates |=
            evaluator.evaluate(event(ActionCommitted{command}, 1, 0, sector));
        if (sector != sector_count || reason == RunEndReason::won)
            candidates |=
                evaluator.evaluate(event(SectorCompleted{}, 1, 1, sector));
    }
    return candidates | evaluator.evaluate(event(
                            RunEnded{reason}, 1,
                            reason == RunEndReason::won ? 2 : 1, sector_count));
}
bool without_overdrive()
{
    for (int used = 0; used <= sector_count; ++used)
    {
        MvpAchievementEvaluator fresh(RunHistoryOrigin::fresh_run), restored;
        if (((finish(fresh, used) & 4) != 0) != (used == 0) ||
            (finish(restored, used) & 4) != 0)
            return false;
    }
    for (int first = 2; first <= sector_count; ++first)
    {
        MvpAchievementEvaluator invalid_fresh(RunHistoryOrigin::fresh_run),
            restored;
        if ((finish(invalid_fresh, 0, first) & 4) != 0 ||
            (finish(restored, 0, first) & 4) != 0)
            return false;
    }
    for (auto reason : {RunEndReason::death, RunEndReason::overload})
    {
        MvpAchievementEvaluator failed(RunHistoryOrigin::fresh_run);
        if ((finish(failed, 0, 1, reason) & 4) != 0)
            return false;
    }
    MvpAchievementEvaluator incomplete(RunHistoryOrigin::fresh_run);
    static_cast<void>(
        incomplete.evaluate(event(ActionCommitted{WaitCommand{}}, 2)));
    if ((finish(incomplete) & 4) != 0)
        return false;
    MvpAchievementEvaluator old(RunHistoryOrigin::fresh_run);
    static_cast<void>(finish(old, 1));
    MvpAchievementEvaluator next_attempt(RunHistoryOrigin::fresh_run);
    return (finish(next_attempt) & 4) != 0 && (finish(old) & 4) == 0;
}
bool reconfiguration()
{
    for (int target = 0; target < 49; ++target)
        for (bool overdrive : {false, true})
        {
            MvpAchievementEvaluator evaluator;
            const auto node = *NodeId::from_value(target);
            const auto other = *NodeId::from_value((target + 1) % 49);
            const Command command = overdrive ? Command{OverdriveCommand{node}}
                                              : Command{RotateCommand{node}};
            if (evaluator.evaluate(event(ActionCommitted{command, node})) !=
                    0 ||
                evaluator.evaluate(event(PacketDelivered{other}, 1, 1)) != 1 ||
                evaluator.evaluate(
                    event(ActionCommitted{RotateCommand{node}}, 2)) != 0 ||
                evaluator.evaluate(event(PacketDelivered{node}, 2, 1)) != 9 ||
                evaluator.evaluate(event(PacketDelivered{node}, 3, 1)) != 1)
                return false;
            MvpAchievementEvaluator restored;
            if (restored.evaluate(event(PacketDelivered{node})) != 1)
                return false;
            static_cast<void>(
                evaluator.evaluate(event(ActionCommitted{command, node}, 4)));
            static_cast<void>(evaluator.evaluate(
                event(ActionCommitted{WaitCommand{}}, 1, 0, 2)));
            if (evaluator.evaluate(event(PacketDelivered{node}, 1, 1, 2)) != 1)
                return false;
        }
    const auto node = *NodeId::from_value(8);
    MvpAchievementEvaluator evaluator;
    test::RecordingSink sink;
    sink.fail_id = AchievementId::reconfigured_delivery;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    EventDispatcher dispatcher(scope, evaluator, sink);
    const auto action = event(ActionCommitted{RotateCommand{node}, node});
    const auto delivered = event(PacketDelivered{node}, 1, 1);
    if (dispatcher.dispatch(scope, action) != DispatchResult::delivered ||
        dispatcher.dispatch(scope, delivered) != DispatchResult::retry ||
        sink.profile.achievement_bits != 1)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, delivered) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 9)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, delivered) == DispatchResult::delivered &&
           sink.calls == calls;
}
bool completed_run()
{
    for (int sector = 1; sector <= sector_count; ++sector)
        for (auto reason :
             {RunEndReason::won, RunEndReason::death, RunEndReason::overload})
        {
            MvpAchievementEvaluator evaluator;
            if (evaluator.evaluate(event(RunEnded{reason}, 48, 2, sector)) !=
                (sector == sector_count && reason == RunEndReason::won ? 16
                                                                       : 0))
                return false;
        }
    MvpAchievementEvaluator evaluator(RunHistoryOrigin::fresh_run);
    test::RecordingSink sink;
    sink.fail_id = AchievementId::completed_run;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    EventDispatcher dispatcher(scope, evaluator, sink);
    for (int sector = 1; sector <= sector_count; ++sector)
        for (const auto &e :
             {event(ActionCommitted{WaitCommand{}}, 1, 0, sector),
              event(SectorCompleted{}, 1, 1, sector)})
            if (dispatcher.dispatch(scope, e) != DispatchResult::delivered)
                return false;
    const auto won = event(RunEnded{RunEndReason::won}, 1, 2, sector_count);
    // Turn-one completions are also swift (0x20).
    if (dispatcher.dispatch(scope, won) != DispatchResult::retry ||
        sink.profile.achievement_bits != 0x26)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, won) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 0x36)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, won) == DispatchResult::delivered &&
           sink.calls == calls;
}
bool swift_sector()
{
    // Only SectorCompleted counts, within 16 turns, in every sector and with
    // either history origin; no other event and no later turn qualifies.
    for (const auto origin :
         {RunHistoryOrigin::fresh_run, RunHistoryOrigin::restored_checkpoint})
        for (int sector = 1; sector <= sector_count; ++sector)
            for (unsigned turn = 1; turn <= 48; ++turn)
            {
                MvpAchievementEvaluator evaluator(origin);
                if (evaluator.evaluate(
                        event(SectorCompleted{}, turn, 1, sector)) !=
                    (turn <= 16 ? 0x22u : 2u))
                    return false;
                for (const DomainEventPayload &payload :
                     {DomainEventPayload{ActionCommitted{WaitCommand{}}},
                      DomainEventPayload{
                          PacketDelivered{*NodeId::from_value(1)}},
                      DomainEventPayload{RunEnded{RunEndReason::won}},
                      DomainEventPayload{RunEnded{RunEndReason::death}}})
                    if ((evaluator.evaluate(event(payload, turn, 2, sector)) &
                         0x20) != 0)
                        return false;
            }
    // A real turn: completion on turn 16 qualifies, on turn 17 it does not,
    // and an unfinished turn 16 never does.
    const auto node = [](int n) { return *NodeId::from_value(n); };
    for (unsigned before : {15u, 16u})
        for (bool finish : {true, false})
        {
            RunState run{.seed = RunSeed{1},
                         .sector_number = *SectorNumber::from_value(1),
                         .sector = GameState{.player_position = node(0)}};
            run.sector.ports[0] = *PortMask::from_bits(2);
            run.sector.ports[1] = *PortMask::from_bits(8);
            run.sector.packet = Packet{node(0), node(1)};
            run.sector.receiver_nodes[1] = run.sector.receiver_nodes[7] = true;
            run.sector.powered_nodes[7] = true;
            run.sector.sector_exit = node(1);
            run.sector.turns_taken = TurnIndex{before};
            const Command command =
                finish ? Command{MoveCommand{node(1)}} : Command{WaitCommand{}};
            const auto result = resolve_run_turn_events(run, command);
            MvpAchievementEvaluator evaluator;
            unsigned candidates = 0;
            for (const auto &e : result.events)
                candidates |= evaluator.evaluate(e);
            if (!result.turn.accepted ||
                ((candidates & 0x20) != 0) != (finish && before == 15))
                return false;
        }
    // Dispatch retries only the failed ID and persists bit 5 once.
    MvpAchievementEvaluator evaluator;
    test::RecordingSink sink;
    sink.fail_id = AchievementId::swift_sector;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    EventDispatcher dispatcher(scope, evaluator, sink);
    const auto completed = event(SectorCompleted{}, 1, 1);
    if (dispatcher.dispatch(scope, event(ActionCommitted{WaitCommand{}})) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope, completed) != DispatchResult::retry ||
        sink.profile.achievement_bits != 2)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, completed) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 0x22)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, completed) == DispatchResult::delivered &&
           sink.calls == calls;
}
bool won_preinstalled_range()
{
    // Only a final win of an ID2 run, with either history origin; the other
    // victory bits do not depend on the configuration.
    for (const auto origin :
         {RunHistoryOrigin::fresh_run, RunHistoryOrigin::restored_checkpoint})
        for (const auto config : start_config_catalog)
            for (const auto reason : {RunEndReason::won, RunEndReason::death,
                                      RunEndReason::overload})
                for (int sector = 1; sector <= sector_count; ++sector)
                {
                    MvpAchievementEvaluator evaluator(origin);
                    const unsigned result = evaluator.evaluate(
                        event(RunEnded{reason, config}, 1, 2, sector));
                    const bool won =
                        reason == RunEndReason::won && sector == sector_count;
                    if (((result & 0x40) != 0) !=
                            (won &&
                             config == StartConfigId::preinstalled_range) ||
                        ((result & 0x10) != 0) != won || (result & ~0xd4u) != 0)
                        return false;
                }
    // Dispatch retries only the failed ID7 and persists bit 6 once.
    MvpAchievementEvaluator evaluator;
    test::RecordingSink sink;
    sink.fail_id = AchievementId::won_preinstalled_range;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(sector_count)};
    EventDispatcher dispatcher(scope, evaluator, sink);
    const auto won =
        event(RunEnded{RunEndReason::won, StartConfigId::preinstalled_range}, 1,
              2, sector_count);
    if (dispatcher.dispatch(
            scope, event(ActionCommitted{WaitCommand{}}, 1, 0, sector_count)) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope,
                            event(SectorCompleted{}, 1, 1, sector_count)) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope, won) != DispatchResult::retry ||
        sink.profile.achievement_bits != 0x32)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, won) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 0x72)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, won) == DispatchResult::delivered &&
           sink.calls == calls;
}
bool won_charged_debt()
{
    // Only a final win of an ID3 run, with either history origin.
    for (const auto origin :
         {RunHistoryOrigin::fresh_run, RunHistoryOrigin::restored_checkpoint})
        for (const auto config : start_config_catalog)
            for (const auto reason : {RunEndReason::won, RunEndReason::death,
                                      RunEndReason::overload})
                for (int sector = 1; sector <= sector_count; ++sector)
                {
                    MvpAchievementEvaluator evaluator(origin);
                    const unsigned result = evaluator.evaluate(
                        event(RunEnded{reason, config}, 1, 2, sector));
                    const bool won =
                        reason == RunEndReason::won && sector == sector_count;
                    if (((result & 0x80) != 0) !=
                        (won && config == StartConfigId::charged_debt))
                        return false;
                }
    // Dispatch retries only the failed ID8 and persists bit 7 once.
    MvpAchievementEvaluator evaluator;
    test::RecordingSink sink;
    sink.fail_id = AchievementId::won_charged_debt;
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(sector_count)};
    EventDispatcher dispatcher(scope, evaluator, sink);
    const auto won =
        event(RunEnded{RunEndReason::won, StartConfigId::charged_debt}, 1, 2,
              sector_count);
    if (dispatcher.dispatch(
            scope, event(ActionCommitted{WaitCommand{}}, 1, 0, sector_count)) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope,
                            event(SectorCompleted{}, 1, 1, sector_count)) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope, won) != DispatchResult::retry ||
        sink.profile.achievement_bits != 0x32)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope, won) != DispatchResult::delivered ||
        sink.profile.achievement_bits != 0xb2)
        return false;
    const auto calls = sink.calls;
    return dispatcher.dispatch(scope, won) == DispatchResult::delivered &&
           sink.calls == calls;
}
int main()
{
    return delivery() && sector_completion() && without_overdrive() &&
                   reconfiguration() && completed_run() && swift_sector() &&
                   won_preinstalled_range() && won_charged_debt()
               ? 0
               : 1;
}
