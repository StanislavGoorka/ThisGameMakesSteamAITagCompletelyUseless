#pragma once
#include "application/asset_reference.hpp"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <cmath>
// Win32's GDI macro must not rename the WinRT JsonValue::GetObject method.
#pragma push_macro("GetObject")
#undef GetObject
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace closed_circuit::windows
{
namespace manifest_json_detail
{
// Bound native JSON parsing before activation/allocation. Brackets in strings
// (including escaped quotes/backslashes) do not contribute to nesting depth.
inline bool bounded(std::string_view text) noexcept
{
    if (text.empty() || text.size() > 1024 * 1024 ||
        text.starts_with("\xef\xbb\xbf"))
        return false;
    unsigned depth = 0;
    bool quoted = false, escaped = false;
    for (const auto c : text)
    {
        if (quoted)
        {
            if (escaped)
                escaped = false;
            else if (c == '\\')
                escaped = true;
            else if (c == '"')
                quoted = false;
        }
        else if (c == '"')
            quoted = true;
        else if (c == '{' || c == '[')
        {
            if (++depth > 32)
                return false;
        }
        else if (c == '}' || c == ']')
        {
            if (depth == 0)
                return false;
            --depth;
        }
    }
    return depth == 0 && !quoted;
}
inline std::optional<std::string>
string_field(const winrt::Windows::Data::Json::JsonObject &object,
             const wchar_t *name)
{
    const auto value = object.GetNamedValue(name);
    if (value.ValueType() != winrt::Windows::Data::Json::JsonValueType::String)
        return std::nullopt;
    return winrt::to_string(value.GetString());
}
} // namespace manifest_json_detail

// The caller owns the WinRT apartment. No IO, cache, renderer or partial
// result.
[[nodiscard]] inline std::optional<AssetManifest>
parse_asset_manifest(std::string_view text)
{
    if (!manifest_json_detail::bounded(text) ||
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                            static_cast<int>(text.size()), nullptr, 0) == 0)
        return std::nullopt;
    namespace json = winrt::Windows::Data::Json;
    try
    {
        json::JsonObject root{nullptr};
        if (!json::JsonObject::TryParse(winrt::to_hstring(text), root))
            return std::nullopt;
        const auto version = root.GetNamedValue(L"schema_version");
        if (version.ValueType() != json::JsonValueType::Number ||
            version.GetNumber() != 1)
            return std::nullopt;
        const auto entries = root.GetNamedArray(L"assets");
        if (entries.Size() > asset_count_limit)
            return std::nullopt;
        AssetManifest result;
        result.reserve(entries.Size());
        for (const auto &entry : entries)
        {
            const auto object = entry.GetObject();
            const auto id = manifest_json_detail::string_field(object, L"id");
            const auto path =
                manifest_json_detail::string_field(object, L"path");
            const auto digest =
                manifest_json_detail::string_field(object, L"sha256");
            const auto status =
                manifest_json_detail::string_field(object, L"status");
            const auto size = object.GetNamedValue(L"bytes");
            if (!id || !path || !digest || !status || *status != "approved" ||
                size.ValueType() != json::JsonValueType::Number)
                return std::nullopt;
            const auto number = size.GetNumber();
            if (!std::isfinite(number) || number < 0 ||
                number > asset_byte_limit || std::floor(number) != number)
                return std::nullopt;
            auto reference = AssetReference::from_fields(
                *id, *path, *digest, static_cast<std::uint64_t>(number));
            if (!reference)
                return std::nullopt;
            result.push_back(std::move(*reference));
        }
        if (!is_asset_manifest_valid(result))
            return std::nullopt;
        return result;
    }
    catch (const winrt::hresult_error &)
    {
        return std::nullopt;
    }
}
} // namespace closed_circuit::windows
#pragma pop_macro("GetObject")
