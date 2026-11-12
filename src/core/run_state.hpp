#pragma once

#include "core/sector_outcome.hpp"

namespace closed_circuit
{
class SectorNumber
{
  public:
    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<SectorNumber>
    from_value(Integer value) noexcept
    {
        const auto number = static_cast<std::uint64_t>(value);
        if (number < 1 || number > 3)
            return std::nullopt;
        return SectorNumber{static_cast<std::uint8_t>(number)};
    }
    [[nodiscard]] constexpr std::uint8_t value() const noexcept
    {
        return value_;
    }
    constexpr auto operator<=>(const SectorNumber &) const noexcept = default;

  private:
    explicit constexpr SectorNumber(std::uint8_t value) noexcept : value_{value}
    {
    }
    std::uint8_t value_;
};

struct RunState
{
    RunSeed seed;
    SectorNumber sector_number{*SectorNumber::from_value(1)};
    // All inherited resources live here; the run never stores another copy.
    GameState sector;
    bool reward_claimed{false};
};

enum class RunPhase : std::uint8_t
{
    playing,
    choosing_reward,
    ready_for_next,
    won,
    lost
};

// Derive the phase rather than storing another mutable sector outcome.
[[nodiscard]] constexpr RunPhase run_phase(const RunState &run) noexcept
{
    switch (sector_outcome(run.sector))
    {
    case SectorOutcome::death:
    case SectorOutcome::overload:
        return RunPhase::lost;
    case SectorOutcome::ongoing:
        return RunPhase::playing;
    case SectorOutcome::completed:
        if (run.sector_number.value() == 3)
            return RunPhase::won;
        return run.reward_claimed ||
                       run.sector.modules.size() == ModuleLoadout::slot_limit
                   ? RunPhase::ready_for_next
                   : RunPhase::choosing_reward;
    }
    return RunPhase::lost;
}
} // namespace closed_circuit
