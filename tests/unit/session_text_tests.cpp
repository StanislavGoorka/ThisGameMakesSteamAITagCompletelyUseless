#include "../support/run_checks.hpp"
#include "presentation/session_text.hpp"
using namespace closed_circuit;

bool selects(const RunState &run, SessionScreen screen,
             std::string_view expected)
{
    const auto before = run;
    const auto line = presentation::session_message(screen, run);
    return test::same_run(run, before) &&
           (expected.empty() ? !line
                             : line && line->id == expected && !line->number);
}
int main()
{
    auto run = start_run(RunSeed{1});
    if (!selects(run, SessionScreen::start, "session-start") ||
        !selects(run, SessionScreen::run, {}) ||
        !selects(run, SessionScreen::result, {}) ||
        !selects(run, static_cast<SessionScreen>(99), {}))
        return 1;
    for (int sector = 1; sector <= 3; ++sector)
    {
        run = start_run(RunSeed{1});
        run.sector_number = *SectorNumber::from_value(sector);
        run.sector.powered_nodes = run.sector.receiver_nodes;
        run.sector.player_position = *run.sector.sector_exit;
        for (auto turn : {47, 48, 49})
        {
            run.sector.turns_taken =
                TurnIndex{static_cast<std::uint32_t>(turn)};
            if (!selects(run, SessionScreen::result,
                         turn > 48     ? "session-overload"
                         : sector == 3 ? "session-won"
                                       : ""))
                return 2;
            run.sector.player_integrity = 0;
            if (!selects(run, SessionScreen::result, "session-death"))
                return 3;
            run.sector.player_integrity = 3;
        }
    }
    const auto empty =
        *TextCatalog::from_records({}, {}, TextAudience::developer);
    const auto line = presentation::session_message(SessionScreen::start, run);
    return line && !format_text_line(empty, *line) ? 0 : 4;
}
