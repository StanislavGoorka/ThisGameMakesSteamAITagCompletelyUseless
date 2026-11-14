#include "../support/xaudio2_fake.hpp"
#include "platform/windows/xaudio2_device.hpp"
#include <iostream>
#include <type_traits>
using namespace closed_circuit;
using Fake = test::FakeXAudio2Api;
static_assert(!std::is_copy_constructible_v<windows::XAudio2Device>);
static_assert(!std::is_move_constructible_v<windows::XAudio2Device>);
static_assert(std::is_nothrow_constructible_v<windows::XAudio2Device>);

int main()
{
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        Fake api;
        {
            windows::XAudio2Device device{api};
            if (!device.available() || !device.engine() ||
                device.error() != S_OK ||
                !api.matches(
                    {Fake::engine, Fake::register_engine, Fake::mastering}))
                return 1;
        }
        if (!api.matches({Fake::engine, Fake::register_engine, Fake::mastering,
                          Fake::unregister_engine, Fake::destroy,
                          Fake::release}))
            return 2;
        for (int failure = 0; failure < 6; ++failure)
        {
            Fake broken;
            if (failure == 0 || failure == 4)
                broken.engine_result = E_ACCESSDENIED;
            if (failure == 1 || failure == 5)
                broken.mastering_result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            broken.null_engine = failure == 2;
            broken.null_voice = failure == 3;
            broken.partial_engine = failure == 4;
            broken.partial_voice = failure == 5;
            {
                windows::XAudio2Device device{broken};
                const auto expected = failure == 0 || failure == 4
                                          ? E_ACCESSDENIED
                                      : failure == 1 || failure == 5
                                          ? HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
                                          : E_UNEXPECTED;
                if (device.available() || device.engine() ||
                    device.error() != expected)
                    return 3;
            }
            if (failure == 0 || failure == 2)
            {
                if (!broken.matches({Fake::engine}))
                    return 4;
            }
            else if (failure == 4)
            {
                if (!broken.matches({Fake::engine, Fake::release}))
                    return 5;
            }
            else if (failure == 5)
            {
                if (!broken.matches({Fake::engine, Fake::register_engine,
                                     Fake::mastering, Fake::unregister_engine,
                                     Fake::destroy, Fake::release}))
                    return 6;
            }
            else if (!broken.matches({Fake::engine, Fake::register_engine,
                                      Fake::mastering, Fake::unregister_engine,
                                      Fake::release}))
                return 7;
        }
    }
    const auto apartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(apartment))
        return 8;
    for (int repeat = 0; repeat < 3; ++repeat)
    {
        windows::XAudio2Device real;
        std::cout << "XAudio2 HRESULT=" << real.error()
                  << " available=" << real.available() << '\n';
        if (real.available() != bool(SUCCEEDED(real.error())))
            return 9;
    }
    CoUninitialize();
    return 0;
}
