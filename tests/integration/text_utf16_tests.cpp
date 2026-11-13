#include "platform/windows/text_utf16.hpp"
#include <array>
using namespace closed_circuit;

int main()
{
    // Literal UTF-8/UTF-16 oracles, independent of a second conversion API.
    const std::string value =
        "[AI PLACEHOLDER] Za\xc5\xbc\xc3\xb3\xc5\x82\xc4\x87 \xf0\x9f\x94\x8c";
    const std::wstring expected =
        L"[AI PLACEHOLDER] Za\u017c\u00f3\u0142\u0107 \xd83d\xdd0c";
    const TextRecord record{"title", value, TextStatus::ai_placeholder,
                            std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    const auto original = *catalog.find("title");
    auto first = windows::read_text_utf16(catalog, "title");
    if (!first.text || first.error != ERROR_SUCCESS || *first.text != expected)
        return 1;
    first.text->assign(L"changed private copy");
    for (int i = 0; i < 3; ++i)
    {
        const auto next = windows::read_text_utf16(catalog, "title");
        if (!next.text || *next.text != expected ||
            *catalog.find("title") != original)
            return 2;
    }
    const auto missing = windows::read_text_utf16(catalog, "absent");
    if (missing.text || missing.error != ERROR_NOT_FOUND)
        return 3;
    for (const auto &bytes :
         std::array<std::string, 5>{"\xc0\xaf", "\xe2\x82", "\xed\xa0\x80",
                                    "\xf4\x90\x80\x80", "\xff"})
    {
        const TextRecord invalid{"bad", "[AI PLACEHOLDER] " + bytes,
                                 TextStatus::ai_placeholder, std::nullopt};
        const auto bad = *TextCatalog::from_records(std::span{&invalid, 1}, {},
                                                    TextAudience::developer);
        const auto result = windows::read_text_utf16(bad, "bad");
        if (result.text || result.error != ERROR_NO_UNICODE_TRANSLATION)
            return 4;
    }
    const TextRecord marker{"marker", "[AI PLACEHOLDER]",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto minimal = *TextCatalog::from_records(std::span{&marker, 1}, {},
                                                    TextAudience::developer);
    if (windows::read_text_utf16(minimal, "marker").text != L"[AI PLACEHOLDER]")
        return 5;
    const TextRecord maximal{"long",
                             "[AI PLACEHOLDER]" + std::string(4080, 'a'),
                             TextStatus::ai_placeholder, std::nullopt};
    const auto long_catalog = *TextCatalog::from_records(
        std::span{&maximal, 1}, {}, TextAudience::developer);
    const auto long_text = windows::read_text_utf16(long_catalog, "long");
    if (!long_text.text || long_text.text->size() != 4096 ||
        !long_text.text->starts_with(L"[AI PLACEHOLDER]"))
        return 6;
    return 0;
}
