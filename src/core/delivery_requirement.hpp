#pragma once

#include <compare>
#include <concepts>
#include <cstdint>
#include <optional>

namespace closed_circuit
{
class DeliveryRequirement
{
  public:
    constexpr DeliveryRequirement() noexcept = default;
    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<DeliveryRequirement>
    from_value(Integer value) noexcept
    {
        if (value != 2 && value != 3)
            return std::nullopt;
        return DeliveryRequirement{static_cast<std::uint8_t>(value)};
    }
    [[nodiscard]] constexpr std::uint8_t value() const noexcept
    {
        return value_;
    }
    constexpr auto
    operator<=>(const DeliveryRequirement &) const noexcept = default;

  private:
    explicit constexpr DeliveryRequirement(std::uint8_t value) noexcept
        : value_{value}
    {
    }
    std::uint8_t value_{2};
};
} // namespace closed_circuit
