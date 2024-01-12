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
			NONE = 0,
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

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] virtual constexpr const void* GetPixelData() const noexcept = 0;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] virtual constexpr u64 GetPixelDataSize() const noexcept = 0;
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
		/// @brief Retrieve the size of the image.
		/// @return Size of the image as a Math::Vec2ui.
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
		[[nodiscard]] static Scope<Image> FlipX(const Image* img);
		/// @brief Flip an image on its Y axis.
		/// @param img Image to flip.
		/// @return Flipped image.
		[[nodiscard]] static Scope<Image> FlipY(const Image* img);
		/// @brief Rotate an image by 90 degrees clockwise.
		/// @param img Image to flip.
		/// @return Rotated image.
		[[nodiscard]] static Scope<Image> Rotate90Clockwise(const Image* img);
		/// @brief Rotate an image by 90 degrees counter clockwise.
		/// @param img Image to flip.
		/// @return Rotated image.
		[[nodiscard]] static Scope<Image> Rotate90CounterClockwise(const Image* img);
		/// @brief Convert a RGB image to RGBA.
		/// @param img Image to convert.
		/// @return Converted image.
		[[nodiscard]] static Scope<Image> ConvertRGBToRGBA(const Image* img);

		static constexpr std::array<std::string_view, 15> SupportedImageFormatSuffixes
		{
			".pgm", ".ppm", ".pnm", ".pam", ".pfm",
			".tga", ".icb", ".vda", ".vst",
			".bmp", ".dib",
			".png",
			".hdr", ".pic",
			".qoi"
		};

	protected:
		/// @brief Flip raw pixel data on X axis.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Flipped raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> FlipX(u32 width, u32 height, ColorFormat format, const T* data);
		/// @brief Flip raw pixel data on Y axis.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Flipped raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> FlipY(u32 width, u32 height, ColorFormat format, const T* data);
		/// @brief Rotate raw pixel data by 90 degrees clockwise.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Rotated raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> Rotate90Clockwise(u32 width, u32 height, ColorFormat format, const T* data);
		/// @brief Rotate raw pixel data by 90 degrees counter clockwise.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Rotate raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> Rotate90CounterClockwise(u32 width, u32 height, ColorFormat format, const T* data);
		/// @brief Converts raw RGB pixel data to RGBA.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Converted RGBA raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> ConvertRGBToRGBA(u32 width, u32 height, ColorFormat format, const T* data);
		/// @brief Converts raw RGBA pixel data to RGB.
		/// @tparam T u8, u16 or f32.
		/// @param width Width of image in pixels.
		/// @param height Height of image in pixels.
		/// @param format Color format of the image data.
		/// @param data Raw pixel data.
		/// @return Converted RGB raw pixel data
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		[[nodiscard]] static std::vector<T> ConvertRGBAToRGB(u32 width, u32 height, ColorFormat format, const T* data);

		/// @brief Converts BGR16 pixel data to RGB24.
		/// @param source BGR16 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGR16ToRGB24(std::vector<u8>& source,
		                                                                        u32 width, u32 height);
		/// @brief Converts BGR24 pixel data to RGB24.
		/// @param source BGR24 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGR24ToRGB24(std::vector<u8>& source,
		                                                                        u32 width, u32 height);
		/// @brief Converts BGR32 pixel data to RGB32.
		/// @param source BGR32 pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return RGB32 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertBGRA32ToRGBA32(std::vector<u8>& source,
		                                                                          u32 width, u32 height);
		/// @brief Decode BGRA indexed pixel data to RGBA.
		/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
		/// @param source Indexed BGRA pixel data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB.
		/// @param colorMap Color table.
		/// @return Decoded pixel data.
		[[nodiscard]] constexpr static std::vector<u8> DecodeBGRAMap(std::vector<u8>& source, u32 width, u32 height,
		                                                                  u32 channels, std::vector<u8>& colorMap);

		u32 m_width = 0;
		u32 m_height = 0;
		bool m_isHDR = false;
		ColorFormat m_colorFormat = ColorFormat::NONE;
		std::filesystem::path m_filepath{};
		u32 m_bitsPerPixel = 0;
	};
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
[[nodiscard]] std::vector<T> TRAP::Image::FlipX(const u32 width, const u32 height, const ColorFormat format,
                                                const T* data)
{
	if (format == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "Image::FlipX(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::FlipX(): Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	std::vector<T> newData{};
	u32 stride = 0;
	const u32 multiplier = std::to_underlying(format);

	newData.assign(data, data + NumericCast<u64>(width) * height * multiplier);
	stride = height * multiplier;

	std::vector<T> row{};
	row.resize(stride);

	for (u32 lowOffset = 0, highOffset = (width - 1) * stride; lowOffset < highOffset;
	     lowOffset += stride, highOffset -= stride)
	{
		std::copy(newData.data() + lowOffset, newData.data() + lowOffset + stride, row.data());
		std::copy(newData.data() + highOffset, newData.data() + highOffset + stride, newData.data() + lowOffset);
		std::copy(row.data(), row.data() + stride, newData.data() + highOffset);
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::vector<T> TRAP::Image::FlipY(const u32 width, const u32 height, const ColorFormat format,
                                                const T* data)
{
	if (format == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "Image::FlipY(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::FlipY(): Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	std::vector<T> newData{};
	u32 stride = 0;
	const u32 multiplier = std::to_underlying(format);

	newData.assign(data, data + NumericCast<u64>(width) * height * multiplier);
	stride = width * multiplier;

	std::vector<T> row{};
	row.resize(stride);

	for (u32 lowOffset = 0, highOffset = (height - 1) * stride; lowOffset < highOffset;
	     lowOffset += stride, highOffset -= stride)
	{
		std::copy(newData.data() + lowOffset, newData.data() + lowOffset + stride, row.data());
		std::copy(newData.data() + highOffset, newData.data() + highOffset + stride, newData.data() + lowOffset);
		std::copy(row.data(), row.data() + stride, newData.data() + highOffset);
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90Clockwise(const u32 width, const u32 height,
                                                            const ColorFormat format, const T* const data)
{
	if (format == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "Image::Rotate90Clockwise(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::Rotate90Clockwise: Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	std::vector<T> rotated(width * height * std::to_underlying(format));

	for(u32 y = 0, destCol = height - 1; y < height; ++y, --destCol)
	{
		const u32 offset = y * width;

		for(u32 x = 0; x < width; ++x)
		{
			for(u32 channel = 0; channel < std::to_underlying(format); ++channel)
			{
				rotated[(x * height + destCol) * std::to_underlying(format) + channel] =
				data[(offset + x) * std::to_underlying(format) + channel];
			}
		}
	}

	return rotated;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90CounterClockwise(const u32 width, const u32 height,
                                                                   const ColorFormat format, const T* const data)
{
	if (format == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "Image::Rotate90CounterClockwise(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::Rotate90CounterClockwise: Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	std::vector<T> rotated(NumericCast<usize>(width) * height * std::to_underlying(format));
	std::ranges::copy_n(data, static_cast<std::iter_difference_t<decltype(data)>>(rotated.size()), rotated.begin());
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
			for(u32 channel = 0; channel < std::to_underlying(format); ++channel)
			{
				std::swap(rotated[(x * height + y) * std::to_underlying(format) + channel],
						  rotated[(I * width + J) * std::to_underlying(format) + channel]);
			}
		}
	}

	return rotated;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBToRGBA(const u32 width, const u32 height, const ColorFormat format,
									                       const T* data)
{
	if(format != ColorFormat::RGB)
	{
		TRAP_ASSERT(false, "Image::ConvertRGBToRGBA(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::ConvertRGBToRGBA(): Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	T whitePixelColor;
	if constexpr (std::same_as<T, u8> || std::same_as<T, u16>)
		whitePixelColor = std::numeric_limits<T>::max();
	else
		whitePixelColor = 1.0f;

	std::vector<T> newData(width * height * std::to_underlying(ColorFormat::RGBA));
	usize newDataIndex = 0;
	for(usize oldDataIndex = 0; oldDataIndex < NumericCast<usize>(width) * height * std::to_underlying(ColorFormat::RGB);
		oldDataIndex += 3)
	{
		newData[newDataIndex + 0] = data[oldDataIndex + 0];
		newData[newDataIndex + 1] = data[oldDataIndex + 1];
		newData[newDataIndex + 2] = data[oldDataIndex + 2];
		newData[newDataIndex + 3] = whitePixelColor;
		newDataIndex += 4;
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBAToRGB(const u32 width, const u32 height, const ColorFormat format,
									                       const T* data)
{
	if(format != ColorFormat::RGBA)
	{
		TRAP_ASSERT(false, "Image::ConvertRGBAToRGB(): Invalid color format!");
		return std::vector<T>();
	}
	if(!data)
	{
		TRAP_ASSERT(false, "Image::ConvertRGBAToRGB(): Raw pixel data is nullptr!");
		return std::vector<T>();
	}

	std::vector<T> newData(width * height * std::to_underlying(ColorFormat::RGB));
	usize newDataIndex = 0;
	for(usize oldDataIndex = 0; oldDataIndex < NumericCast<usize>(width) * height * std::to_underlying(ColorFormat::RGBA);
		oldDataIndex += 4)
	{
		newData[newDataIndex + 0] = data[oldDataIndex + 0];
		newData[newDataIndex + 1] = data[oldDataIndex + 1];
		newData[newDataIndex + 2] = data[oldDataIndex + 2];
		newDataIndex += 3;
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGR16ToRGB24(std::vector<u8>& source, const u32 width, const u32 height)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * 3);

	u32 index = 0;
	for (u32 i = 0; i < width * height * 2; i += 2)
	{
		data[index++] = NumericCast<u8>(source[i + 1u] << 1u) & 0xF8u;
		data[index++] = NumericCast<u8>(NumericCast<u8>((source[i + 1u]) << 6u) | NumericCast<u8>(source[i] >> 2u)) & 0xF8u;
		data[index++] = NumericCast<u8>(source[i] << 3u) & 0xF8u;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::DecodeBGRAMap(std::vector<u8>& source, const u32 width, const u32 height, const u32 channels, std::vector<u8>& colorMap)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * channels);

	u32 index = 0;
	for (u32 i = 0; i < width * height; i++)
	{
		if (channels == 1)
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels];
		else if (channels == 2)
		{
			data[index++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels + 1u] << 1u) & 0xF8u;
			data[index++] = NumericCast<u8>(NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels + 1u] << 6u) | NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] >> 2u)) & 0xF8u;
			data[index++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] << 3u) & 0xF8u;
		}
		else if (channels == 3)
		{
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 0];
		}
		else if (channels == 4)
		{
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 0];
			data[index++] = colorMap[NumericCast<usize>(source[i]) * channels + 3];
		}
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGR24ToRGB24(std::vector<u8>& source, const u32 width, const u32 height)
{
	for (u32 i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::Image::ConvertBGRA32ToRGBA32(std::vector<u8>& source, const u32 width, const u32 height)
{
	for (u32 i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

#endif /*TRAP_IMAGE_H*/