#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::BMPImage", "[imageloader][bmpimage]")
{
    const TRAP::Scope<TRAP::Image> rgb8bppgrayscale = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscale.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bppgrayscalereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bppgrayscalerle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLE.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bppgrayscalerlereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLEReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgb8bpppalette = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPalette.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bpppalettereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bpppaletterle = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteRLE.bmp");
    const TRAP::Scope<TRAP::Image> rgb8bpppaletterlereversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test8BPPPaletteRLEReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgb16bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgb16bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPPReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgb16bppbitfields = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test16BPPBitFields.bmp");

    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test24BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgb24bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test24BPPReversed.bmp");

    const TRAP::Scope<TRAP::Image> rgb32bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPP.bmp");
    const TRAP::Scope<TRAP::Image> rgb32bppreversed = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPPReversed.bmp");
    const TRAP::Scope<TRAP::Image> rgb32bppbitfields = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/BMP/Test32BPPBitFields.bmp");

    SECTION("LoadFromFile()")
    {
        REQUIRE(rgb8bppgrayscale);
        REQUIRE(rgb8bppgrayscalereversed);
        REQUIRE(rgb8bppgrayscalerle);
        REQUIRE(rgb8bppgrayscalerlereversed);

        REQUIRE(rgb8bpppalette);
        REQUIRE(rgb8bpppalettereversed);
        REQUIRE(rgb8bpppaletterle);
        REQUIRE(rgb8bpppaletterlereversed);

        REQUIRE(rgb16bpp);
        REQUIRE(rgb16bppreversed);
        REQUIRE(rgb16bppbitfields);

        REQUIRE(rgb24bpp);
        REQUIRE(rgb24bppreversed);

        REQUIRE(rgb32bpp);
        REQUIRE(rgb32bppreversed);
        REQUIRE(rgb32bppbitfields);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(rgb8bppgrayscale->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscale->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bppgrayscalereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bppgrayscalerle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalerle->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bppgrayscalerlereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bppgrayscalerlereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(rgb8bpppalette->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppalette->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bpppalettereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppalettereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bpppaletterle->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppaletterle->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(rgb8bpppaletterlereversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppaletterlereversed->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(rgb16bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bpp->GetPixelData(), TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgb16bppreversed->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppreversed->GetPixelData(), TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgb16bppbitfields->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppbitfields->GetPixelData(), TestImageSDR16PixelDataRGBA));

        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb24bppreversed->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bppreversed->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb32bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb32bpp->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgb32bppreversed->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb32bppreversed->GetPixelData(), TestImageSDRPixelDataRGBA));
        REQUIRE_FALSE(rgb32bppbitfields->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb32bppbitfields->GetPixelData(), TestImageSDRPixelDataRGBA));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(rgb8bppgrayscale->GetChannelsPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalereversed->GetChannelsPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalerle->GetChannelsPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetChannelsPerPixel() == 1u);

        REQUIRE(rgb8bpppalette->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb8bpppalettereversed->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb8bpppaletterle->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb8bpppaletterlereversed->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb16bppreversed->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb16bppbitfields->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb24bppreversed->GetChannelsPerPixel() == 3u);

        REQUIRE(rgb32bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb32bppreversed->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb32bppbitfields->GetChannelsPerPixel() == 4u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(rgb8bppgrayscale->GetBitsPerPixel() == 8u);
        REQUIRE(rgb8bppgrayscalereversed->GetBitsPerPixel() == 8u);
        REQUIRE(rgb8bppgrayscalerle->GetBitsPerPixel() == 8u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetBitsPerPixel() == 8u);

        REQUIRE(rgb8bpppalette->GetBitsPerPixel() == 32u);
        REQUIRE(rgb8bpppalettereversed->GetBitsPerPixel() == 32u);
        REQUIRE(rgb8bpppaletterle->GetBitsPerPixel() == 32u);
        REQUIRE(rgb8bpppaletterlereversed->GetBitsPerPixel() == 32u);

        REQUIRE(rgb16bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb16bppreversed->GetBitsPerPixel() == 24u);
        REQUIRE(rgb16bppbitfields->GetBitsPerPixel() == 32u);

        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb24bppreversed->GetBitsPerPixel() == 24u);

        REQUIRE(rgb32bpp->GetBitsPerPixel() == 32u);
        REQUIRE(rgb32bppreversed->GetBitsPerPixel() == 32u);
        REQUIRE(rgb32bppbitfields->GetBitsPerPixel() == 32u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(rgb8bppgrayscale->GetBytesPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalereversed->GetBytesPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalerle->GetBytesPerPixel() == 1u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetBytesPerPixel() == 1u);

        REQUIRE(rgb8bpppalette->GetBytesPerPixel() == 4u);
        REQUIRE(rgb8bpppalettereversed->GetBytesPerPixel() == 4u);
        REQUIRE(rgb8bpppaletterle->GetBytesPerPixel() == 4u);
        REQUIRE(rgb8bpppaletterlereversed->GetBytesPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb16bppreversed->GetBytesPerPixel() == 3u);
        REQUIRE(rgb16bppbitfields->GetBytesPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb24bppreversed->GetBytesPerPixel() == 3u);

        REQUIRE(rgb32bpp->GetBytesPerPixel() == 4u);
        REQUIRE(rgb32bppreversed->GetBytesPerPixel() == 4u);
        REQUIRE(rgb32bppbitfields->GetBytesPerPixel() == 4u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(rgb8bppgrayscale->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bppgrayscalereversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bppgrayscalerle->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgb8bpppalette->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bpppalettereversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bpppaletterle->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bpppaletterlereversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgb16bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb16bppreversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgb16bppbitfields->GetBitsPerChannel() == 8u);

        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb24bppreversed->GetBitsPerChannel() == 8u);

        REQUIRE(rgb32bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb32bppreversed->GetBitsPerChannel() == 8u);
        REQUIRE(rgb32bppbitfields->GetBitsPerChannel() == 8u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(rgb8bppgrayscale->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bppgrayscalereversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bppgrayscalerle->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgb8bpppalette->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bpppalettereversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bpppaletterle->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bpppaletterlereversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgb16bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb16bppreversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgb16bppbitfields->GetBytesPerChannel() == 1u);

        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb24bppreversed->GetBytesPerChannel() == 1u);

        REQUIRE(rgb32bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb32bppreversed->GetBytesPerChannel() == 1u);
        REQUIRE(rgb32bppbitfields->GetBytesPerChannel() == 1u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(rgb8bppgrayscale->GetWidth() == 32u);
        REQUIRE(rgb8bppgrayscalereversed->GetWidth() == 32u);
        REQUIRE(rgb8bppgrayscalerle->GetWidth() == 32u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetWidth() == 32u);

        REQUIRE(rgb8bpppalette->GetWidth() == 32u);
        REQUIRE(rgb8bpppalettereversed->GetWidth() == 32u);
        REQUIRE(rgb8bpppaletterle->GetWidth() == 32u);
        REQUIRE(rgb8bpppaletterlereversed->GetWidth() == 32u);

        REQUIRE(rgb16bpp->GetWidth() == 32u);
        REQUIRE(rgb16bppreversed->GetWidth() == 32u);
        REQUIRE(rgb16bppbitfields->GetWidth() == 32u);

        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgb24bppreversed->GetWidth() == 32u);

        REQUIRE(rgb32bpp->GetWidth() == 32u);
        REQUIRE(rgb32bppreversed->GetWidth() == 32u);
        REQUIRE(rgb32bppbitfields->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(rgb8bppgrayscale->GetHeight() == 32u);
        REQUIRE(rgb8bppgrayscalereversed->GetHeight() == 32u);
        REQUIRE(rgb8bppgrayscalerle->GetHeight() == 32u);
        REQUIRE(rgb8bppgrayscalerlereversed->GetHeight() == 32u);

        REQUIRE(rgb8bpppalette->GetHeight() == 32u);
        REQUIRE(rgb8bpppalettereversed->GetHeight() == 32u);
        REQUIRE(rgb8bpppaletterle->GetHeight() == 32u);
        REQUIRE(rgb8bpppaletterlereversed->GetHeight() == 32u);

        REQUIRE(rgb16bpp->GetHeight() == 32u);
        REQUIRE(rgb16bppreversed->GetHeight() == 32u);
        REQUIRE(rgb16bppbitfields->GetHeight() == 32u);

        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgb24bppreversed->GetHeight() == 32u);

        REQUIRE(rgb32bpp->GetHeight() == 32u);
        REQUIRE(rgb32bppreversed->GetHeight() == 32u);
        REQUIRE(rgb32bppbitfields->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(rgb8bppgrayscale->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bppgrayscalereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bppgrayscalerle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bppgrayscalerlereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb8bpppalette->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bpppalettereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bpppaletterle->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bpppaletterlereversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb16bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb16bppbitfields->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb24bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb32bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb32bppreversed->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb32bppbitfields->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(rgb8bppgrayscale->HasAlphaChannel());
        REQUIRE_FALSE(rgb8bppgrayscalereversed->HasAlphaChannel());
        REQUIRE_FALSE(rgb8bppgrayscalerle->HasAlphaChannel());
        REQUIRE_FALSE(rgb8bppgrayscalerlereversed->HasAlphaChannel());

        REQUIRE(rgb8bpppalette->HasAlphaChannel());
        REQUIRE(rgb8bpppalettereversed->HasAlphaChannel());
        REQUIRE(rgb8bpppaletterle->HasAlphaChannel());
        REQUIRE(rgb8bpppaletterlereversed->HasAlphaChannel());

        REQUIRE_FALSE(rgb16bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb16bppreversed->HasAlphaChannel());
        REQUIRE(rgb16bppbitfields->HasAlphaChannel());

        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb24bppreversed->HasAlphaChannel());

        REQUIRE(rgb32bpp->HasAlphaChannel());
        REQUIRE(rgb32bppreversed->HasAlphaChannel());
        REQUIRE(rgb32bppbitfields->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(rgb8bppgrayscale->IsImageGrayScale());
        REQUIRE(rgb8bppgrayscalereversed->IsImageGrayScale());
        REQUIRE(rgb8bppgrayscalerle->IsImageGrayScale());
        REQUIRE(rgb8bppgrayscalerlereversed->IsImageGrayScale());

        REQUIRE_FALSE(rgb8bpppalette->IsImageGrayScale());
        REQUIRE_FALSE(rgb8bpppalettereversed->IsImageGrayScale());
        REQUIRE_FALSE(rgb8bpppaletterle->IsImageGrayScale());
        REQUIRE_FALSE(rgb8bpppaletterlereversed->IsImageGrayScale());

        REQUIRE_FALSE(rgb16bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb16bppreversed->IsImageGrayScale());
        REQUIRE_FALSE(rgb16bppbitfields->IsImageGrayScale());

        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb24bppreversed->IsImageGrayScale());

        REQUIRE_FALSE(rgb32bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb32bppreversed->IsImageGrayScale());
        REQUIRE_FALSE(rgb32bppbitfields->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(rgb8bppgrayscale->IsImageColored());
        REQUIRE_FALSE(rgb8bppgrayscalereversed->IsImageColored());
        REQUIRE_FALSE(rgb8bppgrayscalerle->IsImageColored());
        REQUIRE_FALSE(rgb8bppgrayscalerlereversed->IsImageColored());

        REQUIRE(rgb8bpppalette->IsImageColored());
        REQUIRE(rgb8bpppalettereversed->IsImageColored());
        REQUIRE(rgb8bpppaletterle->IsImageColored());
        REQUIRE(rgb8bpppaletterlereversed->IsImageColored());

        REQUIRE(rgb16bpp->IsImageColored());
        REQUIRE(rgb16bppreversed->IsImageColored());
        REQUIRE(rgb16bppbitfields->IsImageColored());

        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgb24bppreversed->IsImageColored());

        REQUIRE(rgb32bpp->IsImageColored());
        REQUIRE(rgb32bppreversed->IsImageColored());
        REQUIRE(rgb32bppbitfields->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(rgb8bppgrayscale->IsHDR());
        REQUIRE_FALSE(rgb8bppgrayscalereversed->IsHDR());
        REQUIRE_FALSE(rgb8bppgrayscalerle->IsHDR());
        REQUIRE_FALSE(rgb8bppgrayscalerlereversed->IsHDR());

        REQUIRE_FALSE(rgb8bpppalette->IsHDR());
        REQUIRE_FALSE(rgb8bpppalettereversed->IsHDR());
        REQUIRE_FALSE(rgb8bpppaletterle->IsHDR());
        REQUIRE_FALSE(rgb8bpppaletterlereversed->IsHDR());

        REQUIRE_FALSE(rgb16bpp->IsHDR());
        REQUIRE_FALSE(rgb16bppreversed->IsHDR());
        REQUIRE_FALSE(rgb16bppbitfields->IsHDR());

        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgb24bppreversed->IsHDR());

        REQUIRE_FALSE(rgb32bpp->IsHDR());
        REQUIRE_FALSE(rgb32bppreversed->IsHDR());
        REQUIRE_FALSE(rgb32bppbitfields->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(rgb8bppgrayscale->IsLDR());
        REQUIRE(rgb8bppgrayscalereversed->IsLDR());
        REQUIRE(rgb8bppgrayscalerle->IsLDR());
        REQUIRE(rgb8bppgrayscalerlereversed->IsLDR());

        REQUIRE(rgb8bpppalette->IsLDR());
        REQUIRE(rgb8bpppalettereversed->IsLDR());
        REQUIRE(rgb8bpppaletterle->IsLDR());
        REQUIRE(rgb8bpppaletterlereversed->IsLDR());

        REQUIRE(rgb16bpp->IsLDR());
        REQUIRE(rgb16bppreversed->IsLDR());
        REQUIRE(rgb16bppbitfields->IsLDR());

        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgb24bppreversed->IsLDR());

        REQUIRE(rgb32bpp->IsLDR());
        REQUIRE(rgb32bppreversed->IsLDR());
        REQUIRE(rgb32bppbitfields->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(rgb8bppgrayscale->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscale.bmp");
        REQUIRE(rgb8bppgrayscalereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleReversed.bmp");
        REQUIRE(rgb8bppgrayscalerle->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLE.bmp");
        REQUIRE(rgb8bppgrayscalerlereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPGrayscaleRLEReversed.bmp");

        REQUIRE(rgb8bpppalette->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPalette.bmp");
        REQUIRE(rgb8bpppalettereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteReversed.bmp");
        REQUIRE(rgb8bpppaletterle->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteRLE.bmp");
        REQUIRE(rgb8bpppaletterlereversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test8BPPPaletteRLEReversed.bmp");

        REQUIRE(rgb16bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPP.bmp");
        REQUIRE(rgb16bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPPReversed.bmp");
        REQUIRE(rgb16bppbitfields->GetFilePath() == "Testfiles/ImageLoader/BMP/Test16BPPBitFields.bmp");

        REQUIRE(rgb24bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test24BPP.bmp");
        REQUIRE(rgb24bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test24BPPReversed.bmp");

        REQUIRE(rgb32bpp->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPP.bmp");
        REQUIRE(rgb32bppreversed->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPPReversed.bmp");
        REQUIRE(rgb32bppbitfields->GetFilePath() == "Testfiles/ImageLoader/BMP/Test32BPPBitFields.bmp");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(rgb8bppgrayscale->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(rgb8bppgrayscalereversed->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(rgb8bppgrayscalerle->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(rgb8bppgrayscalerlereversed->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);

        REQUIRE(rgb8bpppalette->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb8bpppalettereversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb8bpppaletterle->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb8bpppaletterlereversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb16bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb16bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb16bppbitfields->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb24bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGB);

        REQUIRE(rgb32bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb32bppreversed->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb32bppbitfields->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/BMP/Test24BPP.bmp"));
    }
}
