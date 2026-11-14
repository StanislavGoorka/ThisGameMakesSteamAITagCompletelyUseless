#pragma once
#include "application/audio_output.hpp"
#include "platform/windows/approved_wave.hpp"
#include "platform/windows/xaudio2_device.hpp"

namespace closed_circuit::windows
{
class XAudio2VoiceApi
{
  public:
    virtual ~XAudio2VoiceApi() = default;
    virtual HRESULT create_source(IXAudio2 *engine, const WAVEFORMATEX &format,
                                  IXAudio2SourceVoice **voice) noexcept = 0;
    virtual HRESULT submit(IXAudio2SourceVoice *voice,
                           const XAUDIO2_BUFFER &buffer) noexcept = 0;
    virtual HRESULT start(IXAudio2SourceVoice *voice) noexcept = 0;
    // Must stop all reads of submitted data before returning.
    virtual void destroy_source(IXAudio2SourceVoice *voice) noexcept = 0;
};
class SystemXAudio2VoiceApi final : public XAudio2VoiceApi
{
  public:
    HRESULT create_source(IXAudio2 *engine, const WAVEFORMATEX &format,
                          IXAudio2SourceVoice **voice) noexcept override
    {
        return engine->CreateSourceVoice(voice, &format, XAUDIO2_VOICE_NOPITCH,
                                         1.0f);
    }
    HRESULT submit(IXAudio2SourceVoice *voice,
                   const XAUDIO2_BUFFER &buffer) noexcept override
    {
        return voice->SubmitSourceBuffer(&buffer);
    }
    HRESULT start(IXAudio2SourceVoice *voice) noexcept override
    {
        return voice->Start(0, XAUDIO2_COMMIT_NOW);
    }
    void destroy_source(IXAudio2SourceVoice *voice) noexcept override
    {
        voice->DestroyVoice();
    }
};
inline XAudio2VoiceApi &system_xaudio2_voice_api() noexcept
{
    static SystemXAudio2VoiceApi api;
    return api;
}

// Session-thread owner: source voice, device, then approved sample storage.
// A played result means submitted to XAudio2, not a guarantee of audibility.
class XAudio2Playback
{
  public:
    explicit XAudio2Playback(
        ApprovedDeliveryWave wave,
        XAudio2Api &device_api = system_xaudio2_api(),
        XAudio2VoiceApi &voice_api = system_xaudio2_voice_api()) noexcept
        : wave_(std::move(wave)), device_(device_api), api_(voice_api)
    {
        error_ = create_voice();
    }
    ~XAudio2Playback()
    {
        close_voice();
    }
    XAudio2Playback(const XAudio2Playback &) = delete;
    XAudio2Playback &operator=(const XAudio2Playback &) = delete;
    [[nodiscard]] bool available() const noexcept
    {
        return voice_ && device_.available() && SUCCEEDED(error_);
    }
    [[nodiscard]] HRESULT error() const noexcept
    {
        const auto device_error = device_.error();
        return FAILED(device_error) ? device_error : error_;
    }
    [[nodiscard]] HRESULT recreate() noexcept
    {
        close_voice();
        error_ = device_.recreate();
        if (SUCCEEDED(error_))
            error_ = create_voice();
        return error();
    }
    [[nodiscard]] AudioPlayResult play() noexcept
    {
        if (!available())
        {
            close_voice();
            return AudioPlayResult::unavailable;
        }
        const auto data = wave_.view().data;
        XAUDIO2_BUFFER buffer{};
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.AudioBytes = static_cast<UINT32>(data.size());
        buffer.pAudioData = data.data();
        error_ = api_.submit(voice_, buffer);
        if (SUCCEEDED(error_) && !started_)
        {
            error_ = api_.start(voice_);
            started_ = SUCCEEDED(error_);
        }
        if (FAILED(error_))
        {
            // Discard queued buffers as well: a failed call cannot play later.
            close_voice();
            return AudioPlayResult::unavailable;
        }
        return AudioPlayResult::played;
    }

  private:
    HRESULT create_voice() noexcept
    {
        const auto engine = device_.engine();
        if (!engine)
            return device_.error();
        const auto wave = wave_.view();
        if (wave.data.empty())
            return E_INVALIDARG;
        const WAVEFORMATEX format{WAVE_FORMAT_PCM,
                                  wave.channels,
                                  wave.sample_rate,
                                  wave.sample_rate * wave.block_align,
                                  wave.block_align,
                                  16,
                                  0};
        auto result = api_.create_source(engine, format, &voice_);
        if (SUCCEEDED(result) && !voice_)
            result = E_UNEXPECTED;
        if (FAILED(result))
            close_voice();
        return result;
    }
    void close_voice() noexcept
    {
        if (voice_)
            api_.destroy_source(voice_);
        voice_ = nullptr;
        started_ = false;
    }
    ApprovedDeliveryWave wave_;
    XAudio2Device device_;
    XAudio2VoiceApi &api_;
    IXAudio2SourceVoice *voice_ = nullptr;
    HRESULT error_ = E_FAIL;
    bool started_ = false;
};
} // namespace closed_circuit::windows
