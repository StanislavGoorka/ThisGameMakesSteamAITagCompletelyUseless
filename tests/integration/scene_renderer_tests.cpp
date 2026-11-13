#include "../support/state_checks.hpp"
#include "platform/windows/atlas_renderer.hpp"
#include "presentation/scene_projection.hpp"
using namespace closed_circuit;
namespace p = presentation;
using Microsoft::WRL::ComPtr;
bool overlaps(const p::TileDraw &a, const p::TileDraw &b)
{
    return a.x < b.x + b.size && a.x + a.size > b.x && a.y < b.y + b.size &&
           a.y + a.size > b.y;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    GameState state{.player_position = *NodeId::from_value(24)};
    for (unsigned i = 0; i < 49; ++i)
        state.ports[i] = *PortMask::from_bits(i % 16);
    state.packet = Packet{*NodeId::from_value(24), *NodeId::from_value(48)};
    state.pulses = {{*EntityId::from_value(9), *NodeId::from_value(24)},
                    {*EntityId::from_value(1), *NodeId::from_value(48)}};
    state.sector_entry = *NodeId::from_value(0);
    state.sector_exit = *NodeId::from_value(48);
    state.receiver_nodes[1] = state.receiver_nodes[2] = true;
    state.powered_nodes[2] = true;
    const auto original = state;
    const std::array route{*NodeId::from_value(24), *NodeId::from_value(25),
                           *NodeId::from_value(48)};
    const auto scene = *p::scene_draws(state, 448, 448, route);
    const auto units = *p::unit_draws(state, 448, 448);
    if (scene.size() != 49 + units.size() + route.size() ||
        p::scene_draws(state, 0, 448, route))
        return 2;
    for (unsigned i = 0; i < 49; ++i)
        if (scene[i].x != (i % 7) * 64 + 16 ||
            scene[i].y != (i / 7) * 64 + 16 || scene[i].size != 32 ||
            scene[i].tile != p::tile_for_ports(state.ports[i]))
            return 3;
    for (unsigned i = 0; i < units.size(); ++i)
    {
        if (scene[49 + i] != units[i].draw)
            return 4;
        for (unsigned j = 0; j < 49; ++j)
            if (overlaps(scene[j], units[i].draw))
                return 5;
    }
    for (unsigned i = 0; i < units.size(); ++i)
        for (unsigned j = 0; j < i; ++j)
            if (overlaps(units[i].draw, units[j].draw))
                return 6;
    for (unsigned i = 0; i < route.size(); ++i)
    {
        const auto &marker = scene[49 + units.size() + i];
        if (marker.tile != p::AtlasTile{45, 8, 0} || marker.size != 16 ||
            marker.x != (route[i].value() % 7) * 64 ||
            marker.y != (route[i].value() / 7) * 64 + 24)
            return 10;
        for (unsigned j = 0; j < 49 + units.size(); ++j)
            if (overlaps(marker, scene[j]))
                return 11;
    }
    ComPtr<IWICImagingFactory> wic;
    ComPtr<IWICBitmap> image;
    ComPtr<ID2D1Factory> factory;
    ComPtr<ID2D1RenderTarget> target;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(wic.GetAddressOf()))) ||
        FAILED(wic->CreateBitmap(448, 448, GUID_WICPixelFormat32bppPBGRA,
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
        return 7;
    const AssetManifest manifest{p::atlas_reference()};
    auto renderer =
        windows::AtlasRenderer::create(target.Get(), argv[1], manifest);
    if (!renderer.renderer || FAILED(renderer.renderer->draw(scene)))
        return 8;
    const auto source =
        *windows::read_png_asset(argv[1], p::atlas_reference()).image;
    // Independent integer composition of existing pixels, never an asset.
    std::vector<std::uint8_t> expected(448 * 448 * 4, 0);
    for (unsigned i = 3; i < expected.size(); i += 4)
        expected[i] = 255;
    for (const auto &draw : scene)
    {
        const auto size = static_cast<unsigned>(draw.size);
        for (unsigned y = 0; y < size; ++y)
            for (unsigned x = 0; x < size; ++x)
            {
                unsigned sx = x * 16 / size, sy = y * 16 / size;
                for (unsigned q = 0; q < draw.tile.quarter_turns; ++q)
                {
                    const auto old = sx;
                    sx = sy;
                    sy = 15 - old;
                }
                const auto from = (draw.tile.row * 17 + sy) * source.stride +
                                  (draw.tile.column * 17 + sx) * 4;
                const auto to = ((static_cast<unsigned>(draw.y) + y) * 448 +
                                 static_cast<unsigned>(draw.x) + x) *
                                4;
                if (source.pixels[from + 3])
                    for (unsigned c = 0; c < 3; ++c)
                        expected[to + c] = source.pixels[from + c];
            }
    }
    ComPtr<IWICBitmapLock> lock;
    UINT size = 0, stride = 0;
    BYTE *data = nullptr;
    if (FAILED(image->Lock(nullptr, WICBitmapLockRead, lock.GetAddressOf())) ||
        FAILED(lock->GetDataPointer(&size, &data)) ||
        FAILED(lock->GetStride(&stride)) || stride != 448 * 4 ||
        size != expected.size() ||
        !std::equal(expected.begin(), expected.end(), data) ||
        !test::same_state(state, original))
        return 9;
    return 0;
}
