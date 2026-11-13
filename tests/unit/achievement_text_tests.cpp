#include "presentation/achievement_text.hpp"
using namespace closed_circuit;
int main()
{
    const std::array records{
        TextRecord{"achievement-first-delivery",
                   "[AI PLACEHOLDER] First delivery unlocked.",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"achievement-unavailable",
                   "[AI PLACEHOLDER] Achievement storage unavailable.",
                   TextStatus::ai_placeholder, std::nullopt}};
    const auto catalog =
        *TextCatalog::from_records(records, {}, TextAudience::developer);
    const auto empty =
        *TextCatalog::from_records({}, {}, TextAudience::developer);
    if (presentation::achievement_line({}) ||
        presentation::achievement_storage_line(false))
        return 1;
    for (unsigned id = 0; id < 256; ++id)
    {
        const auto line =
            presentation::achievement_line(static_cast<AchievementId>(id));
        if (static_cast<bool>(line) != (id == 1))
            return 2;
        if (line)
        {
            const auto value = format_text_line(catalog, *line);
            if (!value || *value != records[0].value ||
                !value->starts_with("[AI PLACEHOLDER]") || line->number ||
                format_text_line(empty, *line))
                return 3;
        }
    }
    const auto error = presentation::achievement_storage_line(true);
    if (!error || format_text_line(catalog, *error) != records[1].value ||
        error->number || format_text_line(empty, *error) ||
        TextCatalog::from_records(records, {}, TextAudience::public_release))
        return 4;
    return 0;
}
