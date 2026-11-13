#include "../support/checkpoint_files.hpp"
#include "platform/windows/asset_manifest_parse.hpp"
#include "platform/windows/atlas_renderer.hpp"
#include <iostream>
#include <limits>
using namespace closed_circuit;
namespace p = presentation;
using Microsoft::WRL::ComPtr;

std::vector<std::uint8_t> pixels(IWICBitmap *image)
{
    ComPtr<IWICBitmapLock> lock;
    if (FAILED(image->Lock(nullptr, WICBitmapLockRead, lock.GetAddressOf())))
        return {};
    UINT size = 0, stride = 0;
    BYTE *data = nullptr;
    if (FAILED(lock->GetDataPointer(&size, &data)) ||
        FAILED(lock->GetStride(&stride)) || stride != 128 * 4)
        return {};
    return {data, data + size};
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2)
        return 1;
    winrt::init_apartment();
    const std::filesystem::path root(argv[1]);
    const AssetManifest manifest{p::atlas_reference()};
    if (windows::AtlasRenderer::create(nullptr, root, manifest).renderer)
        return 2;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<IWICBitmap> image;
    ComPtr<ID2D1Factory> factory;
    ComPtr<ID2D1RenderTarget> target;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(wic.GetAddressOf()))) ||
        FAILED(wic->CreateBitmap(128, 128, GUID_WICPixelFormat32bppPBGRA,
                                 WICBitmapCacheOnLoad, image.GetAddressOf())) ||
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 factory.GetAddressOf())) ||
        FAILED(factory->CreateWicBitmapRenderTarget(
            image.Get(),
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            target.GetAddressOf())))
        return 3;
    auto created = windows::AtlasRenderer::create(target.Get(), root, manifest);
    if (!created.renderer || created.error != S_OK)
        return 4;
    std::array<p::TileDraw, 16> draws{};
    for (unsigned i = 0; i < 16; ++i)
        draws[i] = {p::tile_for_ports(*PortMask::from_bits(i)),
                    static_cast<float>((i % 4) * 32),
                    static_cast<float>((i / 4) * 32), 32};
    const D2D1_MATRIX_3X2_F previous{2, 0, 0, 2, 13, 7};
    target->SetTransform(previous);
    if (FAILED(created.renderer->draw(draws)))
        return 5;
    D2D1_MATRIX_3X2_F restored{};
    target->GetTransform(&restored);
    if (restored._11 != 2 || restored._22 != 2 || restored._31 != 13 ||
        restored._32 != 7)
        return 6;
    const auto actual = pixels(image.Get());
    const auto source =
        *windows::read_png_asset(root, p::atlas_reference()).image;
    if (actual.size() != 128 * 128 * 4)
        return 7;
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned y = 0; y < 32; ++y)
            for (unsigned x = 0; x < 32; ++x)
            {
                const auto tile = draws[i].tile;
                unsigned sx = x / 2, sy = y / 2;
                for (unsigned q = 0; q < tile.quarter_turns; ++q)
                {
                    const auto old_x = sx;
                    sx = sy;
                    sy = 15 - old_x;
                }
                const auto from = (tile.row * 17 + sy) * source.stride +
                                  (tile.column * 17 + sx) * 4;
                const auto to = ((i / 4 * 32 + y) * 128 + i % 4 * 32 + x) * 4;
                for (unsigned channel = 0; channel < 4; ++channel)
                {
                    const auto expected =
                        channel == 3 ? 255 : source.pixels[from + channel];
                    if (actual[to + channel] != expected)
                    {
                        std::cerr << "Mismatch mask/x/y/channel " << i << '/'
                                  << x << '/' << y << '/' << channel << '\n';
                        return 8;
                    }
                }
            }
    for (unsigned failure = 0; failure < 4; ++failure)
    {
        auto invalid = draws;
        if (failure == 0)
            invalid.back().tile.column = 49;
        if (failure == 1)
            invalid.back().tile.quarter_turns = 4;
        if (failure == 2)
            invalid.back().size = 0;
        if (failure == 3)
            invalid.back().x = std::numeric_limits<float>::quiet_NaN();
        if (created.renderer->draw(invalid) != E_INVALIDARG ||
            pixels(image.Get()) != actual)
            return 9;
    }
    test::CheckpointDirectory directory;
    auto missing =
        windows::AtlasRenderer::create(target.Get(), directory.path, manifest);
    if (missing.renderer || !FAILED(missing.error))
        return 10;
    const auto copy = directory.path / p::atlas_reference().path();
    std::filesystem::create_directories(copy.parent_path());
    auto bytes = test::read_bytes(root / p::atlas_reference().path());
    test::write_bytes(copy, bytes);
    if (!windows::AtlasRenderer::create(target.Get(), directory.path, manifest)
             .renderer)
        return 11;
    bytes.front() ^= 1;
    test::write_bytes(copy, bytes);
    const auto corrupt =
        windows::AtlasRenderer::create(target.Get(), directory.path, manifest);
    if (corrupt.renderer || corrupt.error != HRESULT_FROM_WIN32(ERROR_CRC))
        return 12;
    const AssetManifest wrong{*AssetReference::from_fields(
        p::atlas_reference().id(), p::atlas_reference().path(),
        std::string(64, '0'), 19451)};
    if (windows::AtlasRenderer::create(target.Get(), root, wrong).error !=
            E_INVALIDARG ||
        windows::AtlasRenderer::create(target.Get(), root, {}).renderer)
        return 13;
    if (FAILED(created.renderer->draw(draws)) || pixels(image.Get()) != actual)
        return 14;
    return 0;
}
