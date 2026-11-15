#pragma once
#include "application/domain_event.hpp"

#include <bit>
#include <cstdint>
#include <span>
#include <variant>

namespace closed_circuit::test
{
// Independent of the evaluator: the swift_sector bit (ID6, 0x20) is due when
// any SectorCompleted occurs within 16 turns (ACHIEVEMENT_EXPANSION_PLAN).
[[nodiscard]] inline std::uint16_t
swift_bit(std::span<const DomainEvent> events)
{
    for (const auto &event : events)
        if (std::holds_alternative<SectorCompleted>(event.payload) &&
            static_cast<std::uint32_t>(event.key.turn) <= 16)
            return 0x20;
    return 0;
}

[[nodiscard]] inline unsigned unlock_count(std::uint16_t mask)
{
    return static_cast<unsigned>(std::popcount(mask));
}
} // namespace closed_circuit::test
