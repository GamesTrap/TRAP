#ifndef TRAP_IMAGEUTILS_H
#define TRAP_IMAGEUTILS_H

#include <span>
#include <vector>

#include "Core/Types.h"
#include "TRAP_Assert.h"
#include "Utils/NumericCasts.h"

namespace TRAP::Utils
{
    /// @brief Flips raw pixel data on X axis.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param channels Number of channels contained in the image.
    /// @param pixelData Raw pixel data.
    /// @return Flipped raw pixel data.
    template <typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> FlipPixelDataX(const u32 width, const u32 height, const u32 channels,
                                                          const std::span<const T> pixelData)
    {
        if(width == 0u || height == 0u || channels == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataX(): One or more parameters are 0!");
            return {};
        }
        if(channels > 4u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataX(): Channel count must be between 1 and 4!");
            return {};
        }
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataX(): Raw pixel data size mismatches width * height * channels!");
            return {};
        }

        std::vector<T> flippedData(pixelData.size());
        const u32 rowSize = width * channels;

        for(u32 y = 0u; y < height; ++y)
        {
            const T* const srcRowStart = pixelData.data() + (y * rowSize);
            T* const dstRowStart = flippedData.data() + (y * rowSize);

            for(u32 x = 0u; x < width; ++x)
            {
                const T* const srcPixel = srcRowStart + (x * channels);
                T* const dstPixel = dstRowStart + ((width - 1u - x) * channels);

                std::copy_n(srcPixel, channels, dstPixel);
            }
        }

        return flippedData;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Flip raw pixel data on Y axis.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param channels Number of channels contained in the image.
    /// @param data Raw pixel data.
    /// @return Flipped raw pixel data.
    template <typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> FlipPixelDataY(const u32 width, const u32 height, const u32 channels,
                                                          const std::span<const T> pixelData)
    {
        if(width == 0u || height == 0u || channels == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataY(): One or more parameters are 0!");
            return {};
        }
        if(channels > 4u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataY(): Channel count must be between 1 and 4!");
            return {};
        }
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataY(): Raw pixel data size mismatches width * height * channels!");
            return {};
        }

        std::vector<T> newData(pixelData.begin(), pixelData.end());
        const u32 rowSize = width * channels;

        for (u32 lowOffset = 0u, highOffset = (height - 1u) * rowSize;
             lowOffset < highOffset;
             lowOffset += rowSize, highOffset -= rowSize)
        {
            std::swap_ranges(newData.begin() + lowOffset, newData.begin() + lowOffset + rowSize, newData.begin() + highOffset);
        }

        return newData;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Rotate raw pixel data by 90 degrees clockwise.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param channels Number of channels contained in the image.
    /// @param data Raw pixel data.
    /// @return Rotated raw pixel data.
    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> RotatePixelData90Clockwise(const u32 width, const u32 height,
                                                                      const u32 channels,
                                                                      const std::span<const T> data)
    {
        if(width == 0u || height == 0u || channels == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90Clockwise(): One or more parameters are 0!");
            return {};
        }
        if(channels > 4u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90Clockwise(): Channel count must be between 1 and 4!");
            return {};
        }
        if(data.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90Clockwise: Raw pixel data size mismatches width * height * channels!");
            return {};
        }

        std::vector<T> rotated(data.size());

        const u32 lastRowIndex = height - 1u;
        const u32 stride = width * channels;

        for(u32 y = 0u; y < height; ++y)
        {
            const u32 srcOffset = y * stride;
            const u32 destCol = lastRowIndex - y;

            for(u32 x = 0u; x < width; ++x)
            {
                const u32 destOffset = (x * height + destCol) * channels;
                const u32 srcXOffset = x * channels;

                std::copy_n(data.begin() + srcOffset + srcXOffset, channels, rotated.begin() + destOffset);
            }
        }

        return rotated;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Rotate raw pixel data by 90 degrees counter clockwise.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param channels Number of channels contained in the image.
    /// @param data Raw pixel data.
    /// @return Rotate raw pixel data.
    template<typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> RotatePixelData90CounterClockwise(const u32 width, const u32 height,
                                                                             const u32 channels,
                                                                             const std::span<const T> pixelData)
    {
        if(width == 0u || height == 0u || channels == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90CounterClockwise(): One or more parameters are 0!");
            return {};
        }
        if(channels > 4u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90CounterClockwise(): Channel count must be between 1 and 4!");
            return {};
        }
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90CounterClockwise: Raw pixel data size mismatches width * height * channels!");
            return std::vector<T>();
        }

        std::vector<T> rotated(pixelData.size());

        for(u32 y = 0u; y < height; ++y)
        {
            for(u32 x = 0u; x < width; ++x)
            {
                const u32 rotatedIndexBase = ((width - 1u - x) * height + y) * channels;
                const u32 originalIndexBase = (y * width + x) * channels;

                std::copy_n(pixelData.begin() + originalIndexBase, channels, rotated.begin() + rotatedIndexBase);
            }
        }

        return rotated;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Converts raw RGB pixel data to RGBA.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param data Raw pixel data.
    /// @return Converted RGBA raw pixel data
    template <typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> ConvertRGBPixelDataToRGBA(const u32 width, const u32 height,
                                                                     const std::span<const T> data)
    {
        constexpr u32 RGBChannels = 3u;
        constexpr u32 RGBAChannels = 4u;

        if(width == 0u || height == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertRGBPixelDataToRGBA(): One or more parameters are 0!");
            return {};
        }
        if(data.size() != (width * height * RGBChannels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertRGBPixelDataToRGBA(): Raw pixel data size mismatches width * height * 3!");
            return std::vector<T>();
        }

        T whitePixelColor;
        if constexpr (std::same_as<T, u8> || std::same_as<T, u16>)
            whitePixelColor = std::numeric_limits<T>::max();
        else
            whitePixelColor = 1.0f;

        std::vector<T> newData(width * height * RGBAChannels);
        for(usize oldDataIndex = 0u, newDataIndex = 0u; oldDataIndex < data.size(); oldDataIndex += RGBChannels, newDataIndex += RGBAChannels)
        {
            std::copy_n(data.begin() + oldDataIndex, RGBChannels, &newData[newDataIndex]); //Copy RGB
            newData[newDataIndex + RGBChannels] = whitePixelColor; //Add alpha channel
        }

        return newData;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Converts raw RGBA pixel data to RGB.
    /// @tparam T u8, u16 or f32.
    /// @param width Width of image in pixels.
    /// @param height Height of image in pixels.
    /// @param data Raw pixel data.
    /// @return Converted RGB raw pixel data.
    template <typename T>
    requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
    [[nodiscard]] constexpr std::vector<T> ConvertRGBAPixelDataToRGB(const u32 width, const u32 height,
                                                                     const std::span<const T> data)
    {
        constexpr u32 RGBChannels = 3u;
        constexpr u32 RGBAChannels = 4u;

        if(width == 0u || height == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertRGBAPixelDataToRGB(): One or more parameters are 0!");
            return {};
        }
        if(data.size() != (width * height * RGBAChannels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertRGBAPixelDataToRGB(): Raw pixel data size mismatches width * height * 4!");
            return std::vector<T>();
        }

        std::vector<T> newData(width * height * RGBChannels);
        for(usize oldDataIndex = 0u, newDataIndex = 0u; oldDataIndex < data.size(); oldDataIndex += RGBAChannels, newDataIndex += RGBChannels)
        {
            std::copy_n(&data[oldDataIndex], RGBChannels, &newData[newDataIndex]);
        }

        return newData;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Converts BGR16 pixel data to RGB24.
    /// @param source BGR16 pixel data.
    /// @param width Width of the image.
    /// @param height Height of the image.
    /// @return RGB24 pixel data.
    [[nodiscard]] constexpr std::vector<u8> ConvertBGR16PixelDataToRGB24(const std::span<const u8> source,
                                                                         const u32 width, const u32 height)
    {
        constexpr u32 BRG16Channels = 2u;
        constexpr u32 RGBChannels = 3u;

        if(width == 0u || height == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertBGR16PixelDataToRGB24(): One or more parameters are 0!");
            return {};
        }
        if(source.size() != (NumericCast<usize>(width) * height * BRG16Channels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGR16PixelDataToRGB24(): Raw pixel data size mismatches width * height * 2!");
            return std::vector<u8>();
        }

        std::vector<u8> data(NumericCast<usize>(width) * height * RGBChannels);

        for (u32 oldDataIndex = 0u, newDataIndex = 0u;
            oldDataIndex < (width * height * BRG16Channels);
            oldDataIndex += BRG16Channels, newDataIndex += RGBChannels)
        {
            data[newDataIndex + 0u] = NumericCast<u8>(source[oldDataIndex + 1u] << 1u) & 0xF8u;
            data[newDataIndex + 1u] = NumericCast<u8>(NumericCast<u8>((source[oldDataIndex + 1u]) << 6u) | NumericCast<u8>(source[oldDataIndex] >> 2u)) & 0xF8u;
            data[newDataIndex + 2u] = NumericCast<u8>(source[oldDataIndex] << 3u) & 0xF8u;
        }

        return data;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Converts BGR24 pixel data to RGB24.
    /// @param source BGR24 pixel data.
    /// @param width Width of the image.
    /// @param height Height of the image.
    /// @return RGB24 pixel data.
    [[nodiscard]] constexpr std::vector<u8> ConvertBGR24PixelDataToRGB24(const std::span<const u8> source,
                                                                         const u32 width, const u32 height)
    {
        constexpr u32 RGBChannels = 3u;

        if(width == 0u || height == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertBGR24PixelDataToRGB24(): One or more parameters are 0!");
            return {};
        }
        if(source.size() != (NumericCast<usize>(width) * height * RGBChannels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGR24PixelDataToRGB24(): Raw pixel data size mismatches width * height * 3!");
            return std::vector<u8>();
        }

        std::vector<u8> data(source.begin(), source.end());

        for (u32 i = 0u; i < data.size(); i += RGBChannels)
            data[i] ^= data[i + 2u] ^= data[i] ^= data[i + 2u];

        return data;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Converts BGR32 pixel data to RGB32.
    /// @param source BGR32 pixel data.
    /// @param width Width of the image.
    /// @param height Height of the image.
    /// @return RGB32 pixel data.
    [[nodiscard]] constexpr std::vector<u8> ConvertBGRA32PixelDataToRGBA32(const std::span<const u8> source,
                                                                           const u32 width, const u32 height)
    {
        constexpr u32 RGBAChannels = 4u;

        if(width == 0u || height == 0u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertBGRA32PixelDataToRGBA32(): One or more parameters are 0!");
            return {};
        }
        if(source.size() != (NumericCast<usize>(width) * height * RGBAChannels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGRA32PixelDataToRGBA32(): Raw pixel data size mismatches width * height * 4!");
            return std::vector<u8>();
        }

        std::vector<u8> data(source.begin(), source.end());

        for (u32 i = 0u; i < data.size(); i += RGBAChannels)
            data[i] ^= data[i + 2u] ^= data[i] ^= data[i + 2u];

        return data;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Decode indexed pixel data.
    /// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
    /// @param source Indexed BGRA pixel data.
    /// @param width Width of the image.
    /// @param height Height of the image.
    /// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB.
    /// @param colorMap Color table.
    /// @return Decoded pixel data.
    [[nodiscard]] constexpr std::vector<u8> DecodeBGRAMappedPixelData(const std::span<const u8> source,
                                                                      const u32 width, const u32 height,
                                                                      const u32 channels, const std::span<const u8> colorMap)
    {
        if(width == 0u || height == 0u || channels == 0u || colorMap.empty())
        {
            TRAP_ASSERT(false, "TRAP::Utils::DecodeBGRAMappedPixelData(): One or more parameters are 0!");
            return {};
        }
        if(channels > 4u)
        {
            TRAP_ASSERT(false, "TRAP::Utils::DecodeBGRAMappedPixelData(): Channel count must be between 1 and 4!");
            return {};
        }
        if(source.size() != (NumericCast<usize>(width) * height))
        {
            TRAP_ASSERT(false, "TRAP::Utils::DecodeBGRAMappedPixelData: Raw pixel data size mismatches width * height!");
            return {};
        }

        std::vector<u8> data(NumericCast<usize>(width) * height * channels);

        for (u32 baseDataIndex = 0u, newDataindex = 0u; baseDataIndex < (width * height); ++baseDataIndex)
        {
            if (channels == 1u)
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels];
            else if (channels == 2u)
            {
                data[newDataindex++] = NumericCast<u8>(colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 1u] << 1u) & 0xF8u;
                data[newDataindex++] = NumericCast<u8>(NumericCast<u8>(colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 1u] << 6u) |
                                                       NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels] >> 2u)) & 0xF8u;
                data[newDataindex++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels] << 3u) & 0xF8u;
            }
            else if (channels == 3u)
            {
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 2u];
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 1u];
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 0u];
            }
            else if (channels == 4u)
            {
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 2u];
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 1u];
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 0u];
                data[newDataindex++] = colorMap[(NumericCast<usize>(source[baseDataIndex]) * channels) + 3u];
            }
        }

        return data;
    }
}

#endif /*TRAP_IMAGEUTILS_H*/
