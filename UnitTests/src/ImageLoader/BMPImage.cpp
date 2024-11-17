#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::BMPImage", "[imageloader][bmpimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscale.bmp");
    const TRAP::Scope<TRAP::Image> grayscale8bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleReversed.bmp");
    const TRAP::Scope<TRAP::Image> grayscale8bpprle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLE.bmp");
    const TRAP::Scope<TRAP::Image> grayscale8bpprlereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLEReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgba8bpppalette = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPalette.bmp");
    const TRAP::Scope<TRAP::Image> rgba8bpppalettereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgba8bpppaletterle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteRLE.bmp");
    const TRAP::Scope<TRAP::Image> rgba8bpppaletterlereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteRLEReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgb16bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgb16bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPPReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgba16bppbitfields = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPPBitFields.bmp");

    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test24BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgb24bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test24BPPReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgba32bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgba32bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPPReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgba32bppbitfields = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPPBitFields.bmp");

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscale8bpp);
        REQUIRE(grayscale8bppreversed);
        REQUIRE(grayscale8bpprle);
        REQUIRE(grayscale8bpprlereversed);

        REQUIRE(rgba8bpppalette);
        REQUIRE(rgba8bpppalettereversed);
        REQUIRE(rgba8bpppaletterle);
        REQUIRE(rgba8bpppaletterlereversed);

        REQUIRE(rgb16bpp);
        REQUIRE(rgb16bppreversed);
        REQUIRE(rgba16bppbitfields);

        REQUIRE(rgb24bpp);
        REQUIRE(rgb24bppreversed);

        REQUIRE(rgba32bpp);
        REQUIRE(rgba32bppreversed);
        REQUIRE(rgba32bppbitfields);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscale->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale8bppreversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale8bpprle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalerle->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale8bpprlereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalerlereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(rgba8bpppalette->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppalette->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgba8bpppalettereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppalettereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgba8bpppaletterle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppaletterle->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgba8bpppaletterlereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppaletterlereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(rgb16bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bpp->GetPixelData(), TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgb16bppreversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppreversed->GetPixelData(), TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgba16bppbitfields->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppbitfields->GetPixelData(), TestImageSDR16PixelDataRGBA));

        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb24bppreversed->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bppreversed->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgba32bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgba32bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgba32bppreversed->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgba32bppreversed->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgba32bppbitfields->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgba32bppbitfields->GetPixelData(), TestImageSDRPixelDataRGBA));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale8bppreversed->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale8bpprle->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale8bpprlereversed->GetChannelsPerPixel() == 1u);

        REQUIRE(rgba8bpppalette->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba8bpppalettereversed->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba8bpppaletterle->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba8bpppaletterlereversed->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb16bppreversed->GetChannelsPerPixel() == 3u);
        REQUIRE(rgba16bppbitfields->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb24bppreversed->GetChannelsPerPixel() == 3u);

        REQUIRE(rgba32bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba32bppreversed->GetChannelsPerPixel() == 4u);
        REQUIRE(rgba32bppbitfields->GetChannelsPerPixel() == 4u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale8bppreversed->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale8bpprle->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale8bpprlereversed->GetBitsPerPixel() == 8u);

        REQUIRE(rgba8bpppalette->GetBitsPerPixel() == 32u);
        REQUIRE(rgba8bpppalettereversed->GetBitsPerPixel() == 32u);
        REQUIRE(rgba8bpppaletterle->GetBitsPerPixel() == 32u);
        REQUIRE(rgba8bpppaletterlereversed->GetBitsPerPixel() == 32u);

        REQUIRE(rgb16bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb16bppreversed->GetBitsPerPixel() == 24u);
        REQUIRE(rgba16bppbitfields->GetBitsPerPixel() == 32u);

        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb24bppreversed->GetBitsPerPixel() == 24u);

        REQUIRE(rgba32bpp->GetBitsPerPixel() == 32u);
        REQUIRE(rgba32bppreversed->GetBitsPerPixel() == 32u);
        REQUIRE(rgba32bppbitfields->GetBitsPerPixel() == 32u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale8bppreversed->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale8bpprle->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale8bpprlereversed->GetBytesPerPixel() == 1u);

        REQUIRE(rgba8bpppalette->GetBytesPerPixel() == 4u);
        REQUIRE(rgba8bpppalettereversed->GetBytesPerPixel() == 4u);
        REQUIRE(rgba8bpppaletterle->GetBytesPerPixel() == 4u);
        REQUIRE(rgba8bpppaletterlereversed->GetBytesPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb16bppreversed->GetBytesPerPixel() == 3u);
        REQUIRE(rgba16bppbitfields->GetBytesPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb24bppreversed->GetBytesPerPixel() == 3u);

        REQUIRE(rgba32bpp->GetBytesPerPixel() == 4u);
        REQUIRE(rgba32bppreversed->GetBytesPerPixel() == 4u);
        REQUIRE(rgba32bppbitfields->GetBytesPerPixel() == 4u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale8bppreversed->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale8bpprle->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale8bpprlereversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgba8bpppalette->GetBitsPerChannel() == 8u);
        REQUIRE(rgba8bpppalettereversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgba8bpppaletterle->GetBitsPerChannel() == 8u);
        REQUIRE(rgba8bpppaletterlereversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgb16bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb16bppreversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgba16bppbitfields->GetBitsPerChannel() == 8u);

        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb24bppreversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgba32bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgba32bppreversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgba32bppbitfields->GetBitsPerChannel() == 8u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale8bppreversed->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale8bpprle->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale8bpprlereversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgba8bpppalette->GetBytesPerChannel() == 1u);
        REQUIRE(rgba8bpppalettereversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgba8bpppaletterle->GetBytesPerChannel() == 1u);
        REQUIRE(rgba8bpppaletterlereversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgb16bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb16bppreversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgba16bppbitfields->GetBytesPerChannel() == 1u);

        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb24bppreversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgba32bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgba32bppreversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgba32bppbitfields->GetBytesPerChannel() == 1u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8bpp->GetWidth() == 32u);
        REQUIRE(grayscale8bppreversed->GetWidth() == 32u);
        REQUIRE(grayscale8bpprle->GetWidth() == 32u);
        REQUIRE(grayscale8bpprlereversed->GetWidth() == 32u);

        REQUIRE(rgba8bpppalette->GetWidth() == 32u);
        REQUIRE(rgba8bpppalettereversed->GetWidth() == 32u);
        REQUIRE(rgba8bpppaletterle->GetWidth() == 32u);
        REQUIRE(rgba8bpppaletterlereversed->GetWidth() == 32u);

        REQUIRE(rgb16bpp->GetWidth() == 32u);
        REQUIRE(rgb16bppreversed->GetWidth() == 32u);
        REQUIRE(rgba16bppbitfields->GetWidth() == 32u);

        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgb24bppreversed->GetWidth() == 32u);

        REQUIRE(rgba32bpp->GetWidth() == 32u);
        REQUIRE(rgba32bppreversed->GetWidth() == 32u);
        REQUIRE(rgba32bppbitfields->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8bpp->GetHeight() == 32u);
        REQUIRE(grayscale8bppreversed->GetHeight() == 32u);
        REQUIRE(grayscale8bpprle->GetHeight() == 32u);
        REQUIRE(grayscale8bpprlereversed->GetHeight() == 32u);

        REQUIRE(rgba8bpppalette->GetHeight() == 32u);
        REQUIRE(rgba8bpppalettereversed->GetHeight() == 32u);
        REQUIRE(rgba8bpppaletterle->GetHeight() == 32u);
        REQUIRE(rgba8bpppaletterlereversed->GetHeight() == 32u);

        REQUIRE(rgb16bpp->GetHeight() == 32u);
        REQUIRE(rgb16bppreversed->GetHeight() == 32u);
        REQUIRE(rgba16bppbitfields->GetHeight() == 32u);

        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgb24bppreversed->GetHeight() == 32u);

        REQUIRE(rgba32bpp->GetHeight() == 32u);
        REQUIRE(rgba32bppreversed->GetHeight() == 32u);
        REQUIRE(rgba32bppbitfields->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale8bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale8bpprle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale8bpprlereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgba8bpppalette->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba8bpppalettereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba8bpppaletterle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba8bpppaletterlereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb16bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba16bppbitfields->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb24bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgba32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba32bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgba32bppbitfields->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8bpp->HasAlphaChannel());
        REQUIRE_FALSE(grayscale8bppreversed->HasAlphaChannel());
        REQUIRE_FALSE(grayscale8bpprle->HasAlphaChannel());
        REQUIRE_FALSE(grayscale8bpprlereversed->HasAlphaChannel());

        REQUIRE(rgba8bpppalette->HasAlphaChannel());
        REQUIRE(rgba8bpppalettereversed->HasAlphaChannel());
        REQUIRE(rgba8bpppaletterle->HasAlphaChannel());
        REQUIRE(rgba8bpppaletterlereversed->HasAlphaChannel());

        REQUIRE_FALSE(rgb16bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb16bppreversed->HasAlphaChannel());
        REQUIRE(rgba16bppbitfields->HasAlphaChannel());

        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb24bppreversed->HasAlphaChannel());

        REQUIRE(rgba32bpp->HasAlphaChannel());
        REQUIRE(rgba32bppreversed->HasAlphaChannel());
        REQUIRE(rgba32bppbitfields->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8bpp->IsImageGrayScale());
        REQUIRE(grayscale8bppreversed->IsImageGrayScale());
        REQUIRE(grayscale8bpprle->IsImageGrayScale());
        REQUIRE(grayscale8bpprlereversed->IsImageGrayScale());

        REQUIRE_FALSE(rgba8bpppalette->IsImageGrayScale());
        REQUIRE_FALSE(rgba8bpppalettereversed->IsImageGrayScale());
        REQUIRE_FALSE(rgba8bpppaletterle->IsImageGrayScale());
        REQUIRE_FALSE(rgba8bpppaletterlereversed->IsImageGrayScale());

        REQUIRE_FALSE(rgb16bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb16bppreversed->IsImageGrayScale());
        REQUIRE_FALSE(rgba16bppbitfields->IsImageGrayScale());

        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb24bppreversed->IsImageGrayScale());

        REQUIRE_FALSE(rgba32bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgba32bppreversed->IsImageGrayScale());
        REQUIRE_FALSE(rgba32bppbitfields->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsImageColored());
        REQUIRE_FALSE(grayscale8bppreversed->IsImageColored());
        REQUIRE_FALSE(grayscale8bpprle->IsImageColored());
        REQUIRE_FALSE(grayscale8bpprlereversed->IsImageColored());

        REQUIRE(rgba8bpppalette->IsImageColored());
        REQUIRE(rgba8bpppalettereversed->IsImageColored());
        REQUIRE(rgba8bpppaletterle->IsImageColored());
        REQUIRE(rgba8bpppaletterlereversed->IsImageColored());

        REQUIRE(rgb16bpp->IsImageColored());
        REQUIRE(rgb16bppreversed->IsImageColored());
        REQUIRE(rgba16bppbitfields->IsImageColored());

        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgb24bppreversed->IsImageColored());

        REQUIRE(rgba32bpp->IsImageColored());
        REQUIRE(rgba32bppreversed->IsImageColored());
        REQUIRE(rgba32bppbitfields->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsHDR());
        REQUIRE_FALSE(grayscale8bppreversed->IsHDR());
        REQUIRE_FALSE(grayscale8bpprle->IsHDR());
        REQUIRE_FALSE(grayscale8bpprlereversed->IsHDR());

        REQUIRE_FALSE(rgba8bpppalette->IsHDR());
        REQUIRE_FALSE(rgba8bpppalettereversed->IsHDR());
        REQUIRE_FALSE(rgba8bpppaletterle->IsHDR());
        REQUIRE_FALSE(rgba8bpppaletterlereversed->IsHDR());

        REQUIRE_FALSE(rgb16bpp->IsHDR());
        REQUIRE_FALSE(rgb16bppreversed->IsHDR());
        REQUIRE_FALSE(rgba16bppbitfields->IsHDR());

        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgb24bppreversed->IsHDR());

        REQUIRE_FALSE(rgba32bpp->IsHDR());
        REQUIRE_FALSE(rgba32bppreversed->IsHDR());
        REQUIRE_FALSE(rgba32bppbitfields->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8bpp->IsLDR());
        REQUIRE(grayscale8bppreversed->IsLDR());
        REQUIRE(grayscale8bpprle->IsLDR());
        REQUIRE(grayscale8bpprlereversed->IsLDR());

        REQUIRE(rgba8bpppalette->IsLDR());
        REQUIRE(rgba8bpppalettereversed->IsLDR());
        REQUIRE(rgba8bpppaletterle->IsLDR());
        REQUIRE(rgba8bpppaletterlereversed->IsLDR());

        REQUIRE(rgb16bpp->IsLDR());
        REQUIRE(rgb16bppreversed->IsLDR());
        REQUIRE(rgba16bppbitfields->IsLDR());

        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgb24bppreversed->IsLDR());

        REQUIRE(rgba32bpp->IsLDR());
        REQUIRE(rgba32bppreversed->IsLDR());
        REQUIRE(rgba32bppbitfields->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscale.bmp");
        REQUIRE(grayscale8bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleReversed.bmp");
        REQUIRE(grayscale8bpprle->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLE.bmp");
        REQUIRE(grayscale8bpprlereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLEReversed.bmp");

        REQUIRE(rgba8bpppalette->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPalette.bmp");
        REQUIRE(rgba8bpppalettereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteReversed.bmp");
        REQUIRE(rgba8bpppaletterle->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteRLE.bmp");
        REQUIRE(rgba8bpppaletterlereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteRLEReversed.bmp");

        REQUIRE(rgb16bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPP.bmp");
        REQUIRE(rgb16bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPPReversed.bmp");
        REQUIRE(rgba16bppbitfields->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPPBitFields.bmp");

        REQUIRE(rgb24bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test24BPP.bmp");
        REQUIRE(rgb24bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test24BPPReversed.bmp");

        REQUIRE(rgba32bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPP.bmp");
        REQUIRE(rgba32bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPPReversed.bmp");
        REQUIRE(rgba32bppbitfields->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPPBitFields.bmp");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale8bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale8bpprle->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale8bpprlereversed->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);

        REQUIRE(rgba8bpppalette->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgba8bpppalettereversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgba8bpppaletterle->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgba8bpppaletterlereversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb16bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb16bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgba16bppbitfields->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb24bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGB);

        REQUIRE(rgba32bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgba32bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgba32bppbitfields->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/BMP/Test24BPP.bmp"));
    }
}
