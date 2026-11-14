#include "../support/xaudio2_fake.hpp"
#include <thread>
using namespace closed_circuit;
using Fake = test::FakeXAudio2Api;

int main()
{
    Fake api;
    {
        windows::XAudio2Device device{api};
        for (int repeat = 0; repeat < 3; ++repeat)
        {
            if (!device.available() || !api.callback)
                return 1;
            const auto count = api.count;
            std::thread worker{
                [cb = api.callback]
                {
                    cb->OnProcessingPassStart();
                    cb->OnProcessingPassEnd();
                    cb->OnCriticalError(XAUDIO2_E_DEVICE_INVALIDATED);
                    cb->OnCriticalError(E_FAIL);
                }};
            worker.join();
            if (device.available() ||
                device.error() != XAUDIO2_E_DEVICE_INVALIDATED ||
                api.count != count)
                return 2;
            api.critical_on_unregister = true;
            if (device.recreate() != S_OK || !device.available())
                return 3;
        }
        api.engine_result = E_ACCESSDENIED;
        if (device.recreate() != E_ACCESSDENIED || device.available() ||
            api.callback)
            return 4;
        api.engine_result = S_OK;
        api.register_result = E_OUTOFMEMORY;
        if (device.recreate() != E_OUTOFMEMORY || device.available() ||
            api.callback)
            return 5;
        api.register_result = S_OK;
        api.mastering_result = E_FAIL;
        if (device.recreate() != E_ABORT || device.available() || api.callback)
            return 6;
        // The old callback error during cleanup is discarded only by the
        // following explicit recreation, after old engine release.
        api.mastering_result = S_OK;
        if (device.recreate() != S_OK || !device.available())
            return 7;
        api.critical_on_register = true;
        if (device.recreate() != E_ABORT || device.available())
            return 8;
    }
    if (!api.valid || api.callback)
        return 9;
    Fake order;
    {
        windows::XAudio2Device device{order};
        order.callback->OnCriticalError(S_OK);
        if (device.available() || device.error() != E_FAIL)
            return 10;
    }
    if (!order.matches({Fake::engine, Fake::register_engine, Fake::mastering,
                        Fake::unregister_engine, Fake::destroy, Fake::release}))
        return 11;
    Fake registration;
    registration.register_result = E_FAIL;
    {
        windows::XAudio2Device device{registration};
        if (device.available() || device.error() != E_FAIL)
            return 12;
    }
    if (!registration.matches(
            {Fake::engine, Fake::register_engine, Fake::release}))
        return 13;
    return 0;
}
