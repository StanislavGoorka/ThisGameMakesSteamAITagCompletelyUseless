#include "../support/achievement_fakes.hpp"
#include "application/event_dispatcher.hpp"
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_copy_constructible_v<EventDispatcher>);
static_assert(!std::is_move_constructible_v<EventDispatcher>);

DeliveryScope scope(int sector = 1)
{
    return {RunSeed{91}, {1}, {1}, *SectorNumber::from_value(sector)};
}
DomainEvent event(unsigned turn, unsigned ordinal = 0,
                  DomainEventPayload payload = ActionCommitted{WaitCommand{}},
                  int sector = 1)
{
    return {{RunSeed{91}, *SectorNumber::from_value(sector), TurnIndex{turn},
             static_cast<std::uint8_t>(ordinal)},
            payload};
}

bool retries()
{
    test::CountingEvaluator evaluator;
    evaluator.candidates = 7;
    test::RecordingSink sink;
    sink.fail_id = AchievementId::first_sector;
    EventDispatcher dispatcher(scope(), evaluator, sink);
    const auto first = event(1);
    if (dispatcher.dispatch(scope(), first) != DispatchResult::retry ||
        evaluator.calls != 1 || sink.profile.achievement_bits != 1 ||
        sink.calls != std::vector{AchievementId::first_delivery,
                                  AchievementId::first_sector} ||
        dispatcher.dispatch(scope(), event(2)) != DispatchResult::retry ||
        evaluator.calls != 1)
        return false;
    evaluator.candidates = 31; // A retry must use the cached mask7.
    sink.failure = AchievementSinkResult::invalid;
    if (dispatcher.dispatch(scope(), first) != DispatchResult::retry ||
        evaluator.calls != 1)
        return false;
    sink.fail_id.reset();
    if (dispatcher.dispatch(scope(), first) != DispatchResult::delivered ||
        evaluator.calls != 1 || sink.profile.achievement_bits != 7 ||
        sink.calls != std::vector{AchievementId::first_delivery,
                                  AchievementId::first_sector,
                                  AchievementId::first_sector,
                                  AchievementId::first_sector,
                                  AchievementId::run_without_overdrive})
        return false;
    const auto calls = sink.calls;
    for (int i = 0; i < 10; ++i)
        if (dispatcher.dispatch(scope(), first) != DispatchResult::delivered ||
            evaluator.calls != 1 || sink.calls != calls)
            return false;
    evaluator.candidates = 0;
    return dispatcher.dispatch(scope(), event(2)) ==
               DispatchResult::delivered &&
           evaluator.calls == 2 && sink.calls == calls;
}

bool exception_and_invalid_mask()
{
    test::CountingEvaluator evaluator;
    evaluator.candidates = 3;
    test::RecordingSink sink;
    sink.throw_after_id = AchievementId::first_sector;
    EventDispatcher dispatcher(scope(), evaluator, sink);
    bool caught = false;
    try
    {
        static_cast<void>(dispatcher.dispatch(scope(), event(1)));
    }
    catch (const std::runtime_error &)
    {
        caught = true;
    }
    if (!caught || evaluator.calls != 1 || sink.profile.achievement_bits != 3 ||
        dispatcher.dispatch(scope(), event(1)) != DispatchResult::delivered ||
        evaluator.calls != 1 ||
        sink.calls != std::vector{AchievementId::first_delivery,
                                  AchievementId::first_sector,
                                  AchievementId::first_sector})
        return false;
    test::CountingEvaluator invalid;
    invalid.candidates = 32;
    test::RecordingSink untouched;
    EventDispatcher broken(scope(), invalid, untouched);
    if (broken.dispatch(scope(), event(1)) !=
        DispatchResult::invalid_candidates)
        return false;
    invalid.candidates = 1;
    return broken.dispatch(scope(), event(1)) ==
               DispatchResult::invalid_candidates &&
           broken.dispatch(scope(), event(2)) ==
               DispatchResult::invalid_candidates &&
           invalid.calls == 1 && untouched.calls.empty();
}

bool validation()
{
    test::CountingEvaluator evaluator;
    test::RecordingSink sink;
    EventDispatcher dispatcher(scope(), evaluator, sink);
    for (int field = 0; field < 4; ++field)
    {
        auto wrong = scope();
        if (field == 0)
            ++wrong.attempt.value;
        if (field == 1)
            ++wrong.branch.value;
        if (field == 2)
            wrong.seed = RunSeed{92};
        if (field == 3)
            wrong.first_sector = *SectorNumber::from_value(2);
        if (dispatcher.dispatch(wrong, event(1)) != DispatchResult::wrong_scope)
            return false;
    }
    auto invalid_scope = scope();
    invalid_scope.attempt.value = 0;
    EventDispatcher invalid(invalid_scope, evaluator, sink);
    if (invalid.dispatch(invalid_scope, event(1)) !=
        DispatchResult::wrong_scope)
        return false;
    for (const auto &bad :
         {event(0), event(49), event(1, 1),
          event(1, 4, PacketDelivered{*NodeId::from_value(0)}),
          event(1, 0, SectorCompleted{}),
          event(1, 1, RunEnded{static_cast<RunEndReason>(0)}),
          event(1, 1, RunEnded{RunEndReason::won})})
        if (dispatcher.dispatch(scope(), bad) != DispatchResult::invalid_event)
            return false;
    auto other_seed = event(1);
    other_seed.key.seed = RunSeed{92};
    if (dispatcher.dispatch(scope(), other_seed) !=
            DispatchResult::invalid_event ||
        dispatcher.dispatch(scope(), event(2)) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(),
                            event(1, 0, ActionCommitted{WaitCommand{}}, 2)) !=
            DispatchResult::out_of_order ||
        evaluator.calls != 0 || !sink.calls.empty())
        return false;
    auto first = event(1);
    const auto copy = first;
    if (dispatcher.dispatch(scope(), first) != DispatchResult::delivered)
        return false;
    first.payload = ActionCommitted{RotateCommand{*NodeId::from_value(0)}};
    if (dispatcher.dispatch(scope(), first) != DispatchResult::conflict ||
        dispatcher.dispatch(scope(), copy) != DispatchResult::delivered ||
        evaluator.calls != 1 ||
        dispatcher.dispatch(scope(), event(3)) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(), event(1, 2, SectorCompleted{})) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(), event(1, 0, ActionCommitted{WaitCommand{}},
                                           2)) != DispatchResult::out_of_order)
        return false;
    if (dispatcher.dispatch(scope(), event(1, 1, SectorCompleted{})) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope(),
                            event(1, 2, RunEnded{RunEndReason::death})) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(), event(2)) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(),
                            event(1, 0, ActionCommitted{WaitCommand{}}, 3)) !=
            DispatchResult::out_of_order ||
        dispatcher.dispatch(scope(), event(1, 0, ActionCommitted{WaitCommand{}},
                                           2)) != DispatchResult::delivered)
        return false;
    return evaluator.calls == 3 && sink.calls.empty();
}

bool full_stream()
{
    test::CountingEvaluator evaluator;
    evaluator.candidates = 31;
    test::RecordingSink sink;
    EventDispatcher dispatcher(scope(), evaluator, sink);
    std::vector<DomainEvent> trace;
    for (int sector = 1; sector <= 3; ++sector)
        for (unsigned turn = 1; turn <= 48; ++turn)
        {
            trace.push_back(
                event(turn, 0, ActionCommitted{WaitCommand{}}, sector));
            if (turn == 48)
            {
                trace.push_back(event(
                    turn, 1, PacketDelivered{*NodeId::from_value(48)}, sector));
                trace.push_back(event(turn, 2, SectorCompleted{}, sector));
                if (sector == 3)
                    trace.push_back(
                        event(turn, 3, RunEnded{RunEndReason::won}, sector));
            }
        }
    for (const auto &value : trace)
        if (dispatcher.dispatch(scope(), value) != DispatchResult::delivered)
            return false;
    const auto calls = sink.calls;
    for (int repeat = 0; repeat < 3; ++repeat)
        for (const auto &value : trace)
            if (dispatcher.dispatch(scope(), value) !=
                DispatchResult::delivered)
                return false;
    if (evaluator.calls != trace.size() || sink.calls != calls ||
        sink.profile.achievement_bits != 31)
        return false;
    test::CountingEvaluator terminal_evaluator;
    EventDispatcher terminal(scope(3), terminal_evaluator, sink);
    if (terminal.dispatch(scope(3), event(1, 0, ActionCommitted{WaitCommand{}},
                                          3)) != DispatchResult::delivered ||
        terminal.dispatch(scope(3), event(1, 1, RunEnded{RunEndReason::won},
                                          3)) != DispatchResult::out_of_order ||
        terminal.dispatch(scope(3), event(1, 1, RunEnded{RunEndReason::death},
                                          3)) != DispatchResult::delivered ||
        terminal.dispatch(scope(3), event(2, 0, ActionCommitted{WaitCommand{}},
                                          3)) != DispatchResult::out_of_order)
        return false;
    return terminal_evaluator.calls == 2;
}
int main()
{
    return retries() && exception_and_invalid_mask() && validation() &&
                   full_stream()
               ? 0
               : 1;
}
