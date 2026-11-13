#include "presentation/reward_text.hpp"
using namespace closed_circuit;

int main()
{
    const std::array records{
        TextRecord{"reward-range", "[AI PLACEHOLDER] Rotation range:",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"reward-terminal", "[AI PLACEHOLDER] Terminal rotation:",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"reward-instruction", "[AI PLACEHOLDER] Left/Right, Enter.",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"reward-selected", "[AI PLACEHOLDER] Selected:",
                   TextStatus::ai_placeholder, std::nullopt}};
    const auto catalog =
        *TextCatalog::from_records(records, {}, TextAudience::developer);
    const auto empty =
        *TextCatalog::from_records({}, {}, TextAudience::developer);
    for (unsigned mask = 0; mask < 4; ++mask)
    {
        ModuleLoadout owned;
        if (mask & 1)
            static_cast<void>(owned.add(ModuleId::rotation_range));
        if (mask & 2)
            static_cast<void>(owned.add(ModuleId::terminal_rotation));
        const auto offer = module_offer(owned);
        for (auto candidate : {std::optional<ModuleId>{}, offer[0], offer[1]})
        {
            const auto lines = presentation::reward_lines(offer, candidate);
            if (!lines || (mask == 3 && !lines->empty()))
                return 1;
            std::uint64_t row = 0;
            for (const auto line : *lines)
            {
                const auto value = format_text_line(catalog, line);
                if (!value || !value->starts_with("[AI PLACEHOLDER]") ||
                    format_text_line(empty, line))
                    return 2;
                if (line.id == "reward-range" || line.id == "reward-terminal")
                {
                    if (line.number != ++row)
                        return 3;
                }
            }
            if (candidate &&
                (lines->back().id != "reward-selected" ||
                 lines->back().number != (candidate == offer[0] ? 1 : 2)))
                return 4;
        }
    }
    const auto a = ModuleId::rotation_range, b = ModuleId::terminal_rotation;
    if (presentation::reward_lines({a, a}, {}) ||
        presentation::reward_lines({b, a}, {}) ||
        presentation::reward_lines({std::nullopt, b}, {}) ||
        presentation::reward_lines({static_cast<ModuleId>(99), {}}, {}) ||
        presentation::reward_lines({a, {}}, b) ||
        presentation::reward_lines({}, a) ||
        TextCatalog::from_records(records, {}, TextAudience::public_release))
        return 5;
    return 0;
}
