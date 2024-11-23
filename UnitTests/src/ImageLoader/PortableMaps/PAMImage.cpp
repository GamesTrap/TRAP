#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/PortableMaps/PAMImage.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"

//TODO Add fuzz testing for this class

TEST_CASE("TRAP::INTERNAL::PAMImage", "[imageloader][pamimage]")
{
    const TRAP::Scope<TRAP::Image> grayscale8BPP = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PAM/Test8BPPGrayscale.pam");
    const TRAP::Scope<TRAP::Image> grayscale16BPP = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PAM/Test16BPPGrayscale.pam");

    const TRAP::Scope<TRAP::Image> rgb24BPP = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PAM/Test24BPP.pam");
    const TRAP::Scope<TRAP::Image> rgb48BPP = TRAP::Image::LoadFromFile("Testfiles/ImageLoader/PM/PAM/Test48BPP.pam");

    SECTION("Class requirements")
    {
        STATIC_REQUIRE(std::is_final_v<TRAP::INTERNAL::PAMImage>);
        STATIC_REQUIRE(std::copyable<TRAP::INTERNAL::PAMImage>);
        STATIC_REQUIRE(std::movable<TRAP::INTERNAL::PAMImage>);
        STATIC_REQUIRE(std::derived_from<TRAP::INTERNAL::PAMImage, TRAP::Image>);
    }

    SECTION("LoadFromFile()")
    {
        REQUIRE(grayscale8BPP);
        REQUIRE(grayscale16BPP);

        REQUIRE(rgb24BPP);
        REQUIRE(rgb48BPP);
    }

    SECTION("GetPixelData()")
    {
        REQUIRE_FALSE(grayscale8BPP->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale8BPP->GetPixelData(), TestImageSDRPixelDataGrayScale));
        REQUIRE_FALSE(grayscale16BPP->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(grayscale16BPP->GetPixelData(), TestImageSDR16PixelDataGrayScale));

        REQUIRE_FALSE(rgb24BPP->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb24BPP->GetPixelData(), TestImageSDRPixelDataRGB));
        REQUIRE_FALSE(rgb48BPP->GetPixelData().empty());
        // REQUIRE(std::ranges::equal(rgb48BPP->GetPixelData(), TestImageSDR16PixelDataRGB));
    }

    SECTION("GetChannelsPerPixel()")
    {
        REQUIRE(grayscale8BPP->GetChannelsPerPixel() == 1u);
        REQUIRE(grayscale16BPP->GetChannelsPerPixel() == 1u);

        REQUIRE(rgb24BPP->GetChannelsPerPixel() == 3u);
        REQUIRE(rgb48BPP->GetChannelsPerPixel() == 3u);
    }

    SECTION("GetBitsPerPixel()")
    {
        REQUIRE(grayscale8BPP->GetBitsPerPixel() == 8u);
        REQUIRE(grayscale16BPP->GetBitsPerPixel() == 16u);

        REQUIRE(rgb24BPP->GetBitsPerPixel() == 24u);
        REQUIRE(rgb48BPP->GetBitsPerPixel() == 48u);
    }

    SECTION("GetBytesPerPixel()")
    {
        REQUIRE(grayscale8BPP->GetBytesPerPixel() == 1u);
        REQUIRE(grayscale16BPP->GetBytesPerPixel() == 2u);

        REQUIRE(rgb24BPP->GetBytesPerPixel() == 3u);
        REQUIRE(rgb48BPP->GetBytesPerPixel() == 6u);
    }

    SECTION("GetBitsPerChannel()")
    {
        REQUIRE(grayscale8BPP->GetBitsPerChannel() == 8u);
        REQUIRE(grayscale16BPP->GetBitsPerChannel() == 16u);

        REQUIRE(rgb24BPP->GetBitsPerChannel() == 8u);
        REQUIRE(rgb48BPP->GetBitsPerChannel() == 16u);
    }

    SECTION("GetBytesPerChannel()")
    {
        REQUIRE(grayscale8BPP->GetBytesPerChannel() == 1u);
        REQUIRE(grayscale16BPP->GetBytesPerChannel() == 2u);

        REQUIRE(rgb24BPP->GetBytesPerChannel() == 1u);
        REQUIRE(rgb48BPP->GetBytesPerChannel() == 2u);
    }

    SECTION("GetWidth()")
    {
        REQUIRE(grayscale8BPP->GetWidth() == 32u);
        REQUIRE(grayscale16BPP->GetWidth() == 32u);

        REQUIRE(rgb24BPP->GetWidth() == 32u);
        REQUIRE(rgb48BPP->GetWidth() == 32u);
    }

    SECTION("GetHeight()")
    {
        REQUIRE(grayscale8BPP->GetHeight() == 32u);
        REQUIRE(grayscale16BPP->GetHeight() == 32u);

        REQUIRE(rgb24BPP->GetHeight() == 32u);
        REQUIRE(rgb48BPP->GetHeight() == 32u);
    }

    SECTION("GetSize()")
    {
        REQUIRE(grayscale8BPP->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(grayscale16BPP->GetSize() == TRAP::Math::Vec2ui(32u, 32u));

        REQUIRE(rgb24BPP->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(rgb48BPP->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
    }

    SECTION("HasAlphaChannel()")
    {
        REQUIRE_FALSE(grayscale8BPP->HasAlphaChannel());
        REQUIRE_FALSE(grayscale16BPP->HasAlphaChannel());

        REQUIRE_FALSE(rgb24BPP->HasAlphaChannel());
        REQUIRE_FALSE(rgb48BPP->HasAlphaChannel());
    }

    SECTION("IsImageGrayScale()")
    {
        REQUIRE(grayscale8BPP->IsImageGrayScale());
        REQUIRE(grayscale16BPP->IsImageGrayScale());

        REQUIRE_FALSE(rgb24BPP->IsImageGrayScale());
        REQUIRE_FALSE(rgb48BPP->IsImageGrayScale());
    }

    SECTION("IsImageColored()")
    {
        REQUIRE_FALSE(grayscale8BPP->IsImageColored());
        REQUIRE_FALSE(grayscale16BPP->IsImageColored());

        REQUIRE(rgb24BPP->IsImageColored());
        REQUIRE(rgb48BPP->IsImageColored());
    }

    SECTION("IsHDR()")
    {
        REQUIRE_FALSE(grayscale8BPP->IsHDR());
        REQUIRE_FALSE(grayscale16BPP->IsHDR());

        REQUIRE_FALSE(rgb24BPP->IsHDR());
        REQUIRE_FALSE(rgb48BPP->IsHDR());
    }

    SECTION("IsLDR()")
    {
        REQUIRE(grayscale8BPP->IsLDR());
        REQUIRE(grayscale16BPP->IsLDR());

        REQUIRE(rgb24BPP->IsLDR());
        REQUIRE(rgb48BPP->IsLDR());
    }

    SECTION("GetFilePath()")
    {
        REQUIRE(grayscale8BPP->GetFilePath() == "Testfiles/ImageLoader/PM/PAM/Test8BPPGrayscale.pam");
        REQUIRE(grayscale16BPP->GetFilePath() == "Testfiles/ImageLoader/PM/PAM/Test16BPPGrayscale.pam");

        REQUIRE(rgb24BPP->GetFilePath() == "Testfiles/ImageLoader/PM/PAM/Test24BPP.pam");
        REQUIRE(rgb48BPP->GetFilePath() == "Testfiles/ImageLoader/PM/PAM/Test48BPP.pam");
    }

    SECTION("GetColorFormat()")
    {
        REQUIRE(grayscale8BPP->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);
        REQUIRE(grayscale16BPP->GetColorFormat() == TRAP::Image::ColorFormat::GrayScale);

        REQUIRE(rgb24BPP->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
        REQUIRE(rgb48BPP->GetColorFormat() == TRAP::Image::ColorFormat::RGB);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE(TRAP::Image::IsSupportedImageFile("Testfiles/ImageLoader/PM/PAM/Test24BPP.pam"));
    }
}
