#include <catch2/catch_test_macros.hpp>

#include <array>
#include <vector>

// #include <ImageLoader/Image.h>
// #include <ImageLoader/PortableMaps/PPMImage.h>
// #include <FileSystem/FileSystem.h>
#include <Utils/ImageUtils.h>

#include "../Testfiles/Utils/ImageUtils_Src.h"
#include "../Testfiles/Utils/ImageUtils_FlippedX.h"
#include "../Testfiles/Utils/ImageUtils_FlippedY.h"
#include "../Testfiles/Utils/ImageUtils_Rotated90Clockwise.h"
#include "../Testfiles/Utils/ImageUtils_Rotated90CounterClockwise.h"

namespace
{
    // template<typename T>
    // requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    // void DumpPixelDataAsHex(const TRAP::Image& img)
    // {
    //     std::string pixelDataHex{};

    //     const std::span<const T> pixelData(reinterpret_cast<const T*>(img.GetPixelData().data()), img.GetPixelData().size() / sizeof(T));

    //     for(u32 i = 0; i < pixelData.size(); ++i)
    //     {
    //         if constexpr(std::same_as<T, u8> || std::same_as<T, u16>)
    //         {
    //             pixelDataHex += fmt::format("{0:#x}, ", pixelData[i]);
    //         }
    //         else
    //         {
    //             pixelDataHex += fmt::format("{0:f}f, ", pixelData[i]);
    //         }

    //         const bool newLine = ((i + 1) % 16) == 0;
    //         if(newLine)
    //         {
    //             pixelDataHex.pop_back();
    //             pixelDataHex += '\n';
    //         }
    //     }
    //     pixelDataHex.pop_back();

    //     TRAP::FileSystem::WriteTextFile("out.raw", pixelDataHex);
    // }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckFlipX(const u32 width, const u32 height, const u32 channels, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> flippedPixelData = TRAP::Utils::FlipPixelDataX<T>(width, height, channels, srcPixelData);

        if(flippedPixelData.size() != expectedPixelData.size())
            return false;

        if(!std::equal(flippedPixelData.begin(), flippedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckFlipY(const u32 width, const u32 height, const u32 channels, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> flippedPixelData = TRAP::Utils::FlipPixelDataY<T>(width, height, channels, srcPixelData);

        if(flippedPixelData.size() != expectedPixelData.size())
            return false;

        if(!std::equal(flippedPixelData.begin(), flippedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckRotate90Clockwise(const u32 width, const u32 height, const u32 channels, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> flippedPixelData = TRAP::Utils::RotatePixelData90Clockwise(width, height, channels, srcPixelData);

        if(flippedPixelData.size() != expectedPixelData.size())
            return false;

        if(!std::equal(flippedPixelData.begin(), flippedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckRotate90CounterClockwise(const u32 width, const u32 height, const u32 channels, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> flippedPixelData = TRAP::Utils::RotatePixelData90CounterClockwise(width, height, channels, srcPixelData);

        if(flippedPixelData.size() != expectedPixelData.size())
            return false;

        if(!std::equal(flippedPixelData.begin(), flippedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckRGBToRGBA(const u32 width, const u32 height, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> convertedPixelData = TRAP::Utils::ConvertRGBPixelDataToRGBA<T>(width, height, srcPixelData);

        if(convertedPixelData.size() != (width * height * 4u))
            return false;

        if(!std::equal(convertedPixelData.begin(), convertedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr bool CheckRGBAToRGB(const u32 width, const u32 height, const std::span<const T> srcPixelData, const std::span<const T> expectedPixelData)
    {
        const std::vector<T> convertedPixelData = TRAP::Utils::ConvertRGBAPixelDataToRGB<T>(width, height, srcPixelData);

        if(convertedPixelData.size() != (width * height * 3u))
            return false;

        if(!std::equal(convertedPixelData.begin(), convertedPixelData.end(), expectedPixelData.begin()))
            return false;

        return true;
    }
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

TEST_CASE("TRAP::Utils::FlipPixelDataX()", "[utils][imageutils]")
{
    SECTION("GrayScale")
    {
        STATIC_REQUIRE(CheckFlipX<u8>(32u, 32u, 1u, TestImageSDRPixelDataGrayScale, TestImageSDRPixelDataGrayScaleFlippedX));
    }

    SECTION("GrayScaleAlpha")
    {
        STATIC_REQUIRE(CheckFlipX<u8>(32u, 32u, 2u, TestImageSDRPixelDataGrayScaleAlpha, TestImageSDRPixelDataGrayScaleAlphaFlippedX));
    }

    SECTION("SDR RGB")
    {
        STATIC_REQUIRE(CheckFlipX<u8>(32u, 32u, 3u, TestImageSDRPixelDataRGB, TestImageSDRPixelDataRGBFlippedX));
    }

    SECTION("SDR RGBA")
    {
        STATIC_REQUIRE(CheckFlipX<u8>(32u, 32u, 4u, TestImageSDRPixelDataRGBA, TestImageSDRPixelDataRGBAFlippedX));
    }

    SECTION("SDR RGB (u16)")
    {
        STATIC_REQUIRE(CheckFlipX<u16>(32u, 32u, 3u, TestImageSDR16PixelDataRGB, TestImageSDR16PixelDataRGBFlippedX));
    }

    SECTION("SDR RGBA (u16)")
    {
        STATIC_REQUIRE(CheckFlipX<u16>(32u, 32u, 4u, TestImageSDR16PixelDataRGBA, TestImageSDR16PixelDataRGBAFlippedX));
    }

    SECTION("HDR RGB")
    {
        STATIC_REQUIRE(CheckFlipX<f32>(32u, 32u, 3u, TestImageHDRPixelDataRGB, TestImageHDRPixelDataRGBFlippedX));
    }
}

TEST_CASE("TRAP::Utils::FlipPixelDataY()", "[utils][imageutils]")
{
    SECTION("GrayScale")
    {
        STATIC_REQUIRE(CheckFlipY<u8>(32u, 32u, 1u, TestImageSDRPixelDataGrayScale, TestImageSDRPixelDataGrayScaleFlippedY));
    }

    SECTION("GrayScaleAlpha")
    {
        STATIC_REQUIRE(CheckFlipY<u8>(32u, 32u, 2u, TestImageSDRPixelDataGrayScaleAlpha, TestImageSDRPixelDataGrayScaleAlphaFlippedY));
    }

    SECTION("SDR RGB")
    {
        STATIC_REQUIRE(CheckFlipY<u8>(32u, 32u, 3u, TestImageSDRPixelDataRGB, TestImageSDRPixelDataRGBFlippedY));
    }

    SECTION("SDR RGBA")
    {
        STATIC_REQUIRE(CheckFlipY<u8>(32u, 32u, 4u, TestImageSDRPixelDataRGBA, TestImageSDRPixelDataRGBAFlippedY));
    }

    SECTION("SDR RGB (u16)")
    {
        STATIC_REQUIRE(CheckFlipY<u16>(32u, 32u, 3u, TestImageSDR16PixelDataRGB, TestImageSDR16PixelDataRGBFlippedY));
    }

    SECTION("SDR RGBA (u16)")
    {
        STATIC_REQUIRE(CheckFlipY<u16>(32u, 32u, 4u, TestImageSDR16PixelDataRGBA, TestImageSDR16PixelDataRGBAFlippedY));
    }

    SECTION("HDR RGB")
    {
        STATIC_REQUIRE(CheckFlipY<f32>(32u, 32u, 3u, TestImageHDRPixelDataRGB, TestImageHDRPixelDataRGBFlippedY));
    }
}

TEST_CASE("TRAP::Utils::RotatePixelData90Clockwise()", "[utils][imageutils]")
{
    SECTION("GrayScale")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u8>(32u, 32u, 1u, TestImageSDRPixelDataGrayScale, TestImageSDRPixelDataGrayScaleRotated90Clockwise));
    }

    SECTION("GrayScaleAlpha")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u8>(32u, 32u, 2u, TestImageSDRPixelDataGrayScaleAlpha, TestImageSDRPixelDataGrayScaleAlphaRotated90Clockwise));
    }

    SECTION("SDR RGB")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u8>(32u, 32u, 3u, TestImageSDRPixelDataRGB, TestImageSDRPixelDataRGBRotated90Clockwise));
    }

    SECTION("SDR RGBA")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u8>(32u, 32u, 4u, TestImageSDRPixelDataRGBA, TestImageSDRPixelDataRGBARotated90Clockwise));
    }

    SECTION("SDR RGB (u16)")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u16>(32u, 32u, 3u, TestImageSDR16PixelDataRGB, TestImageSDR16PixelDataRGBRotated90Clockwise));
    }

    SECTION("SDR RGBA (u16)")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<u16>(32u, 32u, 4u, TestImageSDR16PixelDataRGBA, TestImageSDR16PixelDataRGBARotated90Clockwise));
    }

    SECTION("HDR RGB")
    {
        STATIC_REQUIRE(CheckRotate90Clockwise<f32>(32u, 32u, 3u, TestImageHDRPixelDataRGB, TestImageHDRPixelDataRGBRotated90Clockwise));
    }
}

TEST_CASE("TRAP::Utils::RotatePixelData90CounterClockwise()", "[utils][imageutils]")
{
    SECTION("GrayScale")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u8>(32u, 32u, 1u, TestImageSDRPixelDataGrayScale, TestImageSDRPixelDataGrayScaleRotated90CounterClockwise));
    }

    SECTION("GrayScaleAlpha")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u8>(32u, 32u, 2u, TestImageSDRPixelDataGrayScaleAlpha, TestImageSDRPixelDataGrayScaleAlphaRotated90CounterClockwise));
    }

    SECTION("SDR RGB")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u8>(32u, 32u, 3u, TestImageSDRPixelDataRGB, TestImageSDRPixelDataRGBRotated90CounterClockwise));
    }

    SECTION("SDR RGBA")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u8>(32u, 32u, 4u, TestImageSDRPixelDataRGBA, TestImageSDRPixelDataRGBARotated90CounterClockwise));
    }

    SECTION("SDR RGB (u16)")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u16>(32u, 32u, 3u, TestImageSDR16PixelDataRGB, TestImageSDR16PixelDataRGBRotated90CounterClockwise));
    }

    SECTION("SDR RGBA (u16)")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<u16>(32u, 32u, 4u, TestImageSDR16PixelDataRGBA, TestImageSDR16PixelDataRGBARotated90CounterClockwise));
    }

    SECTION("HDR RGB")
    {
        STATIC_REQUIRE(CheckRotate90CounterClockwise<f32>(32u, 32u, 3u, TestImageHDRPixelDataRGB, TestImageHDRPixelDataRGBRotated90CounterClockwise));
    }
}

TEST_CASE("TRAP::Utils::ConvertRGBPixelDataToRGBA()", "[utils][imageutils]")
{
    SECTION("GrayScale")
    {
        STATIC_REQUIRE_FALSE(CheckRGBToRGBA<u8>(32u, 32u, TestImageSDRPixelDataGrayScale, TestImageSDRPixelDataGrayScaleAlpha));
    }

    SECTION("SDR RGB")
    {
        STATIC_REQUIRE(CheckRGBToRGBA<u8>(32u, 32u, TestImageSDRPixelDataRGB, TestImageSDRPixelDataRGBA));
    }

    SECTION("SDR RGB (u16)")
    {
        STATIC_REQUIRE(CheckRGBToRGBA<u16>(32u, 32u, TestImageSDR16PixelDataRGB, TestImageSDR16PixelDataRGBA));
    }
}

TEST_CASE("TRAP::Utils::ConvertRGBPixelDataToRGBA()", "[utils][imageutils]")
{
    SECTION("GrayScaleAlpha")
    {
        STATIC_REQUIRE_FALSE(CheckRGBAToRGB<u8>(32u, 32u, TestImageSDRPixelDataGrayScaleAlpha, TestImageSDRPixelDataGrayScale));
    }

    SECTION("SDR RGBA")
    {
        STATIC_REQUIRE(CheckRGBAToRGB<u8>(32u, 32u, TestImageSDRPixelDataRGBA, TestImageSDRPixelDataRGB));
    }

    SECTION("SDR RGBA (u16)")
    {
        STATIC_REQUIRE(CheckRGBAToRGB<u16>(32u, 32u, TestImageSDR16PixelDataRGBA, TestImageSDR16PixelDataRGB));
    }
}
