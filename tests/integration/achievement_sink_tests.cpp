#include "../support/checkpoint_files.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include <type_traits>
using namespace closed_circuit;
static_assert(std::has_virtual_destructor_v<AchievementSink>);

class FakeSink final : public AchievementSink
{
  public:
    [[nodiscard]] AchievementSinkResult unlock(AchievementId id) override
    {
        ++calls;
        last = id;
        return answer;
    }
    unsigned calls = 0;
    AchievementId last = AchievementId::first_delivery;
    AchievementSinkResult answer = AchievementSinkResult::unavailable;
};

bool request(AchievementSink &sink, AchievementId id)
{
    const auto result = sink.unlock(id);
    return result == AchievementSinkResult::unlocked ||
           result == AchievementSinkResult::already_unlocked;
}

int main()
{
    FakeSink fake;
    for (const auto result :
         {AchievementSinkResult::unlocked,
          AchievementSinkResult::already_unlocked,
          AchievementSinkResult::invalid, AchievementSinkResult::unavailable})
    {
        fake.answer = result;
        const auto before = fake.calls;
        if (request(fake, AchievementId::completed_run) !=
                (result == AchievementSinkResult::unlocked ||
                 result == AchievementSinkResult::already_unlocked) ||
            fake.calls != before + 1 ||
            fake.last != AchievementId::completed_run)
            return 1;
    }
    test::CheckpointDirectory directory;
    const auto path = directory.path / L"profile";
    windows::LocalAchievementSink first(path), second(path);
    AchievementSink &port = first;
    if (!request(port, AchievementId::first_delivery) ||
        second.unlock(AchievementId::first_delivery) !=
            AchievementSinkResult::already_unlocked ||
        !request(second, AchievementId::first_sector) ||
        first.unlock(static_cast<AchievementId>(255)) !=
            AchievementSinkResult::invalid ||
        windows::read_profile_file(path).profile != LocalProfile{3})
        return 1;
    {
        windows::FileHandle lock(CreateFileW(path.c_str(), GENERIC_READ, 0,
                                             nullptr, OPEN_EXISTING,
                                             FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!lock || first.unlock(AchievementId::completed_run) !=
                         AchievementSinkResult::unavailable)
            return 1;
    }
    if (!request(second, AchievementId::completed_run) ||
        first.unlock(AchievementId::completed_run) !=
            AchievementSinkResult::already_unlocked ||
        windows::read_profile_file(path).profile != LocalProfile{19})
        return 1;
    const std::array<std::uint8_t, 1> broken{0};
    test::write_bytes(path, broken);
    return first.unlock(AchievementId::completed_run) ==
                       AchievementSinkResult::unavailable &&
                   test::read_bytes(path) == std::vector<std::uint8_t>{0}
               ? 0
               : 1;
}
