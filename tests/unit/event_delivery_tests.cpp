#include "application/event_delivery.hpp"
#include <type_traits>
using namespace closed_circuit;
static_assert(!std::is_convertible_v<RunAttemptToken, RunBranchToken>);
static_assert(std::has_virtual_destructor_v<AchievementEvaluator>);

class FakeEvaluator final : public AchievementEvaluator
{
  public:
    [[nodiscard]] std::uint16_t
    evaluate(const DomainEvent &event) noexcept override
    {
        ++calls;
        last = event;
        return candidates;
    }
    unsigned calls = 0;
    std::optional<DomainEvent> last;
    std::uint16_t candidates = 0;
};

int main()
{
    for (unsigned raw = 0; raw <= UINT16_MAX; ++raw)
        if (is_candidate_mask_valid(static_cast<std::uint16_t>(raw)) !=
            (raw <= 31))
            return 1;
    const DeliveryScope scope{
        RunSeed{0}, {1}, {2}, *SectorNumber::from_value(1)};
    auto copy = scope;
    if (!is_delivery_scope_valid(scope) || scope != copy)
        return 1;
    copy.attempt.value = 0;
    if (is_delivery_scope_valid(copy) || copy == scope)
        return 1;
    copy = scope;
    copy.branch.value = 0;
    if (is_delivery_scope_valid(copy) || copy == scope)
        return 1;
    copy = scope;
    copy.seed = RunSeed{UINT64_MAX};
    copy.attempt.value = copy.branch.value = UINT64_MAX;
    copy.first_sector = *SectorNumber::from_value(3);
    if (!is_delivery_scope_valid(copy) || scope == copy ||
        scope.seed != RunSeed{0})
        return 1;
    copy = scope;
    ++copy.attempt.value;
    if (copy == scope)
        return 1;
    copy = scope;
    ++copy.branch.value;
    if (copy == scope)
        return 1;
    copy = scope;
    copy.first_sector = *SectorNumber::from_value(2);
    if (copy == scope)
        return 1;
    FakeEvaluator fake;
    AchievementEvaluator &port = fake;
    const DomainEvent event{{scope.seed, scope.first_sector, TurnIndex{1}, 0},
                            ActionCommitted{WaitCommand{}}};
    const auto before = event;
    fake.candidates = 0x12;
    return port.evaluate(event) == 0x12 && fake.calls == 1 &&
                   fake.last == event && event == before
               ? 0
               : 1;
}
