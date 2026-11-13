#pragma once
#include "application/event_dispatcher.hpp"
#include "application/mvp_achievement_evaluator.hpp"
#include <span>

namespace closed_circuit
{
// One owner per delivery scope; the durable sink outlives it. Prepare may
// allocate, publish cannot, flush preserves the current event on any failure.
class RunAchievements
{
  public:
    RunAchievements(DeliveryScope scope, AchievementSink &sink,
                    RunHistoryOrigin origin)
        : scope_(scope), evaluator_(origin),
          dispatcher_(scope, evaluator_, sink)
    {
    }
    [[nodiscard]] std::vector<DomainEvent>
    prepare(std::span<const DomainEvent> events) const
    {
        std::vector<DomainEvent> next(pending_.begin() + cursor_,
                                      pending_.end());
        next.insert(next.end(), events.begin(), events.end());
        return next;
    }
    void publish(std::vector<DomainEvent> events) noexcept
    {
        pending_ = std::move(events);
        cursor_ = 0;
    }
    [[nodiscard]] bool pending() const noexcept
    {
        return cursor_ < pending_.size();
    }
    [[nodiscard]] bool flush() noexcept
    {
        try
        {
            while (pending())
            {
                if (dispatcher_.dispatch(scope_, pending_[cursor_]) !=
                    DispatchResult::delivered)
                    return false;
                ++cursor_;
            }
            pending_.clear();
            cursor_ = 0;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

  private:
    DeliveryScope scope_;
    MvpAchievementEvaluator evaluator_;
    EventDispatcher dispatcher_;
    std::vector<DomainEvent> pending_;
    std::size_t cursor_ = 0;
};
} // namespace closed_circuit
