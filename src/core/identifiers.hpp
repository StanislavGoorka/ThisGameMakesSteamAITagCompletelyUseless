#pragma once

#include <compare>
#include <concepts>
#include <cstdint>
#include <optional>

namespace closed_circuit
{

class NodeId
{
  public:
    using Value = std::uint8_t;
    static constexpr Value max_value = 48;

    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<NodeId>
    from_value(Integer value) noexcept
    {
        // Negative inputs convert to values above max_value.
        if (static_cast<std::uint64_t>(value) > max_value)
        {
            return std::nullopt;
        }
        return NodeId{static_cast<Value>(value)};
    }

    [[nodiscard]] constexpr Value value() const noexcept
    {
        return value_;
    }

    constexpr auto operator<=>(const NodeId &) const noexcept = default;

  private:
    explicit constexpr NodeId(Value value) noexcept : value_{value}
    {
    }

    Value value_;
};

class EntityId
{
  public:
    using Value = std::uint32_t;
    static constexpr Value max_value = UINT32_MAX;

    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<EntityId>
    from_value(Integer value) noexcept
    {
        if (static_cast<std::uint64_t>(value) > max_value)
        {
            return std::nullopt;
        }
        return EntityId{static_cast<Value>(value)};
    }

    [[nodiscard]] constexpr Value value() const noexcept
    {
        return value_;
    }

    constexpr auto operator<=>(const EntityId &) const noexcept = default;

  private:
    explicit constexpr EntityId(Value value) noexcept : value_{value}
    {
    }

    Value value_;
};

// Every 64-bit unsigned value is a valid run seed.
enum class RunSeed : std::uint64_t
{
};

enum class TurnIndex : std::uint32_t
{
};

} // namespace closed_circuit
