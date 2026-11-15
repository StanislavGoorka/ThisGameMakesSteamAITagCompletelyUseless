#include "developer_text.hpp"
#include "presentation/reward_text.hpp"

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
    // Every implemented module has its own marked record in the real
    // registry; values outside the catalog have no text and no fallback.
    std::set<std::string_view> ids;
    std::set<std::string> values;
    for (unsigned raw = 0; raw < 256; ++raw)
    {
        const auto id = static_cast<ModuleId>(raw);
        const auto text = presentation::module_text_id(id);
        if (text.has_value() != is_known_module(id) ||
            text.has_value() != (raw >= 1 && raw <= 12))
            return 2;
        if (!text)
            continue;
        const auto value = format_text_line(*developer, {*text, std::nullopt});
        if (!value || !value->starts_with("[AI PLACEHOLDER] ") ||
            value->size() <= 17 || !ids.insert(*text).second ||
            !values.insert(*value).second)
            return 3;
    }
    if (ids.size() != module_catalog.size())
        return 4;
    // Offers of any two implemented modules are numbered rows of those
    // records; dropping one record leaves its row unformatted.
    for (const auto first : module_catalog)
        for (const auto second : module_catalog)
        {
            if (second <= first)
                continue;
            const ModuleOffer offer{first, second};
            const auto lines = presentation::reward_lines(offer, second);
            if (!lines || lines->size() != 4 ||
                (*lines)[1].id != *presentation::module_text_id(first) ||
                (*lines)[2].id != *presentation::module_text_id(second) ||
                (*lines)[1].number != 1 || (*lines)[2].number != 2 ||
                lines->back().id != "reward-selected" ||
                lines->back().number != 2)
                return 5;
            std::vector<TextRecord> partial;
            for (const auto &record : windows::developer_text_records)
                if (record.id != *presentation::module_text_id(second))
                    partial.push_back(record);
            const auto reduced =
                TextCatalog::from_records(partial, {}, TextAudience::developer);
            if (!reduced || !format_text_line(*reduced, (*lines)[1]) ||
                format_text_line(*reduced, (*lines)[2]))
                return 6;
        }
    // Unknown or duplicated offers stay refused; the public audience rejects
    // the whole registry while any record is a placeholder.
    const auto known = ModuleId::packet_priority;
    if (presentation::reward_lines({static_cast<ModuleId>(13), {}}, {}) ||
        presentation::reward_lines({known, static_cast<ModuleId>(0)}, {}) ||
        presentation::reward_lines({known, known}, {}) ||
        TextCatalog::from_records(windows::developer_text_records, {},
                                  TextAudience::public_release))
        return 7;
    return 0;
}
