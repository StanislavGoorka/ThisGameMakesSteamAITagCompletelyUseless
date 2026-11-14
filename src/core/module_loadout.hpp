#pragma once

#include "core/module_catalog.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace closed_circuit
{
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
        if (!is_known_module(id) || contains(id) || size() == slot_limit)
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

[[nodiscard]] constexpr std::optional<ModuleLoadout>
decode_module_slots(const std::array<std::uint8_t, 2> &bytes) noexcept
{
    ModuleLoadout result;
    std::uint8_t previous = 0;
    bool empty_seen = false;
    for (const auto byte : bytes)
    {
        if (byte == 0)
        {
            empty_seen = true;
            continue;
        }
        if (empty_seen || byte <= previous ||
            !result.add(static_cast<ModuleId>(byte)))
            return std::nullopt;
        previous = byte;
    }
    return result;
}
} // namespace closed_circuit
