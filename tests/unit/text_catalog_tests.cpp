#include "application/text_build_policy.hpp"
#include <iostream>
using namespace closed_circuit;

auto developer_catalog(std::span<const TextRecord> records,
                       std::span<const AssetReference> assets)
{
    return TextCatalog::from_records(records, assets, TextAudience::developer);
}

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
    const auto built_catalog = TextCatalog::from_records(
        std::span{&placeholder, 1}, {}, build_text_audience);
#if CLOSED_CIRCUIT_PUBLIC_BUILD
    if (built_catalog)
        return 1;
#else
    if (!built_catalog || *built_catalog->find("test") != placeholder.value)
        return 1;
#endif
    for (const auto audience :
         {TextAudience::developer, TextAudience::public_release})
    {
        const std::vector<TextRecord> only_approved{approved};
        const std::vector<TextRecord> only_placeholder{placeholder};
        const std::vector<TextRecord> mixed{approved, placeholder};
        const auto before = mixed;
        const auto developer = audience == TextAudience::developer;
        const auto valid =
            TextCatalog::from_records(only_approved, assets, audience);
        if (!TextCatalog::from_records({}, {}, audience) || !valid ||
            valid->find("unknown") ||
            TextCatalog::from_records(only_placeholder, {}, audience)
                    .has_value() != developer ||
            TextCatalog::from_records(mixed, assets, audience).has_value() !=
                developer ||
            mixed != before ||
            TextCatalog::from_records(only_approved, {}, audience))
            return 1;
    }
    if (TextCatalog::from_records({}, {}, static_cast<TextAudience>(255)) ||
        TextCatalog::from_records(std::span{&approved, 1}, assets,
                                  static_cast<TextAudience>(255)))
        return 1;
    std::vector records{placeholder, approved};
    const auto catalog = developer_catalog(records, assets);
    if (!catalog || catalog->records().size() != 2 || !catalog->find("test") ||
        *catalog->find("test") != placeholder.value ||
        !catalog->find(approved.id) ||
        *catalog->find(approved.id) != approved.value ||
        catalog->find("unknown") || catalog->find("") ||
        !developer_catalog({}, {}))
        return 1;
    records[0].value.clear();
    auto copy = *catalog;
    copy = *developer_catalog({}, {});
    if (*catalog->find("test") != placeholder.value || copy.find("test"))
        return 1;
    auto rejected = [&](TextRecord record)
    {
        if (!developer_catalog(std::span{&record, 1}, assets))
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
    if (!developer_catalog(std::span{&bad, 1}, {}))
        return 1;
    std::vector duplicates{placeholder, placeholder};
    if (developer_catalog(duplicates, assets))
        return 1;
    std::vector<TextRecord> many;
    for (unsigned i = 0; i < 256; ++i)
    {
        many.push_back(placeholder);
        many.back().id = "id" + std::to_string(i);
    }
    if (!developer_catalog(many, {}))
        return 1;
    many.push_back(approved);
    if (developer_catalog(many, assets))
        return 1;
    return 0;
}
