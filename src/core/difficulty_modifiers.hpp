#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

namespace closed_circuit
{
// Stable internal bits of difficulty modifiers, never player-facing labels.
// DIFFICULTY_MODIFIER_PLAN defines the three bits; each is independent.
enum class DifficultyModifier : std::uint8_t
{
    short_turn_limit = 1,
    lean_delivery = 2,
    heavy_overdrive_debt = 4
};

// Only implemented modifiers; a planned bit stays unknown until added here.
inline constexpr std::array<DifficultyModifier, 0>
    difficulty_modifier_catalog{};

[[nodiscard]] constexpr std::uint8_t implemented_difficulty_bits() noexcept
{
    std::uint8_t bits = 0;
    for (const auto modifier : difficulty_modifier_catalog)
        bits = static_cast<std::uint8_t>(bits |
                                         static_cast<std::uint8_t>(modifier));
    return bits;
}

// A set of implemented modifiers. The default empty set is exactly the
// unmodified game; only the factory creates any other value.
class DifficultyModifiers
{
  public:
    constexpr DifficultyModifiers() noexcept = default;

    template <std::integral Integer>
        requires(!std::same_as<Integer, bool>)
    [[nodiscard]] static constexpr std::optional<DifficultyModifiers>
    from_value(Integer value) noexcept
    {
        if (!std::in_range<std::uint8_t>(value))
            return std::nullopt;
        const auto bits = static_cast<std::uint8_t>(value);
        if ((bits & ~implemented_difficulty_bits()) != 0)
            return std::nullopt;
        return DifficultyModifiers{bits};
    }

    [[nodiscard]] constexpr std::uint8_t bits() const noexcept
    {
        return bits_;
    }
    [[nodiscard]] constexpr bool
    contains(DifficultyModifier modifier) const noexcept
    {
        return (bits_ & static_cast<std::uint8_t>(modifier)) != 0;
    }
    [[nodiscard]] constexpr bool
    operator==(const DifficultyModifiers &) const noexcept = default;

  private:
    explicit constexpr DifficultyModifiers(std::uint8_t bits) noexcept
        : bits_{bits}
    {
    }
    std::uint8_t bits_{0};
};
} // namespace closed_circuit
