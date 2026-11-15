#include "developer_text.hpp"
#include "presentation/session_text.hpp"

#include <bit>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace closed_circuit;

namespace
{
bool check(bool condition, const char *message)
{
    if (!condition)
        std::cerr << message << '\n';
    return condition;
}

DifficultyModifiers mask(int bits)
{
    // An unknown mask falls back to none, so comparisons fail plainly.
    return DifficultyModifiers::from_value(bits).value_or(
        DifficultyModifiers{});
}
} // namespace

int main()
{
    const auto developer = TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    if (!check(developer.has_value(), "Developer registry must load"))
        return 1;
    // Every modifier bit has its own marked record in the real registry;
    // other values have no text and no fallback.
    std::set<std::string_view> ids;
    std::set<std::string> values;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const auto modifier = static_cast<DifficultyModifier>(raw);
        const auto text = presentation::difficulty_modifier_text_id(modifier);
        if (!check(text.has_value() == (raw == 1 || raw == 2 || raw == 4),
                   "Only the three modifier bits have a text"))
            return 2;
        if (!text)
            continue;
        const auto value = format_text_line(*developer, {*text, std::nullopt});
        if (!check(value && value->starts_with("[AI PLACEHOLDER] ") &&
                       value->size() > 17 && ids.insert(*text).second &&
                       values.insert(*value).second,
                   "Each modifier needs its own marked record"))
            return 3;
        // Dropping the record leaves the line unformatted.
        std::vector<TextRecord> partial;
        for (const auto &record : windows::developer_text_records)
            if (record.id != *text)
                partial.push_back(record);
        const auto reduced =
            TextCatalog::from_records(partial, {}, TextAudience::developer);
        if (!check(reduced &&
                       !format_text_line(*reduced, {*text, std::nullopt}),
                   "A dropped record must not format"))
            return 4;
    }
    if (!check(ids.size() == difficulty_modifier_catalog.size(),
               "Every catalog bit needs a text"))
        return 5;
    // Start and result screens list the configuration, then active bits in
    // catalog order; the run screen and an unknown configuration show none.
    for (int bits = 0; bits < 8; ++bits)
        for (const auto screen :
             {SessionScreen::start, SessionScreen::run, SessionScreen::result})
        {
            const auto lines = presentation::session_lines(
                screen, StartConfigId::preinstalled_range, mask(bits));
            std::vector<std::string_view> expected;
            if (screen != SessionScreen::run)
            {
                expected.push_back("start-config-range");
                if ((bits & 1) != 0)
                    expected.push_back("difficulty-short-turns");
                if ((bits & 2) != 0)
                    expected.push_back("difficulty-lean-delivery");
                if ((bits & 4) != 0)
                    expected.push_back("difficulty-heavy-debt");
            }
            if (!check(lines.size() == expected.size(),
                       "Unexpected number of session lines"))
                return 6;
            for (std::size_t index = 0; index < lines.size(); ++index)
                if (!check(lines[index].id == expected[index] &&
                               !lines[index].number,
                           "Unexpected session line"))
                    return 7;
            if (!check(presentation::session_lines(
                           screen, static_cast<StartConfigId>(4), mask(bits))
                           .empty(),
                       "An unknown configuration shows no line"))
                return 8;
        }
    // The public audience rejects the registry while any record is marked.
    if (!check(!TextCatalog::from_records(windows::developer_text_records, {},
                                          TextAudience::public_release),
               "Public audience must refuse placeholders"))
        return 9;
    return 0;
}
