#pragma once
#include "application/asset_reference.hpp"

namespace closed_circuit
{
enum class TextAudience
{
    developer,
    public_release
};
enum class TextStatus
{
    ai_placeholder,
    approved
};
struct TextRecord
{
    std::string id;
    std::string value;
    TextStatus status;
    std::optional<std::string> source_asset_id;
    bool operator==(const TextRecord &) const = default;
};

// Owning, validated values only. IDs are never returned as display fallbacks.
// The caller supplies the approved manifest checked by the content gate.
class TextCatalog
{
  public:
    [[nodiscard]] static std::optional<TextCatalog>
    from_records(std::span<const TextRecord> records,
                 std::span<const AssetReference> assets, TextAudience audience)
    {
        if ((audience != TextAudience::developer &&
             audience != TextAudience::public_release) ||
            records.size() > 256 || !is_asset_manifest_valid(assets))
            return std::nullopt;
        for (std::size_t i = 0; i < records.size(); ++i)
        {
            const auto &record = records[i];
            if (!asset_detail::valid_id(record.id) || record.value.empty() ||
                record.value.size() > 4096 ||
                record.value.find('\0') != std::string::npos)
                return std::nullopt;
            if (record.status == TextStatus::ai_placeholder)
            {
                if (audience == TextAudience::public_release ||
                    !record.value.starts_with("[AI PLACEHOLDER]") ||
                    record.source_asset_id)
                    return std::nullopt;
            }
            else if (record.status == TextStatus::approved)
            {
                if (record.value.find("[AI PLACEHOLDER]") !=
                        std::string::npos ||
                    !record.source_asset_id ||
                    !find_asset(assets, *record.source_asset_id))
                    return std::nullopt;
            }
            else
                return std::nullopt;
            for (std::size_t j = 0; j < i; ++j)
                if (records[j].id == record.id)
                    return std::nullopt;
        }
        return TextCatalog{
            std::vector<TextRecord>(records.begin(), records.end())};
    }
    [[nodiscard]] const std::string *find(std::string_view id) const noexcept
    {
        for (const auto &record : records_)
            if (record.id == id)
                return &record.value;
        return nullptr;
    }
    [[nodiscard]] std::span<const TextRecord> records() const noexcept
    {
        return records_;
    }

  private:
    explicit TextCatalog(std::vector<TextRecord> records)
        : records_(std::move(records))
    {
    }
    std::vector<TextRecord> records_;
};
} // namespace closed_circuit
