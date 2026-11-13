#include "application/achievement_notice.hpp"
#include <stdexcept>
using namespace closed_circuit;

class Sink final : public AchievementSink
{
  public:
    AchievementSinkResult result = AchievementSinkResult::unavailable;
    bool throwing = false;
    unsigned calls = 0;
    AchievementId last{};
    AchievementSinkResult unlock(AchievementId id) override
    {
        ++calls;
        last = id;
        if (throwing)
            throw std::runtime_error("test sink");
        return result;
    }
};
int main()
{
    Sink sink;
    AchievementNoticeSink notice{sink};
    if (notice.pending())
        return 1;
    for (const auto result :
         {AchievementSinkResult::already_unlocked,
          AchievementSinkResult::invalid, AchievementSinkResult::unavailable})
    {
        sink.result = result;
        if (notice.unlock(AchievementId::first_delivery) != result ||
            notice.pending())
            return 2;
    }
    sink.throwing = true;
    try
    {
        static_cast<void>(notice.unlock(AchievementId::first_delivery));
        return 3;
    }
    catch (const std::runtime_error &)
    {
    }
    if (notice.pending())
        return 4;
    sink.throwing = false;
    sink.result = AchievementSinkResult::unlocked;
    for (unsigned id = 2; id < 8; ++id)
        if (notice.unlock(static_cast<AchievementId>(id)) != sink.result ||
            notice.pending() || sink.last != static_cast<AchievementId>(id))
            return 5;
    if (notice.unlock(AchievementId::first_delivery) != sink.result ||
        notice.pending() != AchievementId::first_delivery)
        return 6;
    for (int i = 0; i < 5; ++i)
        if (notice.unlock(AchievementId::first_delivery) != sink.result ||
            notice.pending() != AchievementId::first_delivery)
            return 7;
    notice.dismiss();
    notice.dismiss();
    if (notice.pending() ||
        notice.unlock(AchievementId::first_delivery) != sink.result ||
        notice.pending())
        return 8;
    // A new owner still depends on the sink's persisted result.
    sink.result = AchievementSinkResult::already_unlocked;
    AchievementNoticeSink restarted{sink};
    if (restarted.unlock(AchievementId::first_delivery) != sink.result ||
        restarted.pending() || sink.calls != 18)
        return 9;
    return 0;
}
