#pragma once
#include "platform/windows/asset_read.hpp"
#include <wincodec.h>
#include <wrl/client.h>

namespace closed_circuit::windows
{
struct DecodedPng
{
    UINT width;
    UINT height;
    UINT stride;
    // Owning, top-down 32bpp premultiplied BGRA, no padding between rows.
    std::vector<std::uint8_t> pixels;
    bool operator==(const DecodedPng &) const = default;
};
struct PngReadResult
{
    std::optional<DecodedPng> image;
    HRESULT error;
};

// Caller initializes COM on this thread. Only verified bytes reach WIC;
// the source buffer outlives every COM object that references its memory.
[[nodiscard]] inline PngReadResult
read_png_asset(const std::filesystem::path &trusted_root,
               const AssetReference &reference)
{
    auto source = read_asset_file(trusted_root, reference);
    if (!source.bytes)
        return {std::nullopt, HRESULT_FROM_WIN32(source.error)};
    Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
    auto result =
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(factory.GetAddressOf()));
    if (FAILED(result))
        return {std::nullopt, result};
    Microsoft::WRL::ComPtr<IWICStream> stream;
    result = factory->CreateStream(stream.GetAddressOf());
    if (FAILED(result))
        return {std::nullopt, result};
    result = stream->InitializeFromMemory(
        source.bytes->data(), static_cast<DWORD>(source.bytes->size()));
    if (FAILED(result))
        return {std::nullopt, result};
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    result = factory->CreateDecoder(GUID_ContainerFormatPng, nullptr,
                                    decoder.GetAddressOf());
    if (FAILED(result))
        return {std::nullopt, result};
    result = decoder->Initialize(stream.Get(), WICDecodeMetadataCacheOnLoad);
    if (FAILED(result))
        return {std::nullopt, result};
    UINT count = 0;
    result = decoder->GetFrameCount(&count);
    if (FAILED(result))
        return {std::nullopt, result};
    if (count != 1)
        return {std::nullopt, WINCODEC_ERR_BADIMAGE};
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    result = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(result))
        return {std::nullopt, result};
    UINT width = 0, height = 0;
    result = frame->GetSize(&width, &height);
    if (FAILED(result))
        return {std::nullopt, result};
    // At most 64 MiB of decoded pixels; arithmetic is bounded before
    // allocation.
    if (!width || !height || width > 4096 || height > 4096)
        return {std::nullopt, WINCODEC_ERR_IMAGESIZEOUTOFRANGE};
    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    result = factory->CreateFormatConverter(converter.GetAddressOf());
    if (FAILED(result))
        return {std::nullopt, result};
    result = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0.0,
                                   WICBitmapPaletteTypeCustom);
    if (FAILED(result))
        return {std::nullopt, result};
    DecodedPng image{width, height, width * 4,
                     std::vector<std::uint8_t>(width * height * 4)};
    result = converter->CopyPixels(nullptr, image.stride,
                                   static_cast<UINT>(image.pixels.size()),
                                   image.pixels.data());
    if (FAILED(result))
        return {std::nullopt, result};
    return {std::move(image), S_OK};
}
} // namespace closed_circuit::windows
