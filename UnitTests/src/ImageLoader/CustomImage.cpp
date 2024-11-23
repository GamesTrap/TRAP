#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/CustomImage.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

TEST_CASE("TRAP::INTERNAL::CustomImage", "[imageloader][customimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::GrayScale, std::vector<u8>(TestImageSDRPixelDataGrayScale.begin(), TestImageSDRPixelDataGrayScale.end()));
    const TRAP::Scope<TRAP::Image> grayscalealpha16bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::GrayScaleAlpha, std::vector<u8>(TestImageSDRPixelDataGrayScaleAlpha.begin(), TestImageSDRPixelDataGrayScaleAlpha.end()));
    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<u8>(TestImageSDRPixelDataRGB.begin(), TestImageSDRPixelDataRGB.end()));
    const TRAP::Scope<TRAP::Image> rgba32bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end()));
    const TRAP::Scope<TRAP::Image> rgb48bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<u16>(TestImageSDR16PixelDataRGB.begin(), TestImageSDR16PixelDataRGB.end()));
    const TRAP::Scope<TRAP::Image> rgba64bpp = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u16>(TestImageSDR16PixelDataRGBA.begin(), TestImageSDR16PixelDataRGBA.end()));
    const TRAP::Scope<TRAP::Image> rgbhdr = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end()));
    const TRAP::Scope<TRAP::Image> rgbahdr = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<f32>(TestImageHDRPixelDataRGBA.begin(), TestImageHDRPixelDataRGBA.end()));

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::INTERNAL::CustomImage>);
        STATIC_REQUIRE(std::copyable<TRAP::INTERNAL::CustomImage>);
        STATIC_REQUIRE(std::movable<TRAP::INTERNAL::CustomImage>);
        STATIC_REQUIRE(std::derived_from<TRAP::INTERNAL::CustomImage, TRAP::Image>);
    }

    SECTION("LoadFromMemory()")
    {
        REQUIRE(grayscale8bpp);
        REQUIRE(grayscalealpha16bpp);
        REQUIRE(rgb24bpp);
        REQUIRE(rgba32bpp);
        REQUIRE(rgb48bpp);
        REQUIRE(rgba64bpp);
        REQUIRE(rgbhdr);
        REQUIRE(rgbahdr);

        const TRAP::Scope<TRAP::Image> empty = TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>{});
        REQUIRE(empty->GetPixelData().empty());
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(grayscale8bpp->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscalealpha16bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(grayscalealpha16bpp->GetPixelData(), TestImageSDRPixelDataGrayScaleAlpha));
        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgba32bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgba32bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgb48bpp->GetPixelData().empty());
        const std::span<const u16> rgb48bppPixelData{reinterpret_cast<const u16*>(rgb48bpp->GetPixelData().data()), rgb48bpp->GetPixelData().size() / sizeof(u16)};
        REQUIRE(std::ranges::equal(rgb48bppPixelData, TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgba64bpp->GetPixelData().empty());
        const std::span<const u16> rgba48bppPixelData{reinterpret_cast<const u16*>(rgba64bpp->GetPixelData().data()), rgba64bpp->GetPixelData().size() / sizeof(u16)};
        REQUIRE(std::ranges::equal(rgba48bppPixelData, TestImageSDR16PixelDataRGBA));
        REQUIRE_FALSE(rgbhdr->GetPixelData().empty());
        const std::span<const f32> rgbhdrPixelData{reinterpret_cast<const f32*>(rgbhdr->GetPixelData().data()), rgbhdr->GetPixelData().size() / sizeof(f32)};
        REQUIRE(std::ranges::equal(rgbhdrPixelData, TestImageHDRPixelDataRGB));
        REQUIRE_FALSE(rgbahdr->GetPixelData().empty());
        const std::span<const f32> rgbahdrPixelData{reinterpret_cast<const f32*>(rgbahdr->GetPixelData().data()), rgbahdr->GetPixelData().size() / sizeof(f32)};
        REQUIRE(std::ranges::equal(rgbahdrPixelData, TestImageHDRPixelDataRGBA));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscalealpha16bpp->GetChannelsPerPixel() == 2u);
        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgba32bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb48bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgba64bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgbhdr->GetChannelsPerPixel() == 3u);
        REQUIRE(rgbahdr->GetChannelsPerPixel() == 4u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerPixel() == 8u);
        REQUIRE(grayscalealpha16bpp->GetBitsPerPixel() == 16u);
        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgba32bpp->GetBitsPerPixel() == 32u);
        REQUIRE(rgb48bpp->GetBitsPerPixel() == 48u);
        REQUIRE(rgba64bpp->GetBitsPerPixel() == 64u);
        REQUIRE(rgbhdr->GetBitsPerPixel() == 96u);
        REQUIRE(rgbahdr->GetBitsPerPixel() == 128u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerPixel() == 1u);
        REQUIRE(grayscalealpha16bpp->GetBytesPerPixel() == 2u);
        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgba32bpp->GetBytesPerPixel() == 4u);
        REQUIRE(rgb48bpp->GetBytesPerPixel() == 6u);
        REQUIRE(rgba64bpp->GetBytesPerPixel() == 8u);
        REQUIRE(rgbhdr->GetBytesPerPixel() == 12u);
        REQUIRE(rgbahdr->GetBytesPerPixel() == 16u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerChannel() == 8u);
        REQUIRE(grayscalealpha16bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgba32bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb48bpp->GetBitsPerChannel() == 16u);
        REQUIRE(rgba64bpp->GetBitsPerChannel() == 16u);
        REQUIRE(rgbhdr->GetBitsPerChannel() == 32u);
        REQUIRE(rgbahdr->GetBitsPerChannel() == 32u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerChannel() == 1u);
        REQUIRE(grayscalealpha16bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgba32bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb48bpp->GetBytesPerChannel() == 2u);
        REQUIRE(rgba64bpp->GetBytesPerChannel() == 2u);
        REQUIRE(rgbhdr->GetBytesPerChannel() == 4u);
        REQUIRE(rgbahdr->GetBytesPerChannel() == 4u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8bpp->GetWidth() == 32u);
        REQUIRE(grayscalealpha16bpp->GetWidth() == 32u);
        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgba32bpp->GetWidth() == 32u);
        REQUIRE(rgb48bpp->GetWidth() == 32u);
        REQUIRE(rgba64bpp->GetWidth() == 32u);
        REQUIRE(rgbhdr->GetWidth() == 32u);
        REQUIRE(rgbahdr->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8bpp->GetHeight() == 32u);
        REQUIRE(grayscalealpha16bpp->GetHeight() == 32u);
        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgba32bpp->GetHeight() == 32u);
        REQUIRE(rgb48bpp->GetHeight() == 32u);
        REQUIRE(rgba64bpp->GetHeight() == 32u);
        REQUIRE(rgbhdr->GetHeight() == 32u);
        REQUIRE(rgbahdr->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscalealpha16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb48bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba64bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgbhdr->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgbahdr->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8bpp->HasAlphaChannel());
        REQUIRE(grayscalealpha16bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE(rgba32bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb48bpp->HasAlphaChannel());
        REQUIRE(rgba64bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgbhdr->HasAlphaChannel());
        REQUIRE(rgbahdr->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8bpp->IsImageGrayScale());
        REQUIRE(grayscalealpha16bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgba32bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb48bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgba64bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgbhdr->IsImageGrayScale());
        REQUIRE_FALSE(rgbahdr->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsImageColored());
        REQUIRE_FALSE(grayscalealpha16bpp->IsImageColored());
        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgba32bpp->IsImageColored());
        REQUIRE(rgb48bpp->IsImageColored());
        REQUIRE(rgba64bpp->IsImageColored());
        REQUIRE(rgbhdr->IsImageColored());
        REQUIRE(rgbahdr->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsHDR());
        REQUIRE_FALSE(grayscalealpha16bpp->IsHDR());
        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgba32bpp->IsHDR());
        REQUIRE_FALSE(rgb48bpp->IsHDR());
        REQUIRE_FALSE(rgba64bpp->IsHDR());
        REQUIRE(rgbhdr->IsHDR());
        REQUIRE(rgbahdr->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8bpp->IsLDR());
        REQUIRE(grayscalealpha16bpp->IsLDR());
        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgba32bpp->IsLDR());
        REQUIRE(rgb48bpp->IsLDR());
        REQUIRE(rgba64bpp->IsLDR());
        REQUIRE_FALSE(rgbhdr->IsLDR());
        REQUIRE_FALSE(rgbahdr->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8bpp->GetFilePath().empty());
        REQUIRE(grayscalealpha16bpp->GetFilePath().empty());
        REQUIRE(rgb24bpp->GetFilePath().empty());
        REQUIRE(rgba32bpp->GetFilePath().empty());
        REQUIRE(rgb48bpp->GetFilePath().empty());
        REQUIRE(rgba64bpp->GetFilePath().empty());
        REQUIRE(rgbhdr->GetFilePath().empty());
        REQUIRE(rgbahdr->GetFilePath().empty());
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscalealpha16bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScaleAlpha);
        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgba32bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb48bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgba64bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgbhdr->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgbahdr->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }
}
