#pragma once
#include "application/run_checkpoint.hpp"

namespace closed_circuit
{
// Serial requests from one session. True means the boundary is durable: the
// checkpoint is published, or no saved run remains. False may be retried and
// never implies that the previous saved run was lost.
class CheckpointStore
{
  public:
    virtual ~CheckpointStore() = default;
    [[nodiscard]] virtual bool save(const RunCheckpoint &checkpoint) = 0;
    [[nodiscard]] virtual bool clear() = 0;
};
} // namespace closed_circuit
