#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::PFMImage", "[imageloader][pfmimage]")
{
    const TRAP::Scope<TRAP::Image> grayscaleHDR = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PFM/TestGrayscaleHDR.pfm");
    const TRAP::Scope<TRAP::Image> rgbHDR = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PFM/TestHDR.pfm");

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscaleHDR);
        REQUIRE(rgbHDR);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscaleHDR->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscaleHDR->GetPixelData(), TestImageHDRPixelDataGrayScale));
        REQUIRE_FALSE(rgbHDR->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgbHDR->GetPixelData(), TestImageHDRPixelDataRGB));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscaleHDR->GetChannelsPerPixel() == 1u);
        REQUIRE(rgbHDR->GetChannelsPerPixel() == 3u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscaleHDR->GetBitsPerPixel() == 32u);
        REQUIRE(rgbHDR->GetBitsPerPixel() == 96u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscaleHDR->GetBytesPerPixel() == 4u);
        REQUIRE(rgbHDR->GetBytesPerPixel() == 12u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscaleHDR->GetBitsPerChannel() == 32u);
        REQUIRE(rgbHDR->GetBitsPerChannel() == 32u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscaleHDR->GetBytesPerChannel() == 4u);
        REQUIRE(rgbHDR->GetBytesPerChannel() == 4u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscaleHDR->GetWidth() == 32u);
        REQUIRE(rgbHDR->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscaleHDR->GetHeight() == 32u);
        REQUIRE(rgbHDR->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscaleHDR->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgbHDR->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscaleHDR->HasAlphaChannel());
        REQUIRE_FALSE(rgbHDR->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscaleHDR->IsImageGrayScale());
        REQUIRE_FALSE(rgbHDR->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscaleHDR->IsImageColored());
        REQUIRE(rgbHDR->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE(grayscaleHDR->IsHDR());
        REQUIRE(rgbHDR->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE_FALSE(grayscaleHDR->IsLDR());
        REQUIRE_FALSE(rgbHDR->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscaleHDR->GetFilePath() == "Testfiles/ImageLoader/PM/PFM/TestGrayscaleHDR.pfm");
        REQUIRE(rgbHDR->GetFilePath() == "Testfiles/ImageLoader/PM/PFM/TestHDR.pfm");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscaleHDR->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(rgbHDR->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/PM/PFM/TestHDR.pfm"));
    }
}
