#pragma once

#include <concepts>
#include <cstdint>
#include <optional>

namespace closed_circuit
{

class PortMask
{
  public:
    constexpr PortMask() noexcept = default;

    static constexpr std::uint8_t north_bit = 0b0001;
    static constexpr std::uint8_t east_bit = 0b0010;
    static constexpr std::uint8_t south_bit = 0b0100;
    static constexpr std::uint8_t west_bit = 0b1000;

    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<PortMask>
    from_bits(Integer bits) noexcept
    {
        if (static_cast<std::uint64_t>(bits) > all_bits)
        {
            return std::nullopt;
        }
        return PortMask{static_cast<std::uint8_t>(bits)};
    }

    [[nodiscard]] constexpr std::uint8_t bits() const noexcept
    {
        return bits_;
    }

    [[nodiscard]] constexpr PortMask rotate_clockwise() const noexcept
    {
        const auto shifted_ports = (bits_ << 1) & all_bits;
        const auto west_to_north = (bits_ & west_bit) >> 3;
        return PortMask{
            static_cast<std::uint8_t>(shifted_ports | west_to_north)};
    }

  private:
    static constexpr std::uint8_t all_bits =
        north_bit | east_bit | south_bit | west_bit;

    explicit constexpr PortMask(std::uint8_t bits) noexcept : bits_{bits}
    {
    }

    std::uint8_t bits_{};
};

} // namespace closed_circuit
