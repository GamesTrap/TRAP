#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/TARGA/TGAImage.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::TGAImage", "[imageloader][tgaimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/TestGrayscale.tga");

    const TRAP::Scope<TRAP::Image> palette8bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/TestPalette.tga");

    const TRAP::Scope<TRAP::Image> rgb16bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test16BPP.tga");
    const TRAP::Scope<TRAP::Image> rgb16bpprle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test16BPPRLE.tga");

    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test24BPP.tga");
    const TRAP::Scope<TRAP::Image> rgb24bpprle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test24BPPRLE.tga");

    const TRAP::Scope<TRAP::Image> rgba32bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test32BPP.tga");
    const TRAP::Scope<TRAP::Image> rgba32bpprle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/TGA/Test32BPPRLE.tga");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::INTERNAL::TGAImage>);
        STATIC_REQUIRE(std::copyable<TRAP::INTERNAL::TGAImage>);
        STATIC_REQUIRE(std::movable<TRAP::INTERNAL::TGAImage>);
        STATIC_REQUIRE(std::derived_from<TRAP::INTERNAL::TGAImage, TRAP::Image>);
    }

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscale8bpp);

        REQUIRE(palette8bpp);

        REQUIRE(rgb16bpp);
        REQUIRE(rgb16bpprle);

        REQUIRE(rgb24bpp);
        REQUIRE(rgb24bpprle);

        REQUIRE(rgba32bpp);
        REQUIRE(rgba32bpprle);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale8bpp->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(palette8bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(palette8bpp->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb16bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb16bpprle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bpprle->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb24bpprle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb24bpprle->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgba32bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgba32bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgba32bpprle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb32abpprle->GetPixelData(), TestImageSDRPixelDataRGBA));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetChannelsPerPixel() == 1u);

        REQUIRE(palette8bpp->GetChannelsPerPixel() == 3u);

        REQUIRE(rgb16bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb16bpprle->GetChannelsPerPixel() == 3u);

        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb24bpprle->GetChannelsPerPixel() == 3u);

        REQUIRE(rgba32bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba32bpprle->GetChannelsPerPixel() == 4u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerPixel() == 8u);

        REQUIRE(palette8bpp->GetBitsPerPixel() == 24u);

        REQUIRE(rgb16bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb16bpprle->GetBitsPerPixel() == 24u);

        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb24bpprle->GetBitsPerPixel() == 24u);

        REQUIRE(rgba32bpp->GetBitsPerPixel() == 32u);
        REQUIRE(rgba32bpprle->GetBitsPerPixel() == 32u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerPixel() == 1u);

        REQUIRE(palette8bpp->GetBytesPerPixel() == 3u);

        REQUIRE(rgb16bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb16bpprle->GetBytesPerPixel() == 3u);

        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb24bpprle->GetBytesPerPixel() == 3u);

        REQUIRE(rgba32bpp->GetBytesPerPixel() == 4u);
        REQUIRE(rgba32bpprle->GetBytesPerPixel() == 4u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerChannel() == 8u);

        REQUIRE(palette8bpp->GetBitsPerChannel() == 8u);

        REQUIRE(rgb16bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb16bpprle->GetBitsPerChannel() == 8u);

        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb24bpprle->GetBitsPerChannel() == 8u);

        REQUIRE(rgba32bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgba32bpprle->GetBitsPerChannel() == 8u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerChannel() == 1u);

        REQUIRE(palette8bpp->GetBytesPerChannel() == 1u);

        REQUIRE(rgb16bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb16bpprle->GetBytesPerChannel() == 1u);

        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb24bpprle->GetBytesPerChannel() == 1u);

        REQUIRE(rgba32bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgba32bpprle->GetBytesPerChannel() == 1u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8bpp->GetWidth() == 32u);

        REQUIRE(palette8bpp->GetWidth() == 32u);

        REQUIRE(rgb16bpp->GetWidth() == 32u);
        REQUIRE(rgb16bpprle->GetWidth() == 32u);

        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgb24bpprle->GetWidth() == 32u);

        REQUIRE(rgba32bpp->GetWidth() == 32u);
        REQUIRE(rgba32bpprle->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8bpp->GetHeight() == 32u);

        REQUIRE(palette8bpp->GetHeight() == 32u);

        REQUIRE(rgb16bpp->GetHeight() == 32u);
        REQUIRE(rgb16bpprle->GetHeight() == 32u);

        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgb24bpprle->GetHeight() == 32u);

        REQUIRE(rgba32bpp->GetHeight() == 32u);
        REQUIRE(rgba32bpprle->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(palette8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb16bpprle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb24bpprle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgba32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba32bpprle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8bpp->HasAlphaChannel());

        REQUIRE_FALSE(palette8bpp->HasAlphaChannel());

        REQUIRE_FALSE(rgb16bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb16bpprle->HasAlphaChannel());

        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb24bpprle->HasAlphaChannel());

        REQUIRE(rgba32bpp->HasAlphaChannel());
        REQUIRE(rgba32bpprle->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8bpp->IsImageGrayScale());

        REQUIRE_FALSE(palette8bpp->IsImageGrayScale());

        REQUIRE_FALSE(rgb16bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb16bpprle->IsImageGrayScale());

        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb24bpprle->IsImageGrayScale());

        REQUIRE_FALSE(rgba32bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgba32bpprle->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsImageColored());

        REQUIRE(palette8bpp->IsImageColored());

        REQUIRE(rgb16bpp->IsImageColored());
        REQUIRE(rgb16bpprle->IsImageColored());

        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgb24bpprle->IsImageColored());

        REQUIRE(rgba32bpp->IsImageColored());
        REQUIRE(rgba32bpprle->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsHDR());

        REQUIRE_FALSE(palette8bpp->IsHDR());

        REQUIRE_FALSE(rgb16bpp->IsHDR());
        REQUIRE_FALSE(rgb16bpprle->IsHDR());

        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgb24bpprle->IsHDR());

        REQUIRE_FALSE(rgba32bpp->IsHDR());
        REQUIRE_FALSE(rgba32bpprle->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8bpp->IsLDR());

        REQUIRE(palette8bpp->IsLDR());

        REQUIRE(rgb16bpp->IsLDR());
        REQUIRE(rgb16bpprle->IsLDR());

        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgb24bpprle->IsLDR());

        REQUIRE(rgba32bpp->IsLDR());
        REQUIRE(rgba32bpprle->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8bpp->GetFilePath() == "Testfiles/ImageLoader/TGA/TestGrayscale.tga");

        REQUIRE(palette8bpp->GetFilePath() == "Testfiles/ImageLoader/TGA/TestPalette.tga");

        REQUIRE(rgb16bpp->GetFilePath() == "Testfiles/ImageLoader/TGA/Test16BPP.tga");
        REQUIRE(rgb16bpprle->GetFilePath() == "Testfiles/ImageLoader/TGA/Test16BPPRLE.tga");

        REQUIRE(rgb24bpp->GetFilePath() == "Testfiles/ImageLoader/TGA/Test24BPP.tga");
        REQUIRE(rgb24bpprle->GetFilePath() == "Testfiles/ImageLoader/TGA/Test24BPPRLE.tga");

        REQUIRE(rgba32bpp->GetFilePath() == "Testfiles/ImageLoader/TGA/Test32BPP.tga");
        REQUIRE(rgba32bpprle->GetFilePath() == "Testfiles/ImageLoader/TGA/Test32BPPRLE.tga");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);

        REQUIRE(palette8bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);

        REQUIRE(rgb16bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb16bpprle->GetColorFormat() == TRAP::Image::ColorFormat::RGB);

        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb24bpprle->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/TGA/TestGrayscale.tga"));
    }
}
