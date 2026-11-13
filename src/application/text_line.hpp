#pragma once
#include "application/text_catalog.hpp"
#include <cstdint>

namespace closed_circuit
{
// The optional suffix is numeric state, never an unregistered display label.
struct TextLine
{
    std::string_view id;
    std::optional<std::uint64_t> number;
};
[[nodiscard]] inline std::optional<std::string>
format_text_line(const TextCatalog &catalog, TextLine line)
{
    const auto *label = catalog.find(line.id);
    if (!label)
        return std::nullopt;
    auto value = *label;
    if (line.number)
        value += " " + std::to_string(*line.number);
    return value;
}
} // namespace closed_circuit
