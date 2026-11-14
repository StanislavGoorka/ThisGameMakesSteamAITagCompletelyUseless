#pragma once
#include "platform/windows/xaudio2_device.hpp"
#include <algorithm>
#include <array>
#include <span>

namespace closed_circuit::test
{
class FakeXAudio2Api final : public windows::XAudio2Api
{
  public:
    enum Call
    {
        engine,
        mastering,
        destroy,
        release
    };
    HRESULT engine_result = S_OK;
    HRESULT mastering_result = S_OK;
    bool null_engine = false;
    bool null_voice = false;
    bool partial_engine = false;
    bool partial_voice = false;
    bool valid = true;
    std::array<Call, 64> calls{};
    std::size_t count = 0;
    HRESULT create_engine(IXAudio2 **out) noexcept override
    {
        record(engine);
        *out = !null_engine && (SUCCEEDED(engine_result) || partial_engine)
                   ? engine_pointer()
                   : nullptr;
        return engine_result;
    }
    HRESULT create_mastering(IXAudio2 *in,
                             IXAudio2MasteringVoice **out) noexcept override
    {
        valid &= in == engine_pointer();
        record(mastering);
        *out = !null_voice && (SUCCEEDED(mastering_result) || partial_voice)
                   ? voice_pointer()
                   : nullptr;
        return mastering_result;
    }
    void destroy_mastering(IXAudio2MasteringVoice *in) noexcept override
    {
        valid &= in == voice_pointer();
        record(destroy);
    }
    void release_engine(IXAudio2 *in) noexcept override
    {
        valid &= in == engine_pointer();
        record(release);
    }
    bool matches(std::initializer_list<Call> expected) const
    {
        return valid &&
               std::ranges::equal(std::span{calls}.first(count), expected);
    }

  private:
    void record(Call call) noexcept
    {
        if (count < calls.size())
            calls[count++] = call;
        else
            valid = false;
    }
    IXAudio2 *engine_pointer() noexcept
    {
        return reinterpret_cast<IXAudio2 *>(&engine_token_);
    }
    IXAudio2MasteringVoice *voice_pointer() noexcept
    {
        return reinterpret_cast<IXAudio2MasteringVoice *>(&voice_token_);
    }
    int engine_token_ = 0, voice_token_ = 0;
};
} // namespace closed_circuit::test
