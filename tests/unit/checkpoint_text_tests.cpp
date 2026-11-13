#include "developer_text.hpp"
#include "presentation/checkpoint_text.hpp"
#include <algorithm>
#include <cctype>
using namespace closed_circuit;
namespace
{
// No digits or backslash (0x5c): codes and paths never reach the line.
bool marked(const std::optional<std::string> &value)
{
    return value && value->starts_with("[AI PLACEHOLDER] ") &&
           std::ranges::none_of(*value, [](unsigned char c)
                                { return std::isdigit(c) || c == 0x5c; });
}
} // namespace
int main()
{
    const std::array records{
        TextRecord{"checkpoint-corrupt", "[AI PLACEHOLDER] Damaged.",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"checkpoint-unavailable", "[AI PLACEHOLDER] Unreadable.",
                   TextStatus::ai_placeholder, std::nullopt},
        TextRecord{"checkpoint-unsaved", "[AI PLACEHOLDER] Unsaved.",
                   TextStatus::ai_placeholder, std::nullopt}};
    const auto local =
        *TextCatalog::from_records(records, {}, TextAudience::developer);
    const auto developer = *TextCatalog::from_records(
        windows::developer_text_records, {}, TextAudience::developer);
    const auto empty =
        *TextCatalog::from_records({}, {}, TextAudience::developer);
    if (presentation::resume_line(ResumeStatus::none) ||
        presentation::resume_line(ResumeStatus::resumed) ||
        presentation::checkpoint_storage_line(false))
        return 1;
    const std::pair<std::optional<TextLine>, std::string_view> lines[]{
        {presentation::resume_line(ResumeStatus::corrupt), records[0].value},
        {presentation::resume_line(ResumeStatus::unavailable),
         records[1].value},
        {presentation::checkpoint_storage_line(true), records[2].value}};
    for (const auto &[line, value] : lines)
    {
        if (!line || line->number || format_text_line(local, *line) != value ||
            format_text_line(empty, *line) ||
            !marked(format_text_line(developer, *line)))
            return 2;
    }
    for (int raw = 4; raw < 256; ++raw)
        if (presentation::resume_line(static_cast<ResumeStatus>(raw)))
            return 3;
    if (TextCatalog::from_records(records, {}, TextAudience::public_release))
        return 4;
    return 0;
}
