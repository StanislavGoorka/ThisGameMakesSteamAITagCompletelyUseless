#pragma once
#include "application/achievement_sink.hpp"
#include <optional>

namespace closed_circuit
{
// The wrapped durable sink outlives this decorator. No predicate or IO here.
class AchievementNoticeSink final : public AchievementSink
{
  public:
    explicit AchievementNoticeSink(AchievementSink &sink) noexcept : sink_(sink)
    {
    }
    [[nodiscard]] AchievementSinkResult unlock(AchievementId id) override
    {
        const auto result = sink_.unlock(id);
        if (id == AchievementId::first_delivery &&
            result == AchievementSinkResult::unlocked && !announced_)
        {
            announced_ = true;
            pending_ = true;
        }
        return result;
    }
    [[nodiscard]] std::optional<AchievementId> pending() const noexcept
    {
        return pending_ ? std::optional{AchievementId::first_delivery}
                        : std::nullopt;
    }
    void dismiss() noexcept
    {
        pending_ = false;
    }

  private:
    AchievementSink &sink_;
    bool announced_ = false;
    bool pending_ = false;
};
} // namespace closed_circuit
