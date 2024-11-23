#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/PortableNetworkGraphics/PNGImage.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

TEST_CASE("TRAP::INTERNAL::PNGImage", "[imageloader][pngimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test8BPPGrayscaleSmallest.png");
    const TRAP::Scope<TRAP::Image> grayscale8bppinterlaced = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test8BPPGrayscaleBigInterlaced.png");

    const TRAP::Scope<TRAP::Image> rgb8bpppalette = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/TestPaletteSmallest.png");
    const TRAP::Scope<TRAP::Image> rgb8bpppaletteinterlaced = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/TestPaletteBigInterlaced.png");

    const TRAP::Scope<TRAP::Image> rgb16bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test16BPPSmallest.png");
    const TRAP::Scope<TRAP::Image> rgb16bppinterlaced = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test16BPPBigInterlaced.png");

    const TRAP::Scope<TRAP::Image> rgb24bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test24BPPSmallest.png");
    const TRAP::Scope<TRAP::Image> rgb24bppinterlaced = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test24BPPBigInterlaced.png");

    const TRAP::Scope<TRAP::Image> rgb48bpp = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test48BPPSmallest.png");
    const TRAP::Scope<TRAP::Image> rgb48bppinterlaced = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PNG/Test48BPPBigInterlaced.png");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::INTERNAL::PNGImage>);
        STATIC_REQUIRE(std::copyable<TRAP::INTERNAL::PNGImage>);
        STATIC_REQUIRE(std::movable<TRAP::INTERNAL::PNGImage>);
        STATIC_REQUIRE(std::derived_from<TRAP::INTERNAL::PNGImage, TRAP::Image>);
    }

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscale8bpp);
        REQUIRE(grayscale8bppinterlaced);

        REQUIRE(rgb8bpppalette);
        REQUIRE(rgb8bpppaletteinterlaced);

        REQUIRE(rgb16bpp);
        REQUIRE(rgb16bppinterlaced);

        REQUIRE(rgb24bpp);
        REQUIRE(rgb24bppinterlaced);

        REQUIRE(rgb48bpp);
        REQUIRE(rgb48bppinterlaced);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale8bpp->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale8bppinterlaced->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppinterlaced->GetPixelData(), TestImageSDRPixelDataGrayScale));

        REQUIRE_FALSE(rgb8bpppalette->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppalette->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb8bpppaletteinterlaced->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb8bpppaletteinterlaced->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb16bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb16bppinterlaced->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb16bppinterlaced->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb24bpp->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bpp->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb24bppinterlaced->GetPixelData().empty());
        REQUIRE(std::ranges::equal(rgb24bppinterlaced->GetPixelData(), TestImageSDRPixelDataRGB));

        REQUIRE_FALSE(rgb48bpp->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb48bpp->GetPixelData(), TestImageSDR16PixelDataRGB));
        REQUIRE_FALSE(rgb48bppinterlaced->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb48bppinterlaced->GetPixelData(), TestImageSDR16PixelDataRGB));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale8bppinterlaced->GetChannelsPerPixel() == 1u);

        REQUIRE(rgb8bpppalette->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb8bpppaletteinterlaced->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetChannelsPerPixel() == 4u);
        REQUIRE(rgb16bppinterlaced->GetChannelsPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb24bppinterlaced->GetChannelsPerPixel() == 3u);

        REQUIRE(rgb48bpp->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb48bppinterlaced->GetChannelsPerPixel() == 3u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale8bppinterlaced->GetBitsPerPixel() == 8u);

        REQUIRE(rgb8bpppalette->GetBitsPerPixel() == 32u);
        REQUIRE(rgb8bpppaletteinterlaced->GetBitsPerPixel() == 32u);

        REQUIRE(rgb16bpp->GetBitsPerPixel() == 32u);
        REQUIRE(rgb16bppinterlaced->GetBitsPerPixel() == 32u);

        REQUIRE(rgb24bpp->GetBitsPerPixel() == 24u);
        REQUIRE(rgb24bppinterlaced->GetBitsPerPixel() == 24u);

        REQUIRE(rgb48bpp->GetBitsPerPixel() == 48u);
        REQUIRE(rgb48bppinterlaced->GetBitsPerPixel() == 48u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale8bppinterlaced->GetBytesPerPixel() == 1u);

        REQUIRE(rgb8bpppalette->GetBytesPerPixel() == 4u);
        REQUIRE(rgb8bpppaletteinterlaced->GetBytesPerPixel() == 4u);

        REQUIRE(rgb16bpp->GetBytesPerPixel() == 4u);
        REQUIRE(rgb16bppinterlaced->GetBytesPerPixel() == 4u);

        REQUIRE(rgb24bpp->GetBytesPerPixel() == 3u);
        REQUIRE(rgb24bppinterlaced->GetBytesPerPixel() == 3u);

        REQUIRE(rgb48bpp->GetBytesPerPixel() == 6u);
        REQUIRE(rgb48bppinterlaced->GetBytesPerPixel() == 6u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale8bppinterlaced->GetBitsPerChannel() == 8u);

        REQUIRE(rgb8bpppalette->GetBitsPerChannel() == 8u);
        REQUIRE(rgb8bpppaletteinterlaced->GetBitsPerChannel() == 8u);

        REQUIRE(rgb16bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb16bppinterlaced->GetBitsPerChannel() == 8u);

        REQUIRE(rgb24bpp->GetBitsPerChannel() == 8u);
        REQUIRE(rgb24bppinterlaced->GetBitsPerChannel() == 8u);

        REQUIRE(rgb48bpp->GetBitsPerChannel() == 16u);
        REQUIRE(rgb48bppinterlaced->GetBitsPerChannel() == 16u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8bpp->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale8bppinterlaced->GetBytesPerChannel() == 1u);

        REQUIRE(rgb8bpppalette->GetBytesPerChannel() == 1u);
        REQUIRE(rgb8bpppaletteinterlaced->GetBytesPerChannel() == 1u);

        REQUIRE(rgb16bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb16bppinterlaced->GetBytesPerChannel() == 1u);

        REQUIRE(rgb24bpp->GetBytesPerChannel() == 1u);
        REQUIRE(rgb24bppinterlaced->GetBytesPerChannel() == 1u);

        REQUIRE(rgb48bpp->GetBytesPerChannel() == 2u);
        REQUIRE(rgb48bppinterlaced->GetBytesPerChannel() == 2u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8bpp->GetWidth() == 32u);
        REQUIRE(grayscale8bppinterlaced->GetWidth() == 32u);

        REQUIRE(rgb8bpppalette->GetWidth() == 32u);
        REQUIRE(rgb8bpppaletteinterlaced->GetWidth() == 32u);

        REQUIRE(rgb16bpp->GetWidth() == 32u);
        REQUIRE(rgb16bppinterlaced->GetWidth() == 32u);

        REQUIRE(rgb24bpp->GetWidth() == 32u);
        REQUIRE(rgb24bppinterlaced->GetWidth() == 32u);

        REQUIRE(rgb48bpp->GetWidth() == 32u);
        REQUIRE(rgb48bppinterlaced->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8bpp->GetHeight() == 32u);
        REQUIRE(grayscale8bppinterlaced->GetHeight() == 32u);

        REQUIRE(rgb8bpppalette->GetHeight() == 32u);
        REQUIRE(rgb8bpppaletteinterlaced->GetHeight() == 32u);

        REQUIRE(rgb16bpp->GetHeight() == 32u);
        REQUIRE(rgb16bppinterlaced->GetHeight() == 32u);

        REQUIRE(rgb24bpp->GetHeight() == 32u);
        REQUIRE(rgb24bppinterlaced->GetHeight() == 32u);

        REQUIRE(rgb48bpp->GetHeight() == 32u);
        REQUIRE(rgb48bppinterlaced->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale8bppinterlaced->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb8bpppalette->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb8bpppaletteinterlaced->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb16bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb16bppinterlaced->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb24bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb24bppinterlaced->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb48bpp->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb48bppinterlaced->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8bpp->HasAlphaChannel());
        REQUIRE_FALSE(grayscale8bppinterlaced->HasAlphaChannel());

        REQUIRE(rgb8bpppalette->HasAlphaChannel());
        REQUIRE(rgb8bpppaletteinterlaced->HasAlphaChannel());

        REQUIRE(rgb16bpp->HasAlphaChannel());
        REQUIRE(rgb16bppinterlaced->HasAlphaChannel());

        REQUIRE_FALSE(rgb24bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb24bppinterlaced->HasAlphaChannel());

        REQUIRE_FALSE(rgb48bpp->HasAlphaChannel());
        REQUIRE_FALSE(rgb48bppinterlaced->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8bpp->IsImageGrayScale());
        REQUIRE(grayscale8bppinterlaced->IsImageGrayScale());

        REQUIRE_FALSE(rgb8bpppalette->IsImageGrayScale());
        REQUIRE_FALSE(rgb8bpppaletteinterlaced->IsImageGrayScale());

        REQUIRE_FALSE(rgb16bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb16bppinterlaced->IsImageGrayScale());

        REQUIRE_FALSE(rgb24bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb24bppinterlaced->IsImageGrayScale());

        REQUIRE_FALSE(rgb48bpp->IsImageGrayScale());
        REQUIRE_FALSE(rgb48bppinterlaced->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsImageColored());
        REQUIRE_FALSE(grayscale8bppinterlaced->IsImageColored());

        REQUIRE(rgb8bpppalette->IsImageColored());
        REQUIRE(rgb8bpppaletteinterlaced->IsImageColored());

        REQUIRE(rgb16bpp->IsImageColored());
        REQUIRE(rgb16bppinterlaced->IsImageColored());

        REQUIRE(rgb24bpp->IsImageColored());
        REQUIRE(rgb24bppinterlaced->IsImageColored());

        REQUIRE(rgb48bpp->IsImageColored());
        REQUIRE(rgb48bppinterlaced->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8bpp->IsHDR());
        REQUIRE_FALSE(grayscale8bppinterlaced->IsHDR());

        REQUIRE_FALSE(rgb8bpppalette->IsHDR());
        REQUIRE_FALSE(rgb8bpppaletteinterlaced->IsHDR());

        REQUIRE_FALSE(rgb16bpp->IsHDR());
        REQUIRE_FALSE(rgb16bppinterlaced->IsHDR());

        REQUIRE_FALSE(rgb24bpp->IsHDR());
        REQUIRE_FALSE(rgb24bppinterlaced->IsHDR());

        REQUIRE_FALSE(rgb48bpp->IsHDR());
        REQUIRE_FALSE(rgb48bppinterlaced->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8bpp->IsLDR());
        REQUIRE(grayscale8bppinterlaced->IsLDR());

        REQUIRE(rgb8bpppalette->IsLDR());
        REQUIRE(rgb8bpppaletteinterlaced->IsLDR());

        REQUIRE(rgb16bpp->IsLDR());
        REQUIRE(rgb16bppinterlaced->IsLDR());

        REQUIRE(rgb24bpp->IsLDR());
        REQUIRE(rgb24bppinterlaced->IsLDR());

        REQUIRE(rgb48bpp->IsLDR());
        REQUIRE(rgb48bppinterlaced->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8bpp->GetFilePath() == "Testfiles/ImageLoader/PNG/Test8BPPGrayscaleSmallest.png");
        REQUIRE(grayscale8bppinterlaced->GetFilePath() == "Testfiles/ImageLoader/PNG/Test8BPPGrayscaleBigInterlaced.png");

        REQUIRE(rgb8bpppalette->GetFilePath() == "Testfiles/ImageLoader/PNG/TestPaletteSmallest.png");
        REQUIRE(rgb8bpppaletteinterlaced->GetFilePath() == "Testfiles/ImageLoader/PNG/TestPaletteBigInterlaced.png");

        REQUIRE(rgb16bpp->GetFilePath() == "Testfiles/ImageLoader/PNG/Test16BPPSmallest.png");
        REQUIRE(rgb16bppinterlaced->GetFilePath() == "Testfiles/ImageLoader/PNG/Test16BPPBigInterlaced.png");

        REQUIRE(rgb24bpp->GetFilePath() == "Testfiles/ImageLoader/PNG/Test24BPPSmallest.png");
        REQUIRE(rgb24bppinterlaced->GetFilePath() == "Testfiles/ImageLoader/PNG/Test24BPPBigInterlaced.png");

        REQUIRE(rgb48bpp->GetFilePath() == "Testfiles/ImageLoader/PNG/Test48BPPSmallest.png");
        REQUIRE(rgb48bppinterlaced->GetFilePath() == "Testfiles/ImageLoader/PNG/Test48BPPBigInterlaced.png");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8bpp->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale8bppinterlaced->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);

        REQUIRE(rgb8bpppalette->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb8bpppaletteinterlaced->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb16bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
        REQUIRE(rgb16bppinterlaced->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);

        REQUIRE(rgb24bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb24bppinterlaced->GetColorFormat() == TRAP::Image::ColorFormat::RGB);

        REQUIRE(rgb48bpp->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb48bppinterlaced->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/PNG/Test24BPPSmallest.png"));
    }
}
