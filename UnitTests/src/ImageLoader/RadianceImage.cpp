#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::RadianceImage", "[imageloader][radianceimage]")
{
    const TRAP::Scope<TRAP::Image> hdrimg = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/Radiance/TestHDR.hdr");

    SECTION("LoadFromFile()")
    {
        REQUIRE(hdrimg);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(hdrimg->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(hdrimg->GetPixelData(), TestImageHDRPixelDataRGB));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(hdrimg->GetChannelsPerPixel() == 3u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(hdrimg->GetBitsPerPixel() == 96u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(hdrimg->GetBytesPerPixel() == 12u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(hdrimg->GetBitsPerChannel() == 32u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(hdrimg->GetBytesPerChannel() == 4u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(hdrimg->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(hdrimg->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(hdrimg->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(hdrimg->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE_FALSE(hdrimg->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE(hdrimg->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE(hdrimg->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE_FALSE(hdrimg->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(hdrimg->GetFilePath() == "Testfiles/ImageLoader/Radiance/TestHDR.hdr");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(hdrimg->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/Radiance/TestHDR.hdr"));
    }
}
