#pragma once

namespace closed_circuit
{
// Player-facing outcome of reading the run checkpoint. Platform error codes
// stay in the adapter; presentation only sees this classification.
enum class ResumeStatus
{
    none,
    resumed,
    corrupt,
    unavailable
};
} // namespace closed_circuit
