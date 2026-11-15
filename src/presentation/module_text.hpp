#pragma once
#include "core/module_catalog.hpp"

#include <optional>
#include <string_view>

namespace closed_circuit::presentation
{
// Catalog projection: one registered working text ID per implemented module.
// Unknown values have no text; callers refuse instead of showing a raw ID.
[[nodiscard]] constexpr std::optional<std::string_view>
module_text_id(ModuleId id) noexcept
{
    switch (id)
    {
    case ModuleId::rotation_range:
        return "reward-range";
    case ModuleId::terminal_rotation:
        return "reward-terminal";
    case ModuleId::delivery_reserve:
        return "reward-delivery-reserve";
    case ModuleId::first_overdrive_debt:
        return "reward-first-overdrive";
    case ModuleId::packet_stride:
        return "reward-packet-stride";
    case ModuleId::pulse_cadence:
        return "reward-pulse-cadence";
    case ModuleId::powered_pulse_barrier:
        return "reward-pulse-barrier";
    case ModuleId::entry_insulation:
        return "reward-entry-insulation";
    case ModuleId::delivery_debt_relief:
        return "reward-debt-relief";
    case ModuleId::overdrive_range:
        return "reward-overdrive-range";
    case ModuleId::reverse_rotation:
        return "reward-reverse-rotation";
    case ModuleId::packet_priority:
        return "reward-packet-priority";
    }
    return std::nullopt;
}
} // namespace closed_circuit::presentation
