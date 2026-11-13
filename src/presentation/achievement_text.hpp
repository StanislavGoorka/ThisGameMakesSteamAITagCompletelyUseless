#pragma once
#include "application/achievement_notice.hpp"
#include "application/text_line.hpp"

namespace closed_circuit::presentation
{
[[nodiscard]] inline std::optional<TextLine>
achievement_line(std::optional<AchievementId> pending) noexcept
{
    return pending == AchievementId::first_delivery
               ? std::optional{TextLine{"achievement-first-delivery",
                                        std::nullopt}}
               : std::nullopt;
}
[[nodiscard]] inline std::optional<TextLine>
achievement_storage_line(bool unavailable) noexcept
{
    return unavailable ? std::optional{TextLine{"achievement-unavailable",
                                                std::nullopt}}
                       : std::nullopt;
}
} // namespace closed_circuit::presentation
