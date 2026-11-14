#pragma once

#include "core/run_state.hpp"
#include "core/sector_boundary.hpp"

namespace closed_circuit
{
[[nodiscard]] constexpr SectorVariant
sector_variant_for(SectorNumber number) noexcept
{
    return number.value() == sector_count ? SectorVariant::final
                                          : SectorVariant::normal;
}
} // namespace closed_circuit
