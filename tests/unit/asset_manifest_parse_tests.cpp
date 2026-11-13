#include "platform/windows/asset_manifest_parse.hpp"
#include <iostream>
using namespace closed_circuit;
namespace
{
const std::string entry =
    R"({"id":"atlas","path":"assets/approved/a.png","sha256":"000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f","bytes":19451,"status":"approved","author":"synthetic test metadata"})";
std::string document(std::string_view entries)
{
    return "{\"schema_version\":1,\"assets\":[" + std::string(entries) + "]}";
}
std::string replace(std::string text, std::string_view old,
                    std::string_view value)
{
    text.replace(text.find(old), old.size(), value);
    return text;
}
bool rejected(const std::string &text)
{
    if (!windows::parse_asset_manifest(text))
        return true;
    std::cerr << "Unexpected manifest acceptance: " << text.substr(0, 400)
              << '\n';
    return false;
}
bool valid_values()
{
    const auto input = document(entry);
    const auto parsed = windows::parse_asset_manifest(input);
    if (!parsed || parsed->size() != 1 || input != document(entry))
        return false;
    const auto &ref = parsed->front();
    if (ref.id() != "atlas" || ref.path() != "assets/approved/a.png" ||
        ref.bytes() != 19451)
        return false;
    for (std::size_t i = 0; i < 32; ++i)
        if (ref.sha256()[i] != i)
            return false;
    auto copy = *parsed;
    copy.clear();
    if (parsed->size() != 1 || find_asset(*parsed, "missing"))
        return false;
    const auto empty = windows::parse_asset_manifest(document(""));
    const auto escaped =
        replace(replace(input, "atlas", R"(\u0061tlas)"),
                "assets/approved/a.png", R"(assets\/approved\/a.png)");
    const auto zero =
        windows::parse_asset_manifest(replace(input, "19451", "0"));
    const auto maximum =
        windows::parse_asset_manifest(replace(input, "19451", "67108864"));
    return empty && empty->empty() &&
           windows::parse_asset_manifest(escaped) == parsed && zero &&
           zero->front().bytes() == 0 && maximum &&
           maximum->front().bytes() == 67108864;
}
bool bad_fields()
{
    for (const auto text :
         {"", "null", "[]", "{}", "1", "true", "\"text\"", "{", "{]",
          "{\"schema_version\":1}", "{\"assets\":[]}",
          "{\"schema_version\":1,\"assets\":null}",
          "{\"schema_version\":1,\"assets\":{}}"})
        if (!rejected(text))
            return false;
    const auto input = document(entry);
    for (const auto version :
         {"0", "2", "1.5", "true", "null", "\"1\"", "[]", "{}"})
        if (!rejected(replace(input, "\"schema_version\":1",
                              "\"schema_version\":" + std::string(version))))
            return false;
    for (const auto value : {"true", "null", "[]", "{}", "\"19451\"", "-1",
                             "0.5", "67108865", "1e300", "1e9999"})
        if (!rejected(replace(input, "19451", value)))
            return false;
    for (
        const auto field :
        {R"("id":"atlas")", R"("path":"assets/approved/a.png")",
         R"("sha256":"000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f")",
         R"("status":"approved")"})
    {
        const auto colon = std::string_view(field).find(':');
        const auto key = std::string(field).substr(0, colon + 1);
        for (const auto value : {"0", "true", "null", "[]", "{}"})
            if (!rejected(replace(input, field, key + value)))
                return false;
        if (!rejected(replace(input, std::string(field) + ',', "")))
            return false;
    }
    if (!rejected(replace(input, "\"bytes\":19451,", "")))
        return false;
    for (const auto value : {"pending", "denied", "Approved", ""})
        if (!rejected(replace(input, "\"approved\"",
                              "\"" + std::string(value) + "\"")))
            return false;
    for (const auto value : {"Bad", "bad/id", "", "\\u0000"})
        if (!rejected(replace(input, "atlas", value)))
            return false;
    for (const auto value :
         {"assets/approved/../a.png", "assets/approved/CON.png",
          "assets/approved/a:ads", "assets/approved/\\u0000"})
        if (!rejected(replace(input, "assets/approved/a.png", value)))
            return false;
    for (const auto value :
         {"", "00",
          "g00102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"})
        if (!rejected(replace(input,
                              "000102030405060708090a0b0c0d0e0f1011121314151617"
                              "18191a1b1c1d1e1f",
                              value)))
            return false;
    return rejected(input + "x") && rejected(document("null")) &&
           rejected(document(entry + ",")) &&
           rejected(input + std::string(1, '\0'));
}
bool limits_and_duplicates()
{
    const auto duplicate_id =
        replace(entry, "assets/approved/a.png", "assets/approved/b.png");
    const auto duplicate_path =
        replace(replace(entry, "atlas", "other"), "a.png", "A.png");
    if (!rejected(document(entry + ',' + duplicate_id)) ||
        !rejected(document(entry + ',' + duplicate_path)))
        return false;
    std::string entries;
    for (unsigned i = 0; i < 256; ++i)
    {
        if (i)
            entries += ',';
        entries += replace(replace(entry, "atlas", "id" + std::to_string(i)),
                           "a.png", "a" + std::to_string(i) + ".png");
    }
    const auto full = windows::parse_asset_manifest(document(entries));
    if (!full || full->size() != 256 ||
        !rejected(document(entries + ',' + entry)))
        return false;
    auto size_limit = document("");
    size_limit.resize(1024 * 1024, ' ');
    if (!windows::parse_asset_manifest(size_limit) ||
        !rejected(size_limit + ' '))
        return false;
    std::string nested = "0";
    for (unsigned i = 0; i < 31; ++i)
        nested = '[' + nested + ']';
    const auto prefix = "{\"schema_version\":1,\"assets\":[],\"extra\":";
    if (!windows::parse_asset_manifest(prefix + nested + '}') ||
        !rejected(prefix + ('[' + nested + ']') + '}'))
        return false;
    // Thousands of brackets and escaped quotes inside strings are not nesting.
    const auto quoted =
        prefix + std::string("\"\\\"\\\\") + std::string(1000, '[') + "\"}";
    return windows::parse_asset_manifest(quoted).has_value();
}
bool encoding()
{
    const auto input = document(entry);
    if (!rejected(std::string("\xef\xbb\xbf") + input))
        return false;
    for (const auto invalid :
         {"\xff", "\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "\xe2\x82"})
        if (!rejected(replace(input, "synthetic test metadata", invalid)))
            return false;
    return windows::parse_asset_manifest(replace(input,
                                                 "synthetic test metadata",
                                                 "\xc5\x81\xc3\xb3"
                                                 "d\xc5\xba")) &&
           windows::parse_asset_manifest(input);
}
struct Apartment
{
    Apartment()
    {
        winrt::init_apartment();
    }
    ~Apartment()
    {
        winrt::uninit_apartment();
    }
};
} // namespace
int main()
{
    try
    {
        Apartment apartment;
        if (!valid_values() || !bad_fields() || !limits_and_duplicates() ||
            !encoding())
            return 1;
        std::cout << "PASS: bounded UTF-8 manifest values, errors, copies and "
                     "retry\n";
        return 0;
    }
    catch (const winrt::hresult_error &error)
    {
        std::cerr << "WinRT unavailable: " << error.code().value << '\n';
        return 2;
    }
}
