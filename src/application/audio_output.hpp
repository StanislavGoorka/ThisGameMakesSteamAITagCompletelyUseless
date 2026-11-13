#pragma once
#include <cstdint>

namespace closed_circuit
{
// A logical signal, not sound content. A cue plays only after an approved
// file is registered for it; nothing here holds or creates audio data.
enum class AudioCue : std::uint8_t
{
    delivery
};

[[nodiscard]] constexpr bool is_audio_cue(AudioCue cue) noexcept
{
    return cue == AudioCue::delivery;
}

enum class AudioPlayResult : std::uint8_t
{
    played,
    muted,
    unavailable,
    unknown
};

// Fire-and-forget from one session thread. Any result other than played
// leaves gameplay unchanged; callers never wait for or retry a cue.
class AudioOutput
{
  public:
    virtual ~AudioOutput() = default;
    [[nodiscard]] virtual AudioPlayResult play(AudioCue cue) = 0;
};

// Used when no device or approved file exists. Never produces sound.
class SilentAudioOutput final : public AudioOutput
{
  public:
    [[nodiscard]] AudioPlayResult play(AudioCue cue) noexcept override
    {
        return is_audio_cue(cue) ? AudioPlayResult::unavailable
                                 : AudioPlayResult::unknown;
    }
};
} // namespace closed_circuit
