#include <catch2/catch_test_macros.hpp>

#include "ImageLoader/Image.h"

#include "../Testfiles/Utils/ImageUtils_Src.h"
#include "../Testfiles/Utils/ImageUtils_FlippedX.h"
#include "../Testfiles/Utils/ImageUtils_FlippedY.h"
#include "../Testfiles/Utils/ImageUtils_Rotated90Clockwise.h"
#include "../Testfiles/Utils/ImageUtils_Rotated90CounterClockwise.h"
#include "Embed.h"

TEST_CASE("TRAP::Image", "[imageloader][image]")
{
    SECTION("Class requirements")
    {
        STATIC_REQUIRE_FALSE(std::is_final_v<TRAP::Image>);
        STATIC_REQUIRE_FALSE(std::copyable<TRAP::Image>);
        STATIC_REQUIRE_FALSE(std::movable<TRAP::Image>);
    }

    SECTION("LoadFromFile()")
    {
        const TRAP::Scope<TRAP::Image> unsupportedImg = TRAP::Image::LoadFromFile("unsupportedFile.random");
        REQUIRE(unsupportedImg);
        REQUIRE(std::ranges::equal(unsupportedImg->GetPixelData(), TRAP::Embed::DefaultImageData));
        REQUIRE(unsupportedImg->GetChannelsPerPixel() == 4u);
        REQUIRE(unsupportedImg->GetBitsPerPixel() == 32u);
        REQUIRE(unsupportedImg->GetBytesPerPixel() == 4u);
        REQUIRE(unsupportedImg->GetBitsPerChannel() == 8u);
        REQUIRE(unsupportedImg->GetBytesPerChannel() == 1u);
        REQUIRE(unsupportedImg->GetWidth() == 32u);
        REQUIRE(unsupportedImg->GetHeight() == 32u);
        REQUIRE(unsupportedImg->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(unsupportedImg->HasAlphaChannel());
        REQUIRE_FALSE(unsupportedImg->IsImageGrayScale());
        REQUIRE(unsupportedImg->IsImageColored());
        REQUIRE_FALSE(unsupportedImg->IsHDR());
        REQUIRE(unsupportedImg->IsLDR());
        REQUIRE(unsupportedImg->GetFilePath() == "unsupportedFile.random");
        REQUIRE(unsupportedImg->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }

    SECTION("LoadFallback()")
    {
        const TRAP::Scope<TRAP::Image> fallbackImg = TRAP::Image::LoadFallback();
        REQUIRE(fallbackImg);
        REQUIRE(std::ranges::equal(fallbackImg->GetPixelData(), TRAP::Embed::DefaultImageData));
        REQUIRE(fallbackImg->GetChannelsPerPixel() == 4u);
        REQUIRE(fallbackImg->GetBitsPerPixel() == 32u);
        REQUIRE(fallbackImg->GetBytesPerPixel() == 4u);
        REQUIRE(fallbackImg->GetBitsPerChannel() == 8u);
        REQUIRE(fallbackImg->GetBytesPerChannel() == 1u);
        REQUIRE(fallbackImg->GetWidth() == 32u);
        REQUIRE(fallbackImg->GetHeight() == 32u);
        REQUIRE(fallbackImg->GetSize() == TRAP::Math::Vec2ui(32u, 32u));
        REQUIRE(fallbackImg->HasAlphaChannel());
        REQUIRE_FALSE(fallbackImg->IsImageGrayScale());
        REQUIRE(fallbackImg->IsImageColored());
        REQUIRE_FALSE(fallbackImg->IsHDR());
        REQUIRE(fallbackImg->IsLDR());
        REQUIRE(fallbackImg->GetFilePath().empty());
        REQUIRE(fallbackImg->GetColorFormat() == TRAP::Image::ColorFormat::RGBA);
    }

    SECTION("IsSupportedImageFile()")
    {
        REQUIRE_FALSE(TRAP::Image::IsSupportedImageFile(""));
        REQUIRE_FALSE(TRAP::Image::IsSupportedImageFile("unsupportedFile.random"));
    }

    SECTION("FlipX()")
    {
        const auto sdrrgba = TRAP::Image::FlipX(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end())));
        REQUIRE(std::ranges::equal(sdrrgba->GetPixelData(), TestImageSDRPixelDataRGBAFlippedX));

        const auto sdr16rgba = TRAP::Image::FlipX(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u16>(TestImageSDR16PixelDataRGBA.begin(), TestImageSDR16PixelDataRGBA.end())));
        const std::span<const u16> sdr16rgbaPixelData(reinterpret_cast<const u16*>(sdr16rgba->GetPixelData().data()), sdr16rgba->GetPixelData().size() / sizeof(u16));
        REQUIRE(std::ranges::equal(sdr16rgbaPixelData, TestImageSDR16PixelDataRGBAFlippedX));

        const auto hdrrgb = TRAP::Image::FlipX(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end())));
        const std::span<const f32> hdrrgbPixelData(reinterpret_cast<const f32*>(hdrrgb->GetPixelData().data()), hdrrgb->GetPixelData().size() / sizeof(f32));
        REQUIRE(std::ranges::equal(hdrrgbPixelData, TestImageHDRPixelDataRGBFlippedX));
    }

    SECTION("FlipY()")
    {
        const auto sdrrgba = TRAP::Image::FlipY(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end())));
        REQUIRE(std::ranges::equal(sdrrgba->GetPixelData(), TestImageSDRPixelDataRGBAFlippedY));

        const auto sdr16rgba = TRAP::Image::FlipY(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u16>(TestImageSDR16PixelDataRGBA.begin(), TestImageSDR16PixelDataRGBA.end())));
        const std::span<const u16> sdr16rgbaPixelData(reinterpret_cast<const u16*>(sdr16rgba->GetPixelData().data()), sdr16rgba->GetPixelData().size() / sizeof(u16));
        REQUIRE(std::ranges::equal(sdr16rgbaPixelData, TestImageSDR16PixelDataRGBAFlippedY));

        const auto hdrrgb = TRAP::Image::FlipY(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end())));
        const std::span<const f32> hdrrgbPixelData(reinterpret_cast<const f32*>(hdrrgb->GetPixelData().data()), hdrrgb->GetPixelData().size() / sizeof(f32));
        REQUIRE(std::ranges::equal(hdrrgbPixelData, TestImageHDRPixelDataRGBFlippedY));
    }

    SECTION("Rotate90CLockwise()")
    {
        const auto sdrrgba = TRAP::Image::Rotate90Clockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end())));
        REQUIRE(std::ranges::equal(sdrrgba->GetPixelData(), TestImageSDRPixelDataRGBARotated90Clockwise));

        const auto sdr16rgba = TRAP::Image::Rotate90Clockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u16>(TestImageSDR16PixelDataRGBA.begin(), TestImageSDR16PixelDataRGBA.end())));
        const std::span<const u16> sdr16rgbaPixelData(reinterpret_cast<const u16*>(sdr16rgba->GetPixelData().data()), sdr16rgba->GetPixelData().size() / sizeof(u16));
        REQUIRE(std::ranges::equal(sdr16rgbaPixelData, TestImageSDR16PixelDataRGBARotated90Clockwise));

        const auto hdrrgb = TRAP::Image::Rotate90Clockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end())));
        const std::span<const f32> hdrrgbPixelData(reinterpret_cast<const f32*>(hdrrgb->GetPixelData().data()), hdrrgb->GetPixelData().size() / sizeof(f32));
        REQUIRE(std::ranges::equal(hdrrgbPixelData, TestImageHDRPixelDataRGBRotated90Clockwise));
    }

    SECTION("Rotate90CounterClockwise()")
    {
        const auto sdrrgba = TRAP::Image::Rotate90CounterClockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end())));
        REQUIRE(std::ranges::equal(sdrrgba->GetPixelData(), TestImageSDRPixelDataRGBARotated90CounterClockwise));

        const auto sdr16rgba = TRAP::Image::Rotate90CounterClockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u16>(TestImageSDR16PixelDataRGBA.begin(), TestImageSDR16PixelDataRGBA.end())));
        const std::span<const u16> sdr16rgbaPixelData(reinterpret_cast<const u16*>(sdr16rgba->GetPixelData().data()), sdr16rgba->GetPixelData().size() / sizeof(u16));
        REQUIRE(std::ranges::equal(sdr16rgbaPixelData, TestImageSDR16PixelDataRGBARotated90CounterClockwise));

        const auto hdrrgb = TRAP::Image::Rotate90CounterClockwise(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end())));
        const std::span<const f32> hdrrgbPixelData(reinterpret_cast<const f32*>(hdrrgb->GetPixelData().data()), hdrrgb->GetPixelData().size() / sizeof(f32));
        REQUIRE(std::ranges::equal(hdrrgbPixelData, TestImageHDRPixelDataRGBRotated90CounterClockwise));
    }

    SECTION("CovertRGBToRGBA()")
    {
        const auto invalidrgba = TRAP::Image::ConvertRGBToRGBA(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGBA, std::vector<u8>(TestImageSDRPixelDataRGBA.begin(), TestImageSDRPixelDataRGBA.end())));
        REQUIRE_FALSE(invalidrgba);

        const auto sdrgrayscale = TRAP::Image::ConvertRGBToRGBA(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::GrayScale, std::vector<u8>(TestImageSDRPixelDataGrayScale.begin(), TestImageSDRPixelDataGrayScale.end())));
        REQUIRE_FALSE(sdrgrayscale);

        const auto sdrrgba = TRAP::Image::ConvertRGBToRGBA(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<u8>(TestImageSDRPixelDataRGB.begin(), TestImageSDRPixelDataRGB.end())));
        REQUIRE(std::ranges::equal(sdrrgba->GetPixelData(), TestImageSDRPixelDataRGBA));

        const auto sdr16rgba = TRAP::Image::ConvertRGBToRGBA(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<u16>(TestImageSDR16PixelDataRGB.begin(), TestImageSDR16PixelDataRGB.end())));
        const std::span<const u16> sdr16rgbaPixelData(reinterpret_cast<const u16*>(sdr16rgba->GetPixelData().data()), sdr16rgba->GetPixelData().size() / sizeof(u16));
        REQUIRE(std::ranges::equal(sdr16rgbaPixelData, TestImageSDR16PixelDataRGBA));

        const auto hdrrgb = TRAP::Image::ConvertRGBToRGBA(*TRAP::Image::LoadFromMemory(32u, 32u, TRAP::Image::ColorFormat::RGB, std::vector<f32>(TestImageHDRPixelDataRGB.begin(), TestImageHDRPixelDataRGB.end())));
        const std::span<const f32> hdrrgbPixelData(reinterpret_cast<const f32*>(hdrrgb->GetPixelData().data()), hdrrgb->GetPixelData().size() / sizeof(f32));
        REQUIRE(std::ranges::equal(hdrrgbPixelData, TestImageHDRPixelDataRGBA));
    }
}
