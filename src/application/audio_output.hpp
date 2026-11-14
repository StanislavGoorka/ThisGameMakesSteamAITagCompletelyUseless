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
    // Applies to subsequent cues; never queues a muted cue for later replay.
    void set_muted(bool value) noexcept
    {
        muted_ = value;
    }
    [[nodiscard]] bool muted() const noexcept
    {
        return muted_;
    }

  private:
    bool muted_ = false;
};

// Used when no device or approved file exists. Never produces sound.
class SilentAudioOutput final : public AudioOutput
{
  public:
    [[nodiscard]] AudioPlayResult play(AudioCue cue) noexcept override
    {
        if (!is_audio_cue(cue))
            return AudioPlayResult::unknown;
        return muted() ? AudioPlayResult::muted : AudioPlayResult::unavailable;
    }
};
} // namespace closed_circuit
