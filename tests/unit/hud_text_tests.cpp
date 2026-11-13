#include "presentation/hud_text.hpp"
#include <limits>
using namespace closed_circuit;

int main()
{
    const std::array records{
        TextRecord{"hud-integrity", "[AI PLACEHOLDER] HP:",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"hud-turns", "[AI PLACEHOLDER] Turns:",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"hud-deliveries", "[AI PLACEHOLDER] Deliveries:",
                   TextStatus::ai_placeholder, std::nullopt}};
    const auto catalog =
        *TextCatalog::from_records(records, {}, TextAudience::developer);
    const auto lines = presentation::hud_lines({0, TurnIndex{48}, 2});
    const std::array expected{"[AI PLACEHOLDER] HP: 0",
                              "[AI PLACEHOLDER] Turns: 48",
                              "[AI PLACEHOLDER] Deliveries: 2"};
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        auto value = format_text_line(catalog, lines[i]);
        if (!value || *value != expected[i])
            return 1;
        value->clear();
        if (catalog.records()[i] != records[i])
            return 2;
    }
    if (format_text_line(catalog, {"absent", 1}) ||
        format_text_line(catalog, {"hud-turns", std::nullopt}) !=
            records[1].value ||
        format_text_line(
            catalog,
            {"hud-turns", std::numeric_limits<std::uint64_t>::max()}) !=
            "[AI PLACEHOLDER] Turns: 18446744073709551615" ||
        TextCatalog::from_records(records, {}, TextAudience::public_release))
        return 3;
    return 0;
}
