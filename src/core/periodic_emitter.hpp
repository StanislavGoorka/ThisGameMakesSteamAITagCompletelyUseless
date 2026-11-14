#pragma once
#include "core/identifiers.hpp"

namespace closed_circuit
{
// Logical configuration only. The schedule is derived from completed turns.
class PeriodicEmitter
{
  public:
    [[nodiscard]] static constexpr std::optional<PeriodicEmitter>
    from_values(EntityId id, NodeId position, std::uint64_t period,
                std::uint64_t phase, std::uint64_t range) noexcept
    {
        if (period < 2 || period > 48 || phase >= period || range < 1 ||
            range > 48)
            return std::nullopt;
        return PeriodicEmitter{id, position, static_cast<std::uint8_t>(period),
                               static_cast<std::uint8_t>(phase),
                               static_cast<std::uint8_t>(range)};
    }
    [[nodiscard]] constexpr EntityId id() const noexcept
    {
        return id_;
    }
    [[nodiscard]] constexpr NodeId position() const noexcept
    {
        return position_;
    }
    [[nodiscard]] constexpr std::uint8_t period() const noexcept
    {
        return period_;
    }
    [[nodiscard]] constexpr std::uint8_t phase() const noexcept
    {
        return phase_;
    }
    [[nodiscard]] constexpr std::uint8_t range() const noexcept
    {
        return range_;
    }
    [[nodiscard]] constexpr bool
    fires_next(TurnIndex turns_taken) const noexcept
    {
        // Reduce first: even UINT32_MAX must not wrap before the modulo.
        return (static_cast<std::uint32_t>(turns_taken) % period_ + 1) %
                   period_ ==
               phase_;
    }
    constexpr bool operator==(const PeriodicEmitter &) const noexcept = default;

  private:
    constexpr PeriodicEmitter(EntityId id, NodeId position, std::uint8_t period,
                              std::uint8_t phase, std::uint8_t range) noexcept
        : id_(id), position_(position), period_(period), phase_(phase),
          range_(range)
    {
    }
    EntityId id_;
    NodeId position_;
    std::uint8_t period_, phase_, range_;
};
} // namespace closed_circuit
