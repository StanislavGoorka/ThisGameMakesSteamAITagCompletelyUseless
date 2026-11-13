#pragma once
#include "application/run_checkpoint.hpp"

namespace closed_circuit
{
enum class CheckpointAction : std::uint8_t
{
    keep,
    save,
    clear
};

struct CheckpointBoundary
{
    CheckpointAction action;
    // Present exactly when action is save.
    std::optional<RunCheckpoint> checkpoint;
};

// Safe after any accepted change: only an exact sector start saves, a finished
// run clears (death ends the run), everything else keeps the last boundary.
[[nodiscard]] inline CheckpointBoundary checkpoint_boundary(const RunState &run)
{
    switch (run_phase(run))
    {
    case RunPhase::won:
    case RunPhase::lost:
        return {CheckpointAction::clear, std::nullopt};
    case RunPhase::playing:
        if (auto checkpoint = capture_checkpoint(run))
            return {CheckpointAction::save, checkpoint};
        return {CheckpointAction::keep, std::nullopt};
    default:
        return {CheckpointAction::keep, std::nullopt};
    }
}
} // namespace closed_circuit
