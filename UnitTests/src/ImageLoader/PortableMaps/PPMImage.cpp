#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/PortableMaps/PPMImage.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::PPMImage", "[imageloader][ppmimage]")
{
    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PPM/Test24BPP.ppm");
    const TRAP::Scope<TRAP::Image> rgb48bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PPM/Test48BPP.ppm");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::INTERNAL::PPMImage>);
        STATIC_REQUIRE(std::copyable<TRAP::INTERNAL::PPMImage>);
        STATIC_REQUIRE(std::movable<TRAP::INTERNAL::PPMImage>);
        STATIC_REQUIRE(std::derived_from<TRAP::INTERNAL::PPMImage, TRAP::Image>);
    }

    SECTION("LoadFromFile()")
    {
        REQUIRE(rgb24bpp);
        REQUIRE(rgb48bpp);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb48bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb48bpp->GetPixelData(), TestImageSDR16PixelDataRGB));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb48bpp->GetChannelsPerPixel() == 3u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb48bpp->GetBitsPerPixel() == 48u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb48bpp->GetBytesPerPixel() == 6u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb48bpp->GetBitsPerChannel() == 16u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb48bpp->GetBytesPerChannel() == 2u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgb48bpp->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgb48bpp->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb48bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb48bpp->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb48bpp->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgb48bpp->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgb48bpp->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgb48bpp->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(rgb24bpp->GetFilePath() == "Testfiles/ImageLoader/PM/PPM/Test24BPP.ppm");
        REQUIRE(rgb48bpp->GetFilePath() == "Testfiles/ImageLoader/PM/PPM/Test48BPP.ppm");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb48bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/PM/PPM/Test24BPP.ppm"));
    }
}
