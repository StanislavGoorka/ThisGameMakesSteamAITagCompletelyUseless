#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <atomic>
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
    virtual HRESULT
    register_callback(IXAudio2 *engine,
                      IXAudio2EngineCallback *callback) noexcept = 0;
    virtual void
    unregister_callback(IXAudio2 *engine,
                        IXAudio2EngineCallback *callback) noexcept = 0;
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
    HRESULT
    register_callback(IXAudio2 *engine,
                      IXAudio2EngineCallback *callback) noexcept override
    {
        return engine->RegisterForCallbacks(callback);
    }
    void unregister_callback(IXAudio2 *engine,
                             IXAudio2EngineCallback *callback) noexcept override
    {
        engine->UnregisterForCallbacks(callback);
    }
};

inline XAudio2Api &system_xaudio2_api() noexcept
{
    static SystemXAudio2Api api;
    return api;
}

// Construct, query and destroy on the session thread, within its COM apartment.
// Nonmovable: later engine callbacks need a stable address.
class XAudio2Device : private IXAudio2EngineCallback
{
  public:
    explicit XAudio2Device(XAudio2Api &api = system_xaudio2_api()) noexcept
        : api_(api)
    {
        (void)recreate();
    }
    // No automatic retry. Old callbacks/resources are gone before clearing
    // their error; callback code never calls any lifecycle operation.
    [[nodiscard]] HRESULT recreate() noexcept
    {
        close();
        critical_error_.store(S_OK);
        error_ = api_.create_engine(&engine_);
        if (SUCCEEDED(error_) && !engine_)
            error_ = E_UNEXPECTED;
        if (SUCCEEDED(error_))
        {
            error_ = api_.register_callback(engine_, this);
            registered_ = SUCCEEDED(error_);
        }
        if (SUCCEEDED(error_))
        {
            error_ = api_.create_mastering(engine_, &voice_);
            if (SUCCEEDED(error_) && !voice_)
                error_ = E_UNEXPECTED;
        }
        if (FAILED(error_))
            close();
        return error();
    }
    ~XAudio2Device()
    {
        close();
    }
    XAudio2Device(const XAudio2Device &) = delete;
    XAudio2Device &operator=(const XAudio2Device &) = delete;
    [[nodiscard]] bool available() const noexcept
    {
        return engine_ && voice_ && SUCCEEDED(error());
    }
    [[nodiscard]] HRESULT error() const noexcept
    {
        const auto critical = critical_error_.load();
        return FAILED(critical) ? critical : error_;
    }

  private:
    void STDMETHODCALLTYPE OnProcessingPassStart() noexcept override
    {
    }
    void STDMETHODCALLTYPE OnProcessingPassEnd() noexcept override
    {
    }
    void STDMETHODCALLTYPE OnCriticalError(HRESULT error) noexcept override
    {
        HRESULT expected = S_OK;
        critical_error_.compare_exchange_strong(expected,
                                                FAILED(error) ? error : E_FAIL);
    }
    void close() noexcept
    {
        if (registered_)
            api_.unregister_callback(engine_, this);
        registered_ = false;
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
    static_assert(std::atomic<HRESULT>::is_always_lock_free);
    std::atomic<HRESULT> critical_error_{S_OK};
    bool registered_ = false;
};
} // namespace closed_circuit::windows
