#pragma once

#include "application/achievement_sink.hpp"
#include "application/event_delivery.hpp"
#include <algorithm>
#include <vector>

namespace closed_circuit
{
class EventDispatcher
{
  public:
    EventDispatcher(DeliveryScope scope, AchievementEvaluator &evaluator,
                    AchievementSink &sink)
        : scope_(scope), evaluator_(evaluator), sink_(sink)
    {
    }
    EventDispatcher(const EventDispatcher &) = delete;
    EventDispatcher &operator=(const EventDispatcher &) = delete;
    EventDispatcher(EventDispatcher &&) = delete;
    EventDispatcher &operator=(EventDispatcher &&) = delete;

    [[nodiscard]] DispatchResult dispatch(const DeliveryScope &scope,
                                          const DomainEvent &event)
    {
        if (!is_delivery_scope_valid(scope_) || scope != scope_)
            return DispatchResult::wrong_scope;
        if (!valid_event(event))
            return DispatchResult::invalid_event;
        const auto found = std::ranges::find(entries_, event.key,
                                             [](const Entry &entry)
                                             { return entry.event.key; });
        if (found != entries_.end())
            return found->event == event ? deliver(*found)
                                         : DispatchResult::conflict;
        if (!entries_.empty())
        {
            if (!entries_.back().valid)
                return DispatchResult::invalid_candidates;
            if (entries_.back().pending != 0)
                return DispatchResult::retry;
        }
        if (!follows(event))
            return DispatchResult::out_of_order;
        // Allocation and the owning copy precede the nonthrowing evaluator.
        entries_.push_back({event});
        auto &entry = entries_.back();
        entry.pending = evaluator_.evaluate(entry.event);
        entry.valid = is_candidate_mask_valid(entry.pending);
        return deliver(entry);
    }

  private:
    struct Entry
    {
        DomainEvent event;
        std::uint16_t pending = 0;
        bool valid = false;
    };
    [[nodiscard]] bool valid_event(const DomainEvent &event) const
    {
        const auto &key = event.key;
        const auto turn = static_cast<std::uint32_t>(key.turn);
        const bool action = event.kind() == DomainEventKind::action_committed;
        if (key.seed != scope_.seed || turn == 0 || turn > 48 ||
            key.ordinal > 3 || action != (key.ordinal == 0))
            return false;
        if (const auto *end = std::get_if<RunEnded>(&event.payload))
            return (end->reason == RunEndReason::won &&
                    key.sector.value() == 3) ||
                   end->reason == RunEndReason::death ||
                   end->reason == RunEndReason::overload;
        return true;
    }
    [[nodiscard]] bool follows(const DomainEvent &event) const
    {
        const auto &key = event.key;
        if (entries_.empty())
            return key.sector == scope_.first_sector &&
                   key.turn == TurnIndex{1} && key.ordinal == 0;
        const auto &previous = entries_.back().event;
        const auto &last = previous.key;
        const auto *end = std::get_if<RunEnded>(&event.payload);
        const bool completed =
            previous.kind() == DomainEventKind::sector_completed;
        if (key.sector == last.sector && key.turn == last.turn)
        {
            if (key.ordinal != last.ordinal + 1 ||
                event.kind() <= previous.kind())
                return false;
            if (completed || (end && end->reason == RunEndReason::won))
                return completed && end && end->reason == RunEndReason::won;
            return true;
        }
        if (key.ordinal != 0 || previous.kind() == DomainEventKind::run_ended)
            return false;
        if (completed)
            return key.sector.value() == last.sector.value() + 1 &&
                   key.turn == TurnIndex{1};
        return key.sector == last.sector &&
               static_cast<std::uint32_t>(key.turn) ==
                   static_cast<std::uint32_t>(last.turn) + 1;
    }
    [[nodiscard]] DispatchResult deliver(Entry &entry)
    {
        if (!entry.valid)
            return DispatchResult::invalid_candidates;
        for (unsigned id = 1; id <= 5; ++id)
        {
            const auto bit = static_cast<std::uint16_t>(1u << (id - 1));
            if ((entry.pending & bit) == 0)
                continue;
            const auto result = sink_.unlock(static_cast<AchievementId>(id));
            if (result != AchievementSinkResult::unlocked &&
                result != AchievementSinkResult::already_unlocked)
                return DispatchResult::retry;
            entry.pending &= static_cast<std::uint16_t>(~bit);
        }
        return DispatchResult::delivered;
    }

    DeliveryScope scope_;
    AchievementEvaluator &evaluator_;
    AchievementSink &sink_;
    std::vector<Entry> entries_;
};
} // namespace closed_circuit
