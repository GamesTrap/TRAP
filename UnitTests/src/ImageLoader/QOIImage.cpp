#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

TEST_CASE("TRAP::INTERNAL::QOIImage", "[imageloader][qoiimage]")
{
    const TRAP::Scope<TRAP::Image> linear24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/QOI/Test24BPPLinear.qoi");
    const TRAP::Scope<TRAP::Image> linear32bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/QOI/Test32BPPLinear.qoi");

    const TRAP::Scope<TRAP::Image> srgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/QOI/Test24BPPsRGB.qoi");
    const TRAP::Scope<TRAP::Image> srgb32bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/QOI/Test32BPPsRGB.qoi");

    SECTION("LoadFromFile()")
    {
        REQUIRE(linear24bpp);
        REQUIRE(linear32bpp);

        REQUIRE(srgb24bpp);
        REQUIRE(srgb32bpp);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(linear24bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(linear24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(linear32bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(linear32bpp->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(srgb24bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(srgb24bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(srgb32bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(srgb32bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(linear24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(linear32bpp->GetChannelsPerPixel() == 4u);

        REQUIRE(srgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(srgb32bpp->GetChannelsPerPixel() == 4u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(linear24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(linear32bpp->GetBitsPerPixel() == 32u);

        REQUIRE(srgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(srgb32bpp->GetBitsPerPixel() == 32u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(linear24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(linear32bpp->GetBytesPerPixel() == 4u);

        REQUIRE(srgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(srgb32bpp->GetBytesPerPixel() == 4u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(linear24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(linear32bpp->GetBitsPerChannel() == 8u);

        REQUIRE(srgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(srgb32bpp->GetBitsPerChannel() == 8u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(linear24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(linear32bpp->GetBytesPerChannel() == 1u);

        REQUIRE(srgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(srgb32bpp->GetBytesPerChannel() == 1u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(linear24bpp->GetWidth() == 32u);
        REQUIRE(linear32bpp->GetWidth() == 32u);

        REQUIRE(srgb24bpp->GetWidth() == 32u);
        REQUIRE(srgb32bpp->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(linear24bpp->GetHeight() == 32u);
        REQUIRE(linear32bpp->GetHeight() == 32u);

        REQUIRE(srgb24bpp->GetHeight() == 32u);
        REQUIRE(srgb32bpp->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(linear24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(linear32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(srgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(srgb32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(linear24bpp->HasAlphaChannel());
        REQUIRE(linear32bpp->HasAlphaChannel());

        REQUIRE_FALSE(srgb24bpp->HasAlphaChannel());
        REQUIRE(srgb32bpp->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE_FALSE(linear24bpp->IsImageGrayScale());
        REQUIRE_FALSE(linear32bpp->IsImageGrayScale());

        REQUIRE_FALSE(srgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(srgb32bpp->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE(linear24bpp->IsImageColored());
        REQUIRE(linear32bpp->IsImageColored());

        REQUIRE(srgb24bpp->IsImageColored());
        REQUIRE(srgb32bpp->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(linear24bpp->IsHDR());
        REQUIRE_FALSE(linear32bpp->IsHDR());

        REQUIRE_FALSE(srgb24bpp->IsHDR());
        REQUIRE_FALSE(srgb32bpp->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(linear24bpp->IsLDR());
        REQUIRE(linear32bpp->IsLDR());

        REQUIRE(srgb24bpp->IsLDR());
        REQUIRE(srgb32bpp->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(linear24bpp->GetFilePath() == "Testfiles/ImageLoader/QOI/Test24BPPLinear.qoi");
        REQUIRE(linear32bpp->GetFilePath() == "Testfiles/ImageLoader/QOI/Test32BPPLinear.qoi");

        REQUIRE(srgb24bpp->GetFilePath() == "Testfiles/ImageLoader/QOI/Test24BPPsRGB.qoi");
        REQUIRE(srgb32bpp->GetFilePath() == "Testfiles/ImageLoader/QOI/Test32BPPsRGB.qoi");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(linear24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(linear32bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(srgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(srgb32bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/QOI/Test24BPPLinear.qoi"));
    }
}
