#include "../support/achievement_fakes.hpp"
#include "application/run_achievements.hpp"
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_copy_constructible_v<RunAchievements>);
static_assert(!std::is_move_constructible_v<RunAchievements>);

DomainEvent event(int sector, unsigned ordinal, DomainEventPayload payload)
{
    return {{RunSeed{91}, *SectorNumber::from_value(sector), TurnIndex{1},
             static_cast<std::uint8_t>(ordinal)},
            payload};
}
int main()
{
    const auto action = ActionCommitted{RotateCommand{*NodeId::from_value(0)},
                                        *NodeId::from_value(4)};
    const std::vector first{
        event(1, 0, action),
        event(1, 1, PacketDelivered{*NodeId::from_value(4)}),
        event(1, 2, SectorCompleted{})};
    const std::vector rest{event(2, 0, ActionCommitted{WaitCommand{}}),
                           event(2, 1, SectorCompleted{}),
                           event(3, 0, ActionCommitted{WaitCommand{}}),
                           event(3, 1, SectorCompleted{}),
                           event(3, 2, RunEnded{RunEndReason::won})};
    for (auto origin :
         {RunHistoryOrigin::fresh_run, RunHistoryOrigin::restored_checkpoint})
    {
        test::RecordingSink sink;
        const DeliveryScope scope{
            RunSeed{91}, {1}, {1}, *SectorNumber::from_value(1)};
        RunAchievements delivery{scope, sink, origin};
        auto prepared = delivery.prepare(first);
        if (delivery.pending() || !sink.calls.empty() || prepared != first ||
            !delivery.flush())
            return 1;
        prepared.clear();
        delivery.publish(delivery.prepare(first));
        sink.fail_id = AchievementId::first_delivery;
        if (delivery.flush() || !delivery.pending() ||
            sink.calls != std::vector{AchievementId::first_delivery})
            return 2;
        auto next = delivery.prepare(rest);
        if (next.size() != first.size() - 1 + rest.size() ||
            !delivery.pending() || sink.calls.size() != 1)
            return 3;
        delivery.publish(std::move(next));
        sink.fail_id.reset();
        sink.throw_after_id = AchievementId::first_delivery;
        if (delivery.flush() || !delivery.pending() ||
            sink.profile.achievement_bits != 1)
            return 4;
        sink.throw_after_id.reset();
        if (!delivery.flush() || delivery.pending() ||
            sink.profile.achievement_bits !=
                (origin == RunHistoryOrigin::fresh_run ? 31 : 27))
            return 5;
        // ID4 proves the delivery was not reevaluated after consuming its
        // reconfiguration evidence on the failed first attempt.
        const auto calls = sink.calls;
        for (int i = 0; i < 3; ++i)
        {
            delivery.publish(delivery.prepare(first));
            if (!delivery.flush() || delivery.pending() || sink.calls != calls)
                return 6;
        }
        RunAchievements another{
            {RunSeed{91}, {2}, {2}, *SectorNumber::from_value(1)},
            sink,
            origin};
        another.publish(another.prepare(first));
        if (!another.flush() || sink.calls.size() == calls.size())
            return 7;
    }
    test::RecordingSink sink;
    RunAchievements wrong{{RunSeed{92}, {1}, {1}, *SectorNumber::from_value(1)},
                          sink,
                          RunHistoryOrigin::fresh_run};
    wrong.publish(wrong.prepare(first));
    if (wrong.flush() || !wrong.pending() || !sink.calls.empty())
        return 8;
    return 0;
}
