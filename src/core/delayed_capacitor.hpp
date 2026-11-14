#pragma once
#include "core/identifiers.hpp"

namespace closed_circuit
{
class DelayedCapacitor
{
  public:
    [[nodiscard]] static constexpr std::optional<DelayedCapacitor>
    from_values(EntityId id, NodeId position, std::uint64_t delay,
                std::uint64_t range) noexcept
    {
        if (delay < 1 || delay > 48 || range < 1 || range > 48)
            return std::nullopt;
        return DelayedCapacitor{id, position, static_cast<std::uint8_t>(delay),
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
    [[nodiscard]] constexpr std::uint8_t delay() const noexcept
    {
        return delay_;
    }
    [[nodiscard]] constexpr std::uint8_t range() const noexcept
    {
        return range_;
    }
    [[nodiscard]] constexpr std::uint8_t remaining() const noexcept
    {
        return remaining_;
    }

    // One accepted action. Zero is permanently spent, never an underflow.
    [[nodiscard]] constexpr bool advance_fuse() noexcept
    {
        if (remaining_ == 0)
            return false;
        --remaining_;
        return remaining_ == 0;
    }
    constexpr bool
    operator==(const DelayedCapacitor &) const noexcept = default;

  private:
    constexpr DelayedCapacitor(EntityId id, NodeId position, std::uint8_t delay,
                               std::uint8_t range) noexcept
        : id_(id), position_(position), delay_(delay), range_(range),
          remaining_(delay)
    {
    }
    EntityId id_;
    NodeId position_;
    std::uint8_t delay_, range_, remaining_;
};
} // namespace closed_circuit
