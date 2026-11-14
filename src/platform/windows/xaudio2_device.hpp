#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <xaudio2.h>

namespace closed_circuit::windows
{
// Borrowed API seam; it must outlive the device. No sample data crosses it.
class XAudio2Api
{
  public:
    virtual ~XAudio2Api() = default;
    virtual HRESULT create_engine(IXAudio2 **engine) noexcept = 0;
    virtual HRESULT
    create_mastering(IXAudio2 *engine,
                     IXAudio2MasteringVoice **voice) noexcept = 0;
    virtual void destroy_mastering(IXAudio2MasteringVoice *voice) noexcept = 0;
    virtual void release_engine(IXAudio2 *engine) noexcept = 0;
};

class SystemXAudio2Api final : public XAudio2Api
{
  public:
    HRESULT create_engine(IXAudio2 **engine) noexcept override
    {
        return XAudio2Create(engine, 0, XAUDIO2_DEFAULT_PROCESSOR);
    }
    HRESULT create_mastering(IXAudio2 *engine,
                             IXAudio2MasteringVoice **voice) noexcept override
    {
        return engine->CreateMasteringVoice(voice);
    }
    void destroy_mastering(IXAudio2MasteringVoice *voice) noexcept override
    {
        voice->DestroyVoice();
    }
    void release_engine(IXAudio2 *engine) noexcept override
    {
        engine->Release();
    }
};

inline XAudio2Api &system_xaudio2_api() noexcept
{
    static SystemXAudio2Api api;
    return api;
}

// Construct, query and destroy on the session thread, within its COM apartment.
// Nonmovable: later engine callbacks need a stable address.
class XAudio2Device
{
  public:
    explicit XAudio2Device(XAudio2Api &api = system_xaudio2_api()) noexcept
        : api_(api)
    {
        error_ = api_.create_engine(&engine_);
        if (SUCCEEDED(error_) && !engine_)
            error_ = E_UNEXPECTED;
        if (SUCCEEDED(error_))
        {
            error_ = api_.create_mastering(engine_, &voice_);
            if (SUCCEEDED(error_) && !voice_)
                error_ = E_UNEXPECTED;
        }
        if (FAILED(error_))
            close();
    }
    ~XAudio2Device()
    {
        close();
    }
    XAudio2Device(const XAudio2Device &) = delete;
    XAudio2Device &operator=(const XAudio2Device &) = delete;
    [[nodiscard]] bool available() const noexcept
    {
        return engine_ && voice_ && SUCCEEDED(error_);
    }
    [[nodiscard]] HRESULT error() const noexcept
    {
        return error_;
    }

  private:
    void close() noexcept
    {
        if (voice_)
            api_.destroy_mastering(voice_);
        voice_ = nullptr;
        if (engine_)
            api_.release_engine(engine_);
        engine_ = nullptr;
    }
    XAudio2Api &api_;
    IXAudio2 *engine_ = nullptr;
    IXAudio2MasteringVoice *voice_ = nullptr;
    HRESULT error_ = E_FAIL;
};
} // namespace closed_circuit::windows
