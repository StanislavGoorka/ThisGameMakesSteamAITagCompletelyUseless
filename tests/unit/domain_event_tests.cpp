#include "../support/achievement_fakes.hpp"
#include "application/domain_event.hpp"
#include "application/event_dispatcher.hpp"
#include <array>
using namespace closed_circuit;

static_assert(static_cast<unsigned>(DomainEventKind::action_committed) == 1);
static_assert(static_cast<unsigned>(DomainEventKind::packet_delivered) == 2);
static_assert(static_cast<unsigned>(DomainEventKind::sector_completed) == 3);
static_assert(static_cast<unsigned>(DomainEventKind::run_ended) == 4);
static_assert(static_cast<unsigned>(RunEndReason::won) == 1);
static_assert(static_cast<unsigned>(RunEndReason::death) == 2);
static_assert(static_cast<unsigned>(RunEndReason::overload) == 3);

int main()
{
    const DomainEventKey key{RunSeed{UINT64_MAX}, *SectorNumber::from_value(3),
                             TurnIndex{48}, 0};
    const auto target = *NodeId::from_value(48);
    const std::array<DomainEventPayload, 4> payloads{
        ActionCommitted{OverdriveCommand{target}}, PacketDelivered{target},
        SectorCompleted{}, RunEnded{RunEndReason::won}};
    for (std::size_t i = 0; i < payloads.size(); ++i)
    {
        const DomainEvent original{key, payloads[i]};
        auto copy = original;
        if (copy != original || static_cast<unsigned>(copy.kind()) != i + 1)
            return 1;
        copy.key.ordinal = 1;
        if (copy == original || !(original.key < copy.key))
            return 1;
        copy = original;
        copy.key.seed = RunSeed{0};
        if (copy == original || original.key.seed != RunSeed{UINT64_MAX})
            return 1;
        copy = original;
        copy.payload = RunEnded{RunEndReason::death};
        if (copy == original || original.payload != payloads[i])
            return 1;
    }
    for (const Command &command :
         {Command{MoveCommand{target}}, Command{RotateCommand{target}},
          Command{WaitCommand{}}, Command{OverdriveCommand{target}}})
    {
        const DomainEvent original{key, ActionCommitted{command}};
        auto copy = original;
        std::get<ActionCommitted>(copy.payload).command = WaitCommand{};
        if (std::get<ActionCommitted>(original.payload).command != command ||
            copy.kind() != DomainEventKind::action_committed)
            return 1;
    }
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    test::CountingEvaluator evaluator;
    test::RecordingSink sink;
    EventDispatcher dispatcher(scope, evaluator, sink);
    DomainEvent evidence{{scope.seed, scope.first_sector, TurnIndex{1}, 0},
                         ActionCommitted{RotateCommand{target}, target}};
    auto absent = evidence;
    std::get<ActionCommitted>(absent.payload)
        .reconfigured_packet_target.reset();
    if (absent == evidence ||
        ActionCommitted{WaitCommand{}}.reconfigured_packet_target)
        return 1;
    auto invalid = evidence;
    std::get<ActionCommitted>(invalid.payload).command = WaitCommand{};
    if (dispatcher.dispatch(scope, invalid) != DispatchResult::invalid_event ||
        evaluator.calls != 0 || !sink.calls.empty() ||
        dispatcher.dispatch(scope, evidence) != DispatchResult::delivered ||
        dispatcher.dispatch(scope, absent) != DispatchResult::conflict ||
        dispatcher.dispatch(scope, evidence) != DispatchResult::delivered ||
        evaluator.calls != 1 || !sink.calls.empty())
        return 1;
    return 0;
}
