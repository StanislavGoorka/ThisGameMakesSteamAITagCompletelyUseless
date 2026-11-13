#include "../support/achievement_fakes.hpp"
#include "application/event_dispatcher.hpp"
#include "application/mvp_achievement_evaluator.hpp"
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
        if (evaluator.evaluate(event(payload)) != 0)
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
int main()
{
    return delivery() ? 0 : 1;
}
