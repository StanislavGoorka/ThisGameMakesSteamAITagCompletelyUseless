#include "../support/achievement_fakes.hpp"
#include "../support/checkpoint_files.hpp"
#include "../support/run_checks.hpp"
#include "application/event_dispatcher.hpp"
#include "application/mvp_achievement_evaluator.hpp"
#include "application/run_turn_events.hpp"
#include "core/run_reward.hpp"
#include "core/run_start.hpp"
#include "core/run_transition.hpp"
#include "corpus/baseline_witness.hpp"
#include "platform/windows/checkpoint_save.hpp"
#include "platform/windows/local_achievement_sink.hpp"
#include <string_view>
using namespace closed_circuit;

class ObservedSink final : public AchievementSink
{
  public:
    explicit ObservedSink(const std::filesystem::path &path) : local(path)
    {
    }
    [[nodiscard]] AchievementSinkResult unlock(AchievementId id) override
    {
        ++calls;
        const auto result = local.unlock(id);
        if (result == AchievementSinkResult::unlocked)
            ++new_unlocks;
        return result;
    }
    windows::LocalAchievementSink local;
    unsigned calls = 0, new_unlocks = 0;
};

using FileImage = std::array<std::optional<std::vector<std::uint8_t>>, 2>;
FileImage image(const std::filesystem::path &path)
{
    FileImage result;
    const std::array<std::filesystem::path, 2> names{path,
                                                     path.native() + L".bak"};
    for (std::size_t i = 0; i < 2; ++i)
        if (std::filesystem::exists(names[i]))
            result[i] = test::read_bytes(names[i]);
    return result;
}
struct Step
{
    Command command;
    RunState state;
    std::vector<DomainEvent> events;
};
DeliveryScope scope(const RunState &run, unsigned attempt = 1,
                    unsigned branch = 1)
{
    return {run.seed, {attempt}, {branch}, run.sector_number};
}

template <typename Evaluator>
bool publish(EventDispatcher &dispatcher, const DeliveryScope &context,
             const std::vector<DomainEvent> &events, Evaluator &evaluator,
             ObservedSink &sink)
{
    for (const auto &event : events)
    {
        const auto evaluations = evaluator.calls;
        if (dispatcher.dispatch(context, event) != DispatchResult::delivered ||
            evaluator.calls != evaluations + 1)
            return false;
        const auto calls = sink.calls;
        for (int repeat = 0; repeat < 3; ++repeat)
            if (dispatcher.dispatch(context, event) !=
                    DispatchResult::delivered ||
                evaluator.calls != evaluations + 1 || sink.calls != calls)
                return false;
    }
    return true;
}

template <typename Evaluator>
bool sector(RunState &run, EventDispatcher &dispatcher,
            const DeliveryScope &context, Evaluator &evaluator,
            ObservedSink &sink, const std::filesystem::path &profile,
            std::vector<Step> &trace)
{
    SplitMix64 rng(run.seed);
    RunSeed sector_seed{0};
    for (unsigned i = 0; i < run.sector_number.value(); ++i)
        sector_seed = RunSeed{rng.next_u64()};
    // Reuse the certified baseline commands, not its default resource state.
    const auto commands = developer::build_baseline_witness(
        sector_seed, developer::sector_fixture(sector_seed));
    if (!commands)
        return false;
    for (const auto &command : *commands)
    {
        const auto before = run;
        const auto files = image(profile);
        const auto calls = sink.calls, evaluations = evaluator.calls;
        const auto core = resolve_run_turn(run, command);
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            const auto preview = preview_run_turn(run, command);
            if (!preview.accepted ||
                !test::same_run(preview.state, core.state) ||
                preview.phase != core.phase || !test::same_run(run, before) ||
                sink.calls != calls || evaluator.calls != evaluations ||
                image(profile) != files)
                return false;
        }
        const auto actual = resolve_run_turn_events(run, command);
        if (!actual.turn.accepted ||
            !test::same_run(actual.turn.state, core.state) ||
            actual.events.empty())
            return false;
        run = actual.turn.state;
        if (!publish(dispatcher, context, actual.events, evaluator, sink))
            return false;
        trace.push_back({command, run, actual.events});
    }
    return sector_outcome(run.sector) == SectorOutcome::completed;
}

bool worker(const std::filesystem::path &directory)
{
    const auto read = windows::read_checkpoint_file(directory / L"run.dat");
    const auto restored =
        read.checkpoint ? restore_checkpoint(*read.checkpoint) : std::nullopt;
    if (!restored || restored->sector_number.value() != 2)
        return false;
    auto run = *restored;
    const auto profile = directory / L"profil \u0141\u00f3d\u017a.dat";
    const auto files = image(profile);
    test::CountingEvaluator evaluator;
    evaluator.candidates = 31; // Fixed transport probe, never an MVP predicate.
    ObservedSink sink(profile);
    const auto context = scope(run);
    EventDispatcher dispatcher(context, evaluator, sink);
    std::vector<Step> trace;
    return sector(run, dispatcher, context, evaluator, sink, profile, trace) &&
           !trace.empty() && evaluator.calls > 0 && sink.new_unlocks == 0 &&
           image(profile) == files &&
           windows::read_profile_file(profile).profile ==
               LocalProfile{31, 3, 29, true};
}

bool launch(const std::filesystem::path &directory)
{
    std::array<wchar_t, 32768> executable{};
    const auto length = GetModuleFileNameW(
        nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size())
        return false;
    auto command = L"\"" + std::wstring(executable.data()) + L"\" --replay \"" +
                   directory.native() + L"\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION information{};
    if (!CreateProcessW(executable.data(), command.data(), nullptr, nullptr,
                        FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startup,
                        &information))
        return false;
    windows::FileHandle process(information.hProcess),
        thread(information.hThread);
    if (WaitForSingleObject(process.get(), 15000) != WAIT_OBJECT_0)
    {
        TerminateProcess(process.get(), 99);
        static_cast<void>(WaitForSingleObject(process.get(), 10000));
        return false;
    }
    DWORD code = 99;
    return GetExitCodeProcess(process.get(), &code) && code == 0;
}

bool complete(RunSeed seed, const std::filesystem::path &directory)
{
    std::filesystem::create_directory(directory);
    const auto profile = directory / L"profil \u0141\u00f3d\u017a.dat";
    if (!windows::save_profile_file(profile, LocalProfile{0, 3, 29, true}))
        return false;
    auto run = start_run(seed);
    const auto context = scope(run);
    test::CountingEvaluator evaluator;
    evaluator.candidates = 31;
    ObservedSink sink(profile);
    EventDispatcher dispatcher(context, evaluator, sink);
    std::optional<RunCheckpoint> second_boundary;
    std::vector<Step> second_trace;
    unsigned total_events = 0;
    for (unsigned number = 1; number <= 3; ++number)
    {
        if (number == 2)
        {
            second_boundary = capture_checkpoint(run);
            if (!second_boundary ||
                !windows::save_checkpoint_file(directory / L"run.dat",
                                               *second_boundary))
                return false;
        }
        std::vector<Step> trace;
        if (!sector(run, dispatcher, context, evaluator, sink, profile, trace))
            return false;
        for (const auto &step : trace)
            total_events += static_cast<unsigned>(step.events.size());
        if (number == 2)
            second_trace = trace;
        if (number < 3)
        {
            const auto offer = module_offer(run.sector.modules);
            if (!offer[0] || !choose_run_module(run, *offer[0]))
                return false;
            const auto next = advance_run(run);
            if (!next.accepted)
                return false;
            run = next.state;
        }
    }
    if (run_phase(run) != RunPhase::won || evaluator.calls != total_events ||
        sink.new_unlocks != 5 ||
        windows::read_profile_file(profile).profile !=
            LocalProfile{31, 3, 29, true})
        return false;
    const auto files = image(profile);
    const auto calls = sink.calls;
    const auto terminal = resolve_run_turn_events(run, WaitCommand{});
    if (terminal.turn.accepted || !terminal.events.empty() ||
        !test::same_run(run, terminal.turn.state) || sink.calls != calls ||
        image(profile) != files)
        return false;
    auto replay = restore_checkpoint(*second_boundary);
    if (!replay)
        return false;
    const auto branch = scope(*replay, 1, 2);
    test::CountingEvaluator branch_evaluator;
    branch_evaluator.candidates = 31;
    EventDispatcher branch_dispatcher(branch, branch_evaluator, sink);
    for (const auto &step : second_trace)
    {
        const auto result = resolve_run_turn_events(*replay, step.command);
        if (result.events != step.events ||
            !test::same_run(result.turn.state, step.state) ||
            branch_dispatcher.dispatch(context, result.events.front()) !=
                DispatchResult::wrong_scope ||
            !publish(branch_dispatcher, branch, result.events, branch_evaluator,
                     sink))
            return false;
        replay = result.turn.state;
    }
    if (branch_evaluator.calls == 0 || sink.new_unlocks != 5 ||
        image(profile) != files)
        return false;
    // A new attempt with the same seed is evaluated again in its own scope.
    auto fresh = start_run(seed);
    const auto attempt = scope(fresh, 2, 1);
    test::CountingEvaluator fresh_evaluator;
    fresh_evaluator.candidates = 31;
    EventDispatcher fresh_dispatcher(attempt, fresh_evaluator, sink);
    std::vector<Step> fresh_trace;
    if (!sector(fresh, fresh_dispatcher, attempt, fresh_evaluator, sink,
                profile, fresh_trace) ||
        fresh_evaluator.calls == 0 || sink.new_unlocks != 5 ||
        image(profile) != files)
        return false;
    for (int repeat = 0; repeat < 2; ++repeat)
        if (!launch(directory) || image(profile) != files)
            return false;
    return true;
}

class ObservedMvpEvaluator final : public AchievementEvaluator
{
  public:
    explicit ObservedMvpEvaluator(
        RunHistoryOrigin origin = RunHistoryOrigin::restored_checkpoint)
        : actual(origin)
    {
    }
    [[nodiscard]] std::uint16_t
    evaluate(const DomainEvent &event) noexcept override
    {
        ++calls;
        const auto result = actual.evaluate(event);
        candidates |= result;
        return result;
    }
    MvpAchievementEvaluator actual;
    unsigned calls = 0;
    std::uint16_t candidates = 0;
};

bool transition(RunState &run)
{
    const auto offer = module_offer(run.sector.modules);
    if (!offer[0] || !choose_run_module(run, *offer[0]))
        return false;
    const auto next = advance_run(run);
    if (!next.accepted)
        return false;
    run = next.state;
    return true;
}

bool complete_mvp(const std::filesystem::path &directory)
{
    if (!std::filesystem::create_directory(directory))
        return false;
    const auto profile = directory / L"mvp.dat";
    const auto checkpoint_path = directory / L"mvp_run.dat";
    const LocalProfile initial{0, 3, 29, true};
    if (!windows::save_profile_file(profile, initial))
        return false;
    auto run = start_run(RunSeed{1});
    const auto context = scope(run);
    ObservedMvpEvaluator evaluator(RunHistoryOrigin::fresh_run);
    ObservedSink sink(profile);
    EventDispatcher dispatcher(context, evaluator, sink);
    std::array<std::vector<Step>, 2> suffix;
    std::optional<RunState> boundary;
    unsigned total_events = 0;
    for (unsigned number = 1; number <= 3; ++number)
    {
        if (number == 2)
        {
            boundary = run;
            const auto checkpoint = capture_checkpoint(run);
            if (!checkpoint ||
                !windows::save_checkpoint_file(checkpoint_path, *checkpoint))
                return false;
        }
        std::vector<Step> trace;
        if (!sector(run, dispatcher, context, evaluator, sink, profile, trace))
            return false;
        for (const auto &step : trace)
            total_events += static_cast<unsigned>(step.events.size());
        if (number >= 2)
            suffix[number - 2] = trace;
        if (number < 3 && !transition(run))
            return false;
    }
    if (run_phase(run) != RunPhase::won || evaluator.candidates != 31 ||
        evaluator.calls != total_events || sink.new_unlocks != 5 ||
        windows::read_profile_file(profile).profile !=
            LocalProfile{31, 3, 29, true} ||
        windows::read_profile_file(profile.native() + L".bak").profile !=
            LocalProfile{15, 3, 29, true})
        return false;
    const auto original_files = image(profile);
    const auto read = windows::read_checkpoint_file(checkpoint_path);
    auto replay =
        read.checkpoint ? restore_checkpoint(*read.checkpoint) : std::nullopt;
    if (!replay || !boundary || !test::same_run(*replay, *boundary))
        return false;
    const auto restored_profile = directory / L"restored.dat";
    if (!windows::save_profile_file(restored_profile, initial))
        return false;
    ObservedMvpEvaluator unknown;
    ObservedSink restored_sink(restored_profile);
    const auto branch = scope(*replay, 1, 2);
    EventDispatcher restored_dispatcher(branch, unknown, restored_sink);
    unsigned suffix_events = 0;
    for (std::size_t i = 0; i < suffix.size(); ++i)
    {
        for (const auto &step : suffix[i])
        {
            const auto files = image(restored_profile);
            const auto evaluations = unknown.calls, calls = restored_sink.calls;
            const auto result = resolve_run_turn_events(*replay, step.command);
            const auto preview = preview_run_turn(*replay, step.command);
            if (!result.turn.accepted || result.events != step.events ||
                !test::same_run(result.turn.state, step.state) ||
                !test::same_run(preview.state, step.state) ||
                image(restored_profile) != files ||
                evaluations != unknown.calls || calls != restored_sink.calls)
                return false;
            *replay = result.turn.state;
            if (!publish(restored_dispatcher, branch, result.events, unknown,
                         restored_sink))
                return false;
            suffix_events += static_cast<unsigned>(result.events.size());
        }
        if (i == 0 && !transition(*replay))
            return false;
    }
    if (!test::same_run(*replay, run) || unknown.calls != suffix_events ||
        unknown.candidates != 27 || restored_sink.new_unlocks != 4 ||
        windows::read_profile_file(restored_profile).profile !=
            LocalProfile{27, 3, 29, true} ||
        windows::read_profile_file(restored_profile.native() + L".bak")
                .profile != LocalProfile{11, 3, 29, true} ||
        image(profile) != original_files)
        return false;
    const auto restored_files = image(restored_profile);
    const auto evaluations = unknown.calls, calls = restored_sink.calls;
    for (const auto &trace : suffix)
        for (const auto &step : trace)
            for (const auto &event : step.events)
                if (restored_dispatcher.dispatch(branch, event) !=
                    DispatchResult::delivered)
                    return false;
    if (unknown.calls != evaluations || restored_sink.calls != calls ||
        image(restored_profile) != restored_files)
        return false;
    ObservedMvpEvaluator another_branch;
    const auto next_branch = scope(*boundary, 1, 3);
    EventDispatcher repeated(next_branch, another_branch, restored_sink);
    for (const auto &trace : suffix)
        for (const auto &step : trace)
            if (!publish(repeated, next_branch, step.events, another_branch,
                         restored_sink))
                return false;
    return another_branch.calls == suffix_events &&
           another_branch.candidates == 27 && restored_sink.new_unlocks == 4 &&
           image(restored_profile) == restored_files &&
           image(profile) == original_files;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc == 3 && std::wstring_view(argv[1]) == L"--replay")
        return worker(argv[2]) ? 0 : 2;
    if (argc != 1)
        return 2;
    test::CheckpointDirectory directory;
    for (const auto raw : {0ull, 1ull, UINT64_MAX})
        if (!complete(RunSeed{raw}, directory.path / std::to_wstring(raw)))
            return 1;
    return complete_mvp(directory.path / L"rzeczywiste") ? 0 : 1;
}
