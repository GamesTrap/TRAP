#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::PGMImage", "[imageloader][pgmimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PGM/Test8BPPGrayscale.pgm");
    const TRAP::Scope<TRAP::Image> grayscale16bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PGM/Test16BPPGrayscale.pgm");

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscale8bpp);
        REQUIRE(grayscale16bpp);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale8bpp->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale16bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale16bpp->GetPixelData(), TestImageSDRPixelDataGrayScale));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale16bpp->GetChannelsPerPixel() == 1u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale16bpp->GetBitsPerPixel() == 16u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale16bpp->GetBytesPerPixel() == 2u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale16bpp->GetBitsPerChannel() == 16u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale16bpp->GetBytesPerChannel() == 2u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8bpp->GetWidth() == 32u);
        REQUIRE(grayscale16bpp->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8bpp->GetHeight() == 32u);
        REQUIRE(grayscale16bpp->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8bpp->HasAlphaChannel());
        REQUIRE_FALSE(grayscale16bpp->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8bpp->IsImageGrayScale());
        REQUIRE(grayscale16bpp->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsImageColored());
        REQUIRE_FALSE(grayscale16bpp->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsHDR());
        REQUIRE_FALSE(grayscale16bpp->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8bpp->IsLDR());
        REQUIRE(grayscale16bpp->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8bpp->GetFilePath() == "Testfiles/ImageLoader/PM/PGM/Test8BPPGrayscale.pgm");
        REQUIRE(grayscale16bpp->GetFilePath() == "Testfiles/ImageLoader/PM/PGM/Test16BPPGrayscale.pgm");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale16bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/PM/PGM/Test8BPPGrayscale.pgm"));
    }
}
