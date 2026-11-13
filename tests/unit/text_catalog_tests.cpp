#include "application/text_catalog.hpp"
#include <iostream>
using namespace closed_circuit;

int main()
{
    const auto source = *AssetReference::from_fields(
        "source", "assets/approved/source.txt", std::string(64, '0'), 0);
    const AssetManifest assets{source};
    // Non-player fixture only: this synthetic record is never product content.
    const TextRecord approved{"approved-fixture", "synthetic test value",
                              TextStatus::approved, "source"};
    const TextRecord placeholder{"test", "[AI PLACEHOLDER] Test",
                                 TextStatus::ai_placeholder, std::nullopt};
    std::vector records{placeholder, approved};
    const auto catalog = TextCatalog::from_records(records, assets);
    if (!catalog || catalog->records().size() != 2 || !catalog->find("test") ||
        *catalog->find("test") != placeholder.value ||
        !catalog->find(approved.id) ||
        *catalog->find(approved.id) != approved.value ||
        catalog->find("unknown") || catalog->find("") ||
        !TextCatalog::from_records({}, {}))
        return 1;
    records[0].value.clear();
    auto copy = *catalog;
    copy = *TextCatalog::from_records({}, {});
    if (*catalog->find("test") != placeholder.value || copy.find("test"))
        return 1;
    auto rejected = [&](TextRecord record)
    {
        if (!TextCatalog::from_records(std::span{&record, 1}, assets))
            return true;
        std::cerr << "Unexpected text acceptance: " << record.id << '\n';
        return false;
    };
    for (const auto value : {"", "Test", " [AI PLACEHOLDER] Test",
                             "[ai placeholder] Test", "[AI PLACEHOLDER"})
    {
        auto bad = placeholder;
        bad.value = value;
        if (!rejected(bad))
            return 1;
    }
    for (const auto id : {"", "Bad", "bad id", "../id"})
    {
        auto bad = placeholder;
        bad.id = id;
        if (!rejected(bad))
            return 1;
    }
    auto bad = placeholder;
    bad.source_asset_id = "source";
    if (!rejected(bad))
        return 1;
    bad = approved;
    bad.source_asset_id.reset();
    if (!rejected(bad))
        return 1;
    bad.source_asset_id = "unknown";
    if (!rejected(bad))
        return 1;
    bad = approved;
    bad.value = "test [AI PLACEHOLDER]";
    if (!rejected(bad))
        return 1;
    bad = placeholder;
    bad.status = static_cast<TextStatus>(255);
    if (!rejected(bad))
        return 1;
    bad = placeholder;
    bad.value.push_back('\0');
    if (!rejected(bad))
        return 1;
    bad.value = "[AI PLACEHOLDER]" + std::string(4081, 'x');
    if (bad.value.size() != 4097 || !rejected(bad))
        return 1;
    bad.value.pop_back();
    if (!TextCatalog::from_records(std::span{&bad, 1}, {}))
        return 1;
    std::vector duplicates{placeholder, placeholder};
    if (TextCatalog::from_records(duplicates, assets))
        return 1;
    std::vector<TextRecord> many;
    for (unsigned i = 0; i < 256; ++i)
    {
        many.push_back(placeholder);
        many.back().id = "id" + std::to_string(i);
    }
    if (!TextCatalog::from_records(many, {}))
        return 1;
    many.push_back(approved);
    if (TextCatalog::from_records(many, assets))
        return 1;
    return 0;
}
