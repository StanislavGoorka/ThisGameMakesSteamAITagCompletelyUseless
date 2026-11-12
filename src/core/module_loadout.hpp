#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace closed_circuit
{
// Stable internal IDs, never player-facing labels. Zero is not a module.
enum class ModuleId : std::uint8_t
{
    rotation_range = 1,
    terminal_rotation = 2
};

class ModuleLoadout
{
  public:
    static constexpr std::size_t slot_limit = 2;

    [[nodiscard]] constexpr const auto &slots() const noexcept
    {
        return slots_;
    }
    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
        std::size_t count = 0;
        for (const auto slot : slots_)
            count += slot.has_value();
        return count;
    }
    [[nodiscard]] constexpr bool contains(ModuleId id) const noexcept
    {
        for (const auto slot : slots_)
            if (slot == id)
                return true;
        return false;
    }
    // Atomic rejection; occupied slots are always sorted by numeric ID.
    [[nodiscard]] constexpr bool add(ModuleId id) noexcept
    {
        if ((id != ModuleId::rotation_range &&
             id != ModuleId::terminal_rotation) ||
            contains(id) || size() == slot_limit)
            return false;
        slots_[size()] = id;
        if (slots_[1] && *slots_[1] < *slots_[0])
            std::swap(slots_[0], slots_[1]);
        return true;
    }
    [[nodiscard]] constexpr bool
    operator==(const ModuleLoadout &) const noexcept = default;

  private:
    std::array<std::optional<ModuleId>, slot_limit> slots_{};
};
} // namespace closed_circuit
