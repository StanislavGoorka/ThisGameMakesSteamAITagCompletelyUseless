#include "application/domain_event.hpp"
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
    return 0;
}
