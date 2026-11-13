#pragma once
#include "application/text_catalog.hpp"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace closed_circuit::windows
{
struct TextReadResult
{
    std::optional<std::wstring> text;
    DWORD error;
};

// Catalog policy owns provenance/markers; conversion must preserve every
// character and reject malformed UTF-8 rather than replacing it.
[[nodiscard]] inline TextReadResult decode_text_utf16(std::string_view text)
{
    if (text.empty() || text.size() > 1000000)
        return {std::nullopt, ERROR_INVALID_DATA};
    const auto length =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                            static_cast<int>(text.size()), nullptr, 0);
    if (!length)
        return {std::nullopt, GetLastError()};
    std::wstring value(length, L'\0');
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                             static_cast<int>(text.size()), value.data(),
                             length))
        return {std::nullopt, GetLastError()};
    return {std::move(value), ERROR_SUCCESS};
}
[[nodiscard]] inline TextReadResult read_text_utf16(const TextCatalog &catalog,
                                                    std::string_view id)
{
    const auto *text = catalog.find(id);
    return text ? decode_text_utf16(*text)
                : TextReadResult{std::nullopt, ERROR_NOT_FOUND};
}
} // namespace closed_circuit::windows
