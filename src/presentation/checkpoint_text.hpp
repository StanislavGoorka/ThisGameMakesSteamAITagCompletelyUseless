#pragma once
#include "application/session_resume.hpp"
#include "application/text_line.hpp"

namespace closed_circuit::presentation
{
// Only catalog IDs leave this layer: platform codes and paths are never text.
[[nodiscard]] inline std::optional<TextLine>
resume_line(ResumeStatus status) noexcept
{
    switch (status)
    {
    case ResumeStatus::corrupt:
        return TextLine{"checkpoint-corrupt", std::nullopt};
    case ResumeStatus::unavailable:
        return TextLine{"checkpoint-unavailable", std::nullopt};
    default:
        return std::nullopt;
    }
}
[[nodiscard]] inline std::optional<TextLine>
checkpoint_storage_line(bool failed) noexcept
{
    return failed ? std::optional{TextLine{"checkpoint-unsaved", std::nullopt}}
                  : std::nullopt;
}
} // namespace closed_circuit::presentation
