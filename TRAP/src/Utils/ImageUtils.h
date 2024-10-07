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
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataX(): Raw pixel data size mismatches width * height * channels!");
            return std::vector<T>();
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
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::FlipPixelDataY(): Raw pixel data size mismatches width * height * channels!");
            return std::vector<T>();
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
        if(data.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90Clockwise: Raw pixel data size mismatches width * height * channels!");
            return std::vector<T>();
        }

        std::vector<T> rotated(data.size());

        for(u32 y = 0u, destCol = height - 1u; y < height; ++y, --destCol)
        {
            const u32 offset = y * width;

            for(u32 x = 0u; x < width; ++x)
            {
                for(u32 channel = 0u; channel < channels; ++channel)
                {
                    rotated[(x * height + destCol) * channels + channel] =
                        data[(offset + x) * channels + channel];
                }
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
        if(pixelData.size() != (width * height * channels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::RotatePixelData90CounterClockwise: Raw pixel data size mismatches width * height * channels!");
            return std::vector<T>();
        }

        std::vector<T> rotated(pixelData.begin(), pixelData.end());
        for(u32 x = 0; x < width; ++x)
        {
            for(u32 y = 0; y < height; ++y)
            {
                u32 I = y;
                u32 J = width - 1 - x;
                while((x * height + y) > (I * width + J))
                {
                    const u32 p = I * width + J;
                    I = p % height;
                    J = width - 1 - (p / height);
                }
                for(u32 channel = 0; channel < channels; ++channel)
                {
                    std::swap(rotated[(x * height + y) * channels + channel],
                            rotated[(I * width + J) * channels + channel]);
                }
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
        constexpr u32 RGBChannels = 3;
        constexpr u32 RGBAChannels = 4;

        if(data.size() < (width * height * RGBChannels))
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
        usize newDataIndex = 0;
        for(usize oldDataIndex = 0; oldDataIndex < (NumericCast<usize>(width) * height * RGBChannels);
            oldDataIndex += 3, newDataIndex += 4)
        {
            std::copy(data.begin() + oldDataIndex, data.begin() + oldDataIndex + 3, &newData[newDataIndex]); //Copy RGB
            newData[newDataIndex + 3] = whitePixelColor; //Add alpha channel
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
        constexpr u32 RGBChannels = 3;
        constexpr u32 RGBAChannels = 4;

        if(data.size() < (width * height * RGBAChannels))
        {
            TRAP_ASSERT(false, "TRAP::Utils::ConvertRGBAPixelDataToRGB(): Raw pixel data size mismatches width * height * 4!");
            return std::vector<T>();
        }

        std::vector<T> newData(width * height * RGBChannels);
        usize newDataIndex = 0;
        for(usize oldDataIndex = 0; oldDataIndex < (NumericCast<usize>(width) * height * RGBAChannels);
            oldDataIndex += 4, newDataIndex += 3)
        {
            std::copy(&data[oldDataIndex], &data[oldDataIndex + 3], &newData[newDataIndex]);
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
        constexpr u32 BRG16Channels = 2;
        constexpr u32 RGBChannels = 3;

        if(source.size() < (NumericCast<usize>(width) * height * BRG16Channels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGR16PixelDataToRGB24(): Raw pixel data size mismatches width * height * 2!");
            return std::vector<u8>();
        }

        std::vector<u8> data(NumericCast<usize>(width) * height * RGBChannels);

        for (u32 oldDataIndex = 0, newDataIndex = 0;
            oldDataIndex < (width * height * BRG16Channels);
            oldDataIndex += 2, newDataIndex += 3)
        {
            data[newDataIndex + 0] = NumericCast<u8>(source[oldDataIndex + 1u] << 1u) & 0xF8u;
            data[newDataIndex + 1] = NumericCast<u8>(NumericCast<u8>((source[oldDataIndex + 1u]) << 6u) | NumericCast<u8>(source[oldDataIndex] >> 2u)) & 0xF8u;
            data[newDataIndex + 2] = NumericCast<u8>(source[oldDataIndex] << 3u) & 0xF8u;
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
        constexpr u32 RGBChannels = 3;

        if(source.size() < (NumericCast<usize>(width) * height * RGBChannels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGR24PixelDataToRGB24(): Raw pixel data size mismatches width * height * 3!");
            return std::vector<u8>();
        }

        std::vector<u8> data(source.begin(), source.end());

        for (u32 i = 0; i < (width * height * RGBChannels); i += 3)
            data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2];

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
        constexpr u32 RGBAChannels = 4;

        if(source.size() < (NumericCast<usize>(width) * height * RGBAChannels))
        {
            TRAP_ASSERT(false, "TRAP::Image::ConvertBGRA32PixelDataToRGBA32(): Raw pixel data size mismatches width * height * 4!");
            return std::vector<u8>();
        }

        std::vector<u8> data(source.begin(), source.end());

        for (u32 i = 0; i < (width * height * RGBAChannels); i += 4)
            data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2];

        return data;
    }

    //-------------------------------------------------------------------------------------------------------------------//

    /// @brief Decode BGRA indexed pixel data to RGBA.
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
        std::vector<u8> data(NumericCast<usize>(width) * height * channels);

        for (u32 baseDataIndex = 0, newDataindex = 0; baseDataIndex < width * height; baseDataIndex++)
        {
            if (channels == 1)
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels];
            else if (channels == 2)
            {
                data[newDataindex++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 1u] << 1u) & 0xF8u;
                data[newDataindex++] = NumericCast<u8>(NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 1u] << 6u) | NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels] >> 2u)) & 0xF8u;
                data[newDataindex++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[baseDataIndex]) * channels] << 3u) & 0xF8u;
            }
            else if (channels == 3)
            {
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 2];
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 1];
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 0];
            }
            else if (channels == 4)
            {
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 2];
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 1];
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 0];
                data[newDataindex++] = colorMap[NumericCast<usize>(source[baseDataIndex]) * channels + 3];
            }
        }

        return data;
    }
}

#endif /*TRAP_IMAGEUTILS_H*/
