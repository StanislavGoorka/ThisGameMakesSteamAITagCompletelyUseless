#pragma once

#include "core/module_offer.hpp"
#include "core/run_state.hpp"

namespace closed_circuit
{
[[nodiscard]] constexpr ModuleOffer reward_offer(const RunState &run) noexcept
{
    return run_phase(run) == RunPhase::choosing_reward
               ? module_offer(run.sector.modules)
               : ModuleOffer{};
}

// Owns only a cursor. Reading or moving it cannot award a module.
class RewardSelection
{
  public:
    void reset() noexcept
    {
        context_.reset();
        index_.reset();
    }
    void sync(const RunState &run) noexcept
    {
        const auto next = context(run);
        if (!context_ || *context_ != next)
        {
            context_ = next;
            index_.reset();
        }
    }
    void move(const RunState &run, bool right) noexcept
    {
        sync(run);
        const auto &offer = context_->offer;
        const std::size_t count = offer[1] ? 2 : (offer[0] ? 1 : 0);
        if (count == 0)
            return;
        index_ = index_ ? (*index_ + (right ? 1 : count - 1)) % count
                        : (right ? 0 : count - 1);
    }
    [[nodiscard]] std::optional<ModuleId>
    candidate(const RunState &run) const noexcept
    {
        if (!context_ || *context_ != context(run) || !index_)
            return std::nullopt;
        return context_->offer[*index_];
    }

  private:
    struct Context
    {
        RunSeed seed;
        SectorNumber sector;
        RunPhase phase;
        ModuleOffer offer;
        bool operator==(const Context &) const = default;
    };
    [[nodiscard]] static Context context(const RunState &run) noexcept
    {
        return {run.seed, run.sector_number, run_phase(run), reward_offer(run)};
    }
    std::optional<Context> context_;
    std::optional<std::size_t> index_;
};
} // namespace closed_circuit
