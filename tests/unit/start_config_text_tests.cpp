#include "developer_text.hpp"
#include "presentation/session_text.hpp"

#include <set>
#include <string>
#include <vector>

using namespace closed_circuit;
int main()
{
    const auto developer = TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    if (!developer)
        return 1;
    // Every implemented configuration has its own marked record in the real
    // registry; values outside the catalog have no text and no fallback.
    std::set<std::string_view> ids;
    std::set<std::string> values;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const auto config = static_cast<StartConfigId>(raw);
        const auto text = presentation::start_config_text_id(config);
        if (text.has_value() != is_known_start_config(config) ||
            text.has_value() != (raw >= 1 && raw <= 3))
            return 2;
        for (const auto screen :
             {SessionScreen::start, SessionScreen::run, SessionScreen::result})
        {
            const auto lines = presentation::session_lines(screen, config, {});
            const bool shown = text && screen != SessionScreen::run;
            if (lines.size() != (shown ? 1u : 0u) ||
                (shown && (lines[0].id != *text || lines[0].number)))
                return 3;
        }
        if (!text)
            continue;
        const auto value = format_text_line(*developer, {*text, std::nullopt});
        if (!value || !value->starts_with("[AI PLACEHOLDER] ") ||
            value->size() <= 17 || !ids.insert(*text).second ||
            !values.insert(*value).second)
            return 4;
        // Dropping the record leaves the line unformatted.
        std::vector<TextRecord> partial;
        for (const auto &record : windows::developer_text_records)
            if (record.id != *text)
                partial.push_back(record);
        const auto reduced =
            TextCatalog::from_records(partial, {}, TextAudience::developer);
        if (!reduced || format_text_line(*reduced, {*text, std::nullopt}))
            return 5;
    }
    if (ids.size() != start_config_catalog.size())
        return 6;
    // The public audience rejects the registry while any record is marked.
    if (TextCatalog::from_records(windows::developer_text_records, {},
                                  TextAudience::public_release))
        return 7;
    return 0;
}
