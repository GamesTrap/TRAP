#ifndef TRAP_IMAGE_H
#define TRAP_IMAGE_H

#include <algorithm>
#include <vector>
#include <filesystem>

#include "Core/Base.h"
#include "Utils/NumericCasts.h"

#include "Maths/Types.h"
#include "Maths/Vec2.h"

namespace TRAP
{
	/// @brief Abstract image base class.
	class Image
	{
	public:
		/// @brief Color formats.
		enum class ColorFormat : u32
		{
			GrayScale = 1,
			GrayScaleAlpha = 2,
			RGB = 3,
			RGBA = 4
		};

	protected:
		/// @brief Constructor.
		explicit Image(std::filesystem::path filepath);
		/// @brief Constructor.
		Image(std::filesystem::path filepath, u32 width, u32 height, ColorFormat format);
	public:
		/// @brief Copy constructor.
		Image(const Image&) noexcept = default;
		/// @brief Copy assignment operator.
		Image& operator=(const Image&) noexcept = default;
		/// @brief Move constructor.
		Image(Image&&) noexcept = default;
		/// @brief Move assignment operator.
		Image& operator=(Image&&) noexcept = default;
		/// @brief Destructor.
		virtual ~Image() = default;

		/// @brief Retrieve the raw pixel data of the image in bytes.
		/// @return Raw pixel data in bytes.
		[[nodiscard]] virtual constexpr std::span<const u8> GetPixelData() const noexcept = 0;
		/// @brief Retrieve the number of channels for a single pixel in the image.
		/// @return Number of chanels for a single pixel in the image.
		[[nodiscard]] constexpr u32 GetChannelsPerPixel() const noexcept;
		/// @brief Retrieve the amount of bits used for a single pixel in the image.
		/// @return Amount of bits.
		[[nodiscard]] constexpr u32 GetBitsPerPixel() const noexcept;
		/// @brief Retrieve the amount of bytes used for a single pixel in the image.
		/// @return Amount of bytes.
		[[nodiscard]] constexpr u32 GetBytesPerPixel() const noexcept;
		/// @brief Retrieve the amount of bits used for a single channel of a pixel in the image.
		/// @return Amount of bits.
		[[nodiscard]] constexpr u32 GetBitsPerChannel() const noexcept;
		/// @brief Retrieve the amount of bytes used for a single channel of a pixel in the image.
		/// @return Amount of bytes.
		[[nodiscard]] constexpr u32 GetBytesPerChannel() const noexcept;
		/// @brief Retrieve the width of the image.
		/// @return Width of the image.
		[[nodiscard]] constexpr u32 GetWidth() const noexcept;
		/// @brief Retrieve the height of the image.
		/// @return Height of the image.
		[[nodiscard]] constexpr u32 GetHeight() const noexcept;
		/// @brief Retrieve the pixel size of the image.
		/// @return Pixel size of the image as a Math::Vec2ui.
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// @brief Retrieve whether the image has an alpha channel or not.
		/// @return True if image has an alpha channel, false otherwise.
		[[nodiscard]] constexpr bool HasAlphaChannel() const noexcept;
		/// @brief Retrieve whether the image is gray scale or not.
		/// @return True if image is gray scale, false otherwise.
		[[nodiscard]] constexpr bool IsImageGrayScale() const noexcept;
		/// @brief Retrieve whether the image is colored or not.
		/// @return True if image is colored, false otherwise.
		[[nodiscard]] constexpr bool IsImageColored() const noexcept;
		/// @brief Retrieve whether the image is HDR(High Dynamic Range) or not.
		/// @return True if image is HDR, false otherwise.
		[[nodiscard]] constexpr bool IsHDR() const noexcept;
		/// @brief Retrieve whether the image is LDR(Low Dynamic Range) or not.
		/// @return True if image is LDR, false otherwise.
		[[nodiscard]] constexpr bool IsLDR() const noexcept;
		/// @brief Retrieve the file path of the image.
		/// @return Path to the image file, or empty string for custom images.
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// @brief Retrieve the color format used by the image.
		/// @return Color format of the image.
		[[nodiscard]] constexpr ColorFormat GetColorFormat() const noexcept;

		/// @brief Load an image from disk.
		/// @param filepath Path to the image.
		/// Supported formats:
		///		- Portable Maps: PGM, PPM, PNM, PAM, PFM
		///		- Targa: TGA, ICB, VDA, VST
		///		- Bitmap: BMP, DIB
		///		- Portable Network Graphics: PNG
		///		- Radiance: HDR, PIC
		/// @return Loaded image on success, fallback image otherwise.
		[[nodiscard]] static Scope<Image> LoadFromFile(const std::filesystem::path& filepath);
		/// @brief Load an image from memory.
		/// @param width Width for the image.
		/// @param height Height for the image
		/// @param format Color format for the image.
		/// @param pixelData Raw pixel data for the image
		/// @return Loaded image.
		/// @note There are no validation checks for images loaded from memory!
		[[nodiscard]] static Scope<Image> LoadFromMemory(u32 width, u32 height, ColorFormat format,
		                                                 const std::vector<u8>& pixelData);
		/// @brief Load an image from memory.
		/// @param width Width for the image.
		/// @param height Height for the image
		/// @param format Color format for the image.
		/// @param pixelData Raw pixel data for the image
		/// @return Loaded Image.
		/// @note There are no validation checks for images loaded from memory!
		[[nodiscard]] static Scope<Image> LoadFromMemory(u32 width, u32 height, ColorFormat format,
		                                                 const std::vector<u16>& pixelData);
		/// @brief Load an HDR image from memory.
		/// @param width Width for the image.
		/// @param height Height for the image
		/// @param format Color format for the image.
		/// @param pixelData Raw pixel data for the image
		/// @return Loaded Image.
		/// @note There are no validation checks for images loaded from memory!
		[[nodiscard]] static Scope<Image> LoadFromMemory(u32 width, u32 height, ColorFormat format,
		                                                 const std::vector<f32>& pixelData);
		/// @brief Load the fallback image.
		/// @return Fallback image.
		[[nodiscard]] static Scope<Image> LoadFallback();

		/// @brief Check if the given file is a supported image.
		/// @param filepath Path to a file
		/// @return True if given file is an image, false otherwise.
		[[nodiscard]] static bool IsSupportedImageFile(const std::filesystem::path& filepath);

		/// @brief Flip an image on its X axis.
		/// @param img Image to flip.
		/// @return Flipped image.
		[[nodiscard]] static Scope<Image> FlipX(const Image& img);
		/// @brief Flip an image on its Y axis.
		/// @param img Image to flip.
		/// @return Flipped image.
		[[nodiscard]] static Scope<Image> FlipY(const Image& img);
		/// @brief Rotate an image by 90 degrees clockwise.
		/// @param img Image to flip.
		/// @return Rotated image.
		[[nodiscard]] static Scope<Image> Rotate90Clockwise(const Image& img);
		/// @brief Rotate an image by 90 degrees counter clockwise.
		/// @param img Image to flip.
		/// @return Rotated image.
		[[nodiscard]] static Scope<Image> Rotate90CounterClockwise(const Image& img);
		/// @brief Convert a RGB image to RGBA.
		/// @param img Image to convert.
		/// @return Converted image.
		[[nodiscard]] static Scope<Image> ConvertRGBToRGBA(const Image& img);

	protected:
		/// @brief Flips raw pixel data on X axis.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param channels Number of channels contained in the image.
		/// @param pixelData Raw pixel data.
		/// @return Flipped raw pixel data.
		template <typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> FlipPixelDataX(u32 width, u32 height, u32 channels,
															         std::span<const T> pixelData);
		/// @brief Flip raw pixel data on Y axis.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param channels Number of channels contained in the image.
		/// @param data Raw pixel data.
		/// @return Flipped raw pixel data
		template <typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> FlipPixelDataY(u32 width, u32 height, u32 channels,
															         std::span<const T> pixelData);
		/// @brief Rotate raw pixel data by 90 degrees clockwise.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param channels Number of channels contained in the image.
		/// @param data Raw pixel data.
		/// @return Rotated raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> RotatePixelData90Clockwise(u32 width, u32 height, u32 channels, std::span<const T> data);
		/// @brief Rotate raw pixel data by 90 degrees counter clockwise.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param channels Number of channels contained in the image.
		/// @param data Raw pixel data.
		/// @return Rotate raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> RotatePixelData90CounterClockwise(u32 width, u32 height, u32 channels, std::span<const T> data);
		/// @brief Converts raw RGB pixel data to RGBA.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param data Raw pixel data.
		/// @return Converted RGBA raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> ConvertRGBToRGBA(u32 width, u32 height, std::span<const T> data);
		/// @brief Converts raw RGBA pixel data to RGB.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param data Raw pixel data.
		/// @return Converted RGB raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static constexpr std::vector<T> ConvertRGBAToRGB(u32 width, u32 height, std::span<const T> data);

		/// @brief Converts BGR16 pixel data to RGB24.
		/// @param source BGR16 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGR16ToRGB24(std::span<const u8> source,
		                                                                   u32 width, u32 height);
		/// @brief Converts BGR24 pixel data to RGB24.
		/// @param source BGR24 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGR24ToRGB24(std::span<const u8> source,
		                                                                   u32 width, u32 height);
		/// @brief Converts BGR32 pixel data to RGB32.
		/// @param source BGR32 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB32 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGRA32ToRGBA32(std::span<const u8> source,
		                                                                     u32 width, u32 height);
		/// @brief Decode BGRA indexed pixel data to RGBA.
		/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
		/// @param source Indexed BGRA pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB.
		/// @param colorMap Color table.
		/// @return Decoded pixel data.
		[[nodiscard]] static constexpr std::vector<u8> DecodeBGRAMap(std::span<const u8> source, u32 width, u32 height,
		                                                             u32 channels, std::vector<u8>& colorMap);

		u32 m_width = 0;
		u32 m_height = 0;
		bool m_isHDR = false;
		ColorFormat m_colorFormat = ColorFormat::RGBA;
		std::filesystem::path m_filepath{};
		u32 m_bitsPerPixel = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetChannelsPerPixel() const noexcept
{
	switch(m_colorFormat)
	{
	case TRAP::Image::ColorFormat::GrayScale:
		return 1;

	case TRAP::Image::ColorFormat::GrayScaleAlpha:
		return 2;

	case TRAP::Image::ColorFormat::RGB:
		return 3;

	case TRAP::Image::ColorFormat::RGBA:
		return 4;
	}

	TRAP_ASSERT(false, "GetChannelsPerPixel(): Unknown TRAP::Image::ColorFormat value!");
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBitsPerPixel() const noexcept
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBytesPerPixel() const noexcept
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBitsPerChannel() const noexcept
{
	return m_bitsPerPixel / std::to_underlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBytesPerChannel() const noexcept
{
	return GetBytesPerPixel() / std::to_underlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Math::Vec2ui TRAP::Image::GetSize() const noexcept
{
	return Math::Vec2ui{ m_width, m_height };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Image::HasAlphaChannel() const noexcept
{
	return m_colorFormat == ColorFormat::GrayScaleAlpha || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Image::IsImageGrayScale() const noexcept
{
	return m_colorFormat == ColorFormat::GrayScale || m_colorFormat == ColorFormat::GrayScaleAlpha;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Image::IsImageColored() const noexcept
{
	return m_colorFormat == ColorFormat::RGB || m_colorFormat == ColorFormat::RGBA;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Image::IsHDR() const noexcept
{
	return m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Image::IsLDR() const noexcept
{
	return !m_isHDR;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Image::ColorFormat TRAP::Image::GetColorFormat() const noexcept
{
	return m_colorFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::FlipPixelDataX(const u32 width, const u32 height,
                                                                   const u32 channels,
																   const std::span<const T> pixelData)
{
	if(pixelData.size() != (width * height * channels))
	{
		TRAP_ASSERT(false, "TRAP::Image::FlipPixelDataX(): Raw pixel data size mismatches width * height * channels!");
		return std::vector<T>();
	}

	std::vector<T> newData(pixelData.begin(), pixelData.end());
	const u32 rowStride = height * channels;

	std::vector<T> tmpRow(rowStride);

	for (u32 lowOffset = 0, highOffset = (width - 1) * rowStride; lowOffset < highOffset;
		lowOffset += rowStride, highOffset -= rowStride)
	{
		std::copy(&newData[lowOffset], &newData[lowOffset + rowStride], tmpRow.begin()); //Copy low row to temporary row buffer
		std::copy(&newData[highOffset], &newData[highOffset + rowStride], &newData[lowOffset]); //Copy high row to low row destination
		std::ranges::copy(tmpRow, &newData[highOffset]); //Copy temporary row buffer (low row) to high row destination
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::FlipPixelDataY(const u32 width, const u32 height,
                                                                   const u32 channels,
														           const std::span<const T> pixelData)
{
	if(pixelData.size() != (width * height * channels))
	{
		TRAP_ASSERT(false, "TRAP::Image::FlipPixelDataY(): Raw pixel data size mismatches width * height * channels!");
		return std::vector<T>();
	}

	std::vector<T> newData(pixelData.begin(), pixelData.end());
	const u32 colStride = width * channels;

	std::vector<T> tmpCol(colStride);

	for (u32 lowOffset = 0, highOffset = (height - 1) * colStride; lowOffset < highOffset;
		lowOffset += colStride, highOffset -= colStride)
	{
		std::copy(&newData[lowOffset], &newData[lowOffset + colStride], tmpCol.begin()); //Copy low column to temporary column buffer
		std::copy(&newData[highOffset], &newData[highOffset + colStride], &newData[lowOffset]); //Copy high column to low column destination
		std::ranges::copy(tmpCol, &newData[highOffset]); //Copy temporary column buffer (low column) to high column destination
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::RotatePixelData90Clockwise(const u32 width, const u32 height,
                                                                               const u32 channels,
															                   const std::span<const T> data)
{
	if(data.size() != (width * height * channels))
	{
		TRAP_ASSERT(false, "TRAP::Image::RotatePixelData90Clockwise: Raw pixel data size mismatches width * height * channels!");
		return std::vector<T>();
	}

	std::vector<T> rotated(data.size());

	for(u32 y = 0, destCol = height - 1; y < height; ++y, --destCol)
	{
		const u32 offset = y * width;

		for(u32 x = 0; x < width; ++x)
		{
			for(u32 channel = 0; channel < channels; ++channel)
			{
				rotated[(x * height + destCol) * channels + channel] =
				    data[(offset + x) * channels + channel];
			}
		}
	}

	return rotated;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::RotatePixelData90CounterClockwise(const u32 width, const u32 height,
                                                                                      const u32 channels,
																                      const std::span<const T> pixelData)
{
	if(pixelData.size() != (width * height * channels))
	{
		TRAP_ASSERT(false, "TRAP::Image::RotatePixelData90CounterClockwise: Raw pixel data size mismatches width * height * channels!");
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

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::ConvertRGBToRGBA(const u32 width, const u32 height,
                                                                     const std::span<const T> data)
{
	constexpr u32 RGBChannels = 3;
	constexpr u32 RGBAChannels = 4;

	if(data.size() < (width * height * RGBChannels))
	{
		TRAP_ASSERT(false, "TRAP::Image::ConvertRGBToRGBA(): Raw pixel data size mismatches width * height * 3!");
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
		std::copy(&data[oldDataIndex], &data[oldDataIndex + 3], &newData[newDataIndex]); //Copy RGB
		newData[newDataIndex + 3] = whitePixelColor; //Add alpha channel
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] constexpr std::vector<T> TRAP::Image::ConvertRGBAToRGB(const u32 width, const u32 height,
									                                 const std::span<const T> data)
{
	constexpr u32 RGBChannels = 3;
	constexpr u32 RGBAChannels = 4;

	if(data.size() < (width * height * RGBAChannels))
	{
		TRAP_ASSERT(false, "TRAP::Image::ConvertRGBAToRGB(): Raw pixel data size mismatches width * height * 4!");
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

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGR16ToRGB24(const std::span<const u8> source,
                                                                         const u32 width, const u32 height)
{
	constexpr u32 BRG16Channels = 2;
	constexpr u32 RGBChannels = 3;

	if(source.size() < (NumericCast<usize>(width) * height * BRG16Channels))
	{
		TRAP_ASSERT(false, "TRAP::Image::ConvertBGR16ToRGB24(): Raw pixel data size mismatches width * height * 2!");
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

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGR24ToRGB24(const std::span<const u8> source,
                                                                         const u32 width, const u32 height)
{
	constexpr u32 RGBChannels = 3;

	if(source.size() < (NumericCast<usize>(width) * height * RGBChannels))
	{
		TRAP_ASSERT(false, "TRAP::Image::ConvertBGR24ToRGB24(): Raw pixel data size mismatches width * height * 3!");
		return std::vector<u8>();
	}

	std::vector<u8> data(source.begin(), source.end());

	for (u32 i = 0; i < (width * height * RGBChannels); i += 3)
		data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2];

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGRA32ToRGBA32(const std::span<const u8> source,
                                                                           const u32 width, const u32 height)
{
	constexpr u32 RGBAChannels = 4;

	if(source.size() < (NumericCast<usize>(width) * height * RGBAChannels))
	{
		TRAP_ASSERT(false, "TRAP::Image::ConvertBGRA32ToRGBA32(): Raw pixel data size mismatches width * height * 4!");
		return std::vector<u8>();
	}

	std::vector<u8> data(source.begin(), source.end());

	for (u32 i = 0; i < (width * height * RGBAChannels); i += 4)
		data[i] ^= data[i + 2] ^= data[i] ^= data[i + 2];

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::DecodeBGRAMap(const std::span<const u8> source,
                                                                   const u32 width, const u32 height,
																   const u32 channels, std::vector<u8>& colorMap)
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

#endif /*TRAP_IMAGE_H*/
