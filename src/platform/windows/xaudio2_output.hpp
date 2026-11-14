#pragma once
#include "application/audio_output.hpp"
#include "platform/windows/xaudio2_playback.hpp"

namespace closed_circuit::windows
{
// Only the pinned approved file can create playback. File failures leave a
// usable, silent port; no automatic IO/device retry on delivery.
class XAudio2Output final : public AudioOutput
{
  public:
    explicit XAudio2Output(
        const std::filesystem::path &root = {},
        XAudio2Api &api = system_xaudio2_api(),
        XAudio2VoiceApi &voices = system_xaudio2_voice_api()) noexcept
    {
        try
        {
            if (auto wave = ApprovedDeliveryWave::load(root))
                playback_.emplace(std::move(*wave), api, voices);
        }
        catch (...)
        {
            // Audio is optional even when the file reader throws.
        }
    }
    [[nodiscard]] AudioPlayResult play(AudioCue cue) noexcept override
    {
        if (!is_audio_cue(cue))
            return AudioPlayResult::unknown;
        if (muted())
            return AudioPlayResult::muted;
        return playback_ ? playback_->play() : AudioPlayResult::unavailable;
    }

  private:
    std::optional<XAudio2Playback> playback_;
};
} // namespace closed_circuit::windows
