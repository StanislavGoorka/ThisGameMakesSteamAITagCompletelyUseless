#include "../support/checkpoint_files.hpp"
#include "application/event_dispatcher.hpp"
#include "application/mvp_achievement_evaluator.hpp"
#include "application/run_turn_events.hpp"
#include "platform/windows/local_achievement_sink.hpp"
using namespace closed_circuit;
NodeId node(int n)
{
    return *NodeId::from_value(n);
}
RunState fixture()
{
    RunState run{.seed = RunSeed{1},
                 .sector = GameState{.player_position = node(0)}};
    run.sector.ports[0] = *PortMask::from_bits(2);
    run.sector.ports[1] = *PortMask::from_bits(8);
    run.sector.packet = Packet{node(0), node(1)};
    run.sector.receiver_nodes[1] = true;
    return run;
}
bool delivery(const std::filesystem::path &path)
{
    const DeliveryScope scope{
        RunSeed{1}, {1}, {1}, *SectorNumber::from_value(1)};
    const LocalProfile initial{0, 3, 42, true};
    if (!windows::save_profile_file(path, initial))
        return false;
    const auto original = test::read_bytes(path);
    MvpAchievementEvaluator evaluator;
    windows::LocalAchievementSink sink(path);
    EventDispatcher dispatcher(scope, evaluator, sink);
    auto run = fixture();
    // A delivery survives death later in the same accepted turn.
    run.sector.player_integrity = 1;
    run.sector.pulses.push_back({*EntityId::from_value(1), node(0)});
    const auto result = resolve_run_turn_events(run, WaitCommand{});
    if (!result.turn.accepted || result.turn.phase != RunPhase::lost ||
        result.events.size() != 3 ||
        dispatcher.dispatch(scope, result.events[0]) !=
            DispatchResult::delivered)
        return false;
    {
        windows::FileHandle lock(CreateFileW(path.c_str(), GENERIC_READ, 0,
                                             nullptr, OPEN_EXISTING,
                                             FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!lock || dispatcher.dispatch(scope, result.events[1]) !=
                         DispatchResult::retry)
            return false;
    }
    if (test::read_bytes(path) != original ||
        dispatcher.dispatch(scope, result.events[1]) !=
            DispatchResult::delivered ||
        dispatcher.dispatch(scope, result.events[2]) !=
            DispatchResult::delivered ||
        windows::read_profile_file(path).profile !=
            LocalProfile{1, 3, 42, true})
        return false;
    const auto main = test::read_bytes(path);
    const auto backup = test::read_bytes(path.native() + L".bak");
    // Fresh evaluator/branch and a second sink read the durable set again.
    MvpAchievementEvaluator restored;
    windows::LocalAchievementSink second(path);
    const DeliveryScope branch{
        scope.seed, scope.attempt, {2}, scope.first_sector};
    EventDispatcher replay(branch, restored, second);
    for (const auto &e : result.events)
        if (replay.dispatch(branch, e) != DispatchResult::delivered)
            return false;
    return test::read_bytes(path) == main &&
           test::read_bytes(path.native() + L".bak") == backup &&
           backup == original;
}
int main()
{
    test::CheckpointDirectory directory;
    return delivery(directory.path / L"predykaty.dat") ? 0 : 1;
}
