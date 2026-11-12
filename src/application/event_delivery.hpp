#pragma once

#include "application/domain_event.hpp"

namespace closed_circuit
{
struct RunAttemptToken
{
    std::uint64_t value;
    constexpr auto
    operator<=>(const RunAttemptToken &) const noexcept = default;
};
struct RunBranchToken
{
    std::uint64_t value;
    constexpr auto operator<=>(const RunBranchToken &) const noexcept = default;
};
struct DeliveryScope
{
    RunSeed seed;
    RunAttemptToken attempt;
    RunBranchToken branch;
    SectorNumber first_sector;
    constexpr bool operator==(const DeliveryScope &) const noexcept = default;
};

[[nodiscard]] constexpr bool
is_delivery_scope_valid(const DeliveryScope &scope) noexcept
{
    return scope.attempt.value != 0 && scope.branch.value != 0;
}
[[nodiscard]] constexpr bool
is_candidate_mask_valid(std::uint16_t mask) noexcept
{
    return mask <= 0x1f;
}

enum class DispatchResult
{
    delivered,
    retry,
    wrong_scope,
    invalid_event,
    conflict,
    out_of_order,
    invalid_candidates
};

// One fresh evaluator per delivery scope. Fixed mask: bit ID-1 requests an
// unlock. Evaluation changes only private tracking state, without IO, throwing
// or service calls; the dispatcher owns subsequent sink retries.
class AchievementEvaluator
{
  public:
    virtual ~AchievementEvaluator() = default;
    [[nodiscard]] virtual std::uint16_t
    evaluate(const DomainEvent &event) noexcept = 0;
};
} // namespace closed_circuit
