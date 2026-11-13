#include "platform/windows/atlas_renderer.hpp"
#include <limits>
using namespace closed_circuit;
using Microsoft::WRL::ComPtr;
struct Surface
{
    ComPtr<IWICBitmap> image;
    ComPtr<ID2D1RenderTarget> target;
};
Surface make_surface(IWICImagingFactory *wic, ID2D1Factory *factory)
{
    Surface s;
    if (FAILED(wic->CreateBitmap(768, 128, GUID_WICPixelFormat32bppPBGRA,
                                 WICBitmapCacheOnLoad,
                                 s.image.GetAddressOf())) ||
        FAILED(factory->CreateWicBitmapRenderTarget(
            s.image.Get(),
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            s.target.GetAddressOf())))
        return {};
    return s;
}
std::vector<BYTE> pixels(IWICBitmap *image)
{
    std::vector<BYTE> result(768 * 128 * 4);
    if (FAILED(image->CopyPixels(
            nullptr, 768 * 4, static_cast<UINT>(result.size()), result.data())))
        return {};
    return result;
}
bool black(const std::vector<BYTE> &image)
{
    if (image.empty())
        return false;
    for (std::size_t i = 0; i < image.size(); i += 4)
        if (image[i] || image[i + 1] || image[i + 2] || image[i + 3] != 255)
            return false;
    return true;
}
int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const TextRecord record{"label",
                            "[AI PLACEHOLDER] Test \xc5\xbc \xf0\x9f\x94\x8c",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto text = windows::TextRenderer::create(catalog, "label");
    if (!text.renderer || text.error != S_OK ||
        text.renderer->text() != L"[AI PLACEHOLDER] Test \u017c \xd83d\xdd0c" ||
        text.renderer->extent().width <= 0 ||
        text.renderer->extent().height <= 0)
        return 2;
    const auto missing = windows::TextRenderer::create(catalog, "absent");
    if (missing.renderer ||
        missing.error != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        return 3;
    auto bad_record = record;
    bad_record.value += '\xff';
    const auto bad_catalog = *TextCatalog::from_records(
        std::span{&bad_record, 1}, {}, TextAudience::developer);
    const auto bad_text = windows::TextRenderer::create(bad_catalog, "label");
    if (bad_text.renderer ||
        bad_text.error != HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION))
        return 4;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<ID2D1Factory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(wic.GetAddressOf()))) ||
        FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 factory.GetAddressOf())))
        return 5;
    const AssetManifest manifest{presentation::atlas_reference()};
    std::vector<BYTE> reference;
    for (int repeat = 0; repeat < 2; ++repeat)
    {
        auto surface = make_surface(wic.Get(), factory.Get());
        if (!surface.target)
            return 6;
        auto atlas = windows::AtlasRenderer::create(surface.target.Get(),
                                                    argv[1], manifest);
        if (!atlas.renderer ||
            FAILED(atlas.renderer->draw({}, text.renderer.get())))
            return 7;
        auto drawn = pixels(surface.image.Get());
        if (black(drawn) || drawn.empty() || (repeat && drawn != reference))
            return 8;
        reference = drawn;
        for (unsigned y = 0; y < 128; ++y)
            for (unsigned x = 0; x < 768; ++x)
                if ((x < 4 || y < 4 || x >= 4 + text.renderer->extent().width ||
                     y >= 4 + text.renderer->extent().height) &&
                    (drawn[(y * 768 + x) * 4] || drawn[(y * 768 + x) * 4 + 1] ||
                     drawn[(y * 768 + x) * 4 + 2]))
                    return 9;
        if (FAILED(atlas.renderer->draw({})))
            return 10;
        surface.target->BeginDraw();
        const auto matrix = D2D1::Matrix3x2F::Translation(5, 7);
        surface.target->SetTransform(matrix);
        const auto result =
            text.renderer->draw(surface.target.Get(), D2D1::RectF(4, 4, 5, 5));
        D2D1_MATRIX_3X2_F after{};
        surface.target->GetTransform(&after);
        if (result != S_FALSE || after.dx != 5 || after.dy != 7 ||
            FAILED(surface.target->EndDraw()) ||
            !black(pixels(surface.image.Get())))
            return 11;
        const auto nan = std::numeric_limits<float>::quiet_NaN();
        for (const auto bounds :
             {D2D1::RectF(nan, 0, 10, 10), D2D1::RectF(-1, 0, 10, 10),
              D2D1::RectF(0, 0, 0, 10), D2D1::RectF(0, 0, 769, 128)})
            if (text.renderer->draw(surface.target.Get(), bounds) !=
                E_INVALIDARG)
                return 12;
        if (text.renderer->draw(nullptr, D2D1::RectF(0, 0, 10, 10)) !=
                E_INVALIDARG ||
            !black(pixels(surface.image.Get())))
            return 13;
    }
    return 0;
}
