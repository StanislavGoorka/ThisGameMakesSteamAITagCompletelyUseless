#pragma once

#include "core/sector_boundary.hpp"
#include "corpus/baseline_witness.hpp"

namespace closed_circuit::developer
{
// Only the exact standalone final baseline is supported, without inherited
// resources, modules or hazards. Production run selection remains separate.
[[nodiscard]] inline std::optional<std::vector<Command>>
build_final_witness(RunSeed seed, const GameState &initial)
{
    return witness_detail::build_exact_perimeter_witness(
        initial, *build_sector_state(seed, SectorVariant::final));
}
} // namespace closed_circuit::developer
