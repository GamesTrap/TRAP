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
	/// <summary>
	/// Abstract image base class.
	/// </summary>
	class Image
	{
	public:
		/// <summary>
		/// Color formats.
		/// </summary>
		enum class ColorFormat : uint32_t
		{
			NONE = 0,
			GrayScale = 1,
			GrayScaleAlpha = 2,
			RGB = 3,
			RGBA = 4
		};

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit Image(std::filesystem::path filepath);
		/// <summary>
		/// Constructor.
		/// </summary>
		Image(std::filesystem::path filepath, uint32_t width, uint32_t height, ColorFormat format);
	public:
		/// <summary>
		/// Copy constructor.
		/// </summary>
		Image(const Image&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Image& operator=(const Image&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Image(Image&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Image& operator=(Image&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Image() = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] virtual constexpr const void* GetPixelData() const noexcept = 0;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] virtual constexpr uint64_t GetPixelDataSize() const noexcept = 0;
		/// <summary>
		/// Retrieve the amount of bits used for a single pixel in the image.
		/// </summary>
		/// <returns>Amount of bits.</returns>
		[[nodiscard]] constexpr uint32_t GetBitsPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bytes used for a single pixel in the image.
		/// </summary>
		/// <returns>Amount of bytes.</returns>
		[[nodiscard]] constexpr uint32_t GetBytesPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bits used for a single channel of a pixel in the image.
		/// </summary>
		/// <returns>Amount of bits.</returns>
		[[nodiscard]] constexpr uint32_t GetBitsPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bytes used for a single channel of a pixel in the image.
		/// </summary>
		/// <returns>Amount of bytes.</returns>
		[[nodiscard]] constexpr uint32_t GetBytesPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the width of the image.
		/// </summary>
		/// <returns>Width of the image.</returns>
		[[nodiscard]] constexpr uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the image.
		/// </summary>
		/// <returns>Height of the image.</returns>
		[[nodiscard]] constexpr uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the size of the image.
		/// </summary>
		/// <returns>Size of the image as a Math::Vec2ui.</returns>
		[[nodiscard]] constexpr Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Retrieve whether the image has an alpha channel or not.
		/// </summary>
		/// <returns>True if image has an alpha channel, false otherwise.</returns>
		[[nodiscard]] constexpr bool HasAlphaChannel() const noexcept;
		/// <summary>
		/// Retrieve whether the image is gray scale or not.
		/// </summary>
		/// <returns>True if image is gray scale, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsImageGrayScale() const noexcept;
		/// <summary>
		/// Retrieve whether the image is colored or not.
		/// </summary>
		/// <returns>True if image is colored, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsImageColored() const noexcept;
		/// <summary>
		/// Retrieve whether the image is HDR(High Dynamic Range) or not.
		/// </summary>
		/// <returns>True if image is HDR, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsHDR() const noexcept;
		/// <summary>
		/// Retrieve whether the image is LDR(Low Dynamic Range) or not.
		/// </summary>
		/// <returns>True if image is LDR, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsLDR() const noexcept;
		/// <summary>
		/// Retrieve the file path of the image.
		/// </summary>
		/// <returns>Path to the image file, or empty string for custom images.</returns>
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// <summary>
		/// Retrieve the color format used by the image.
		/// </summary>
		/// <returns>Color format of the image.</returns>
		[[nodiscard]] constexpr ColorFormat GetColorFormat() const noexcept;

		/// <summary>
		/// Load an image from disk.
		/// </summary>
		/// <param name="filepath">
		/// Path to the image.
		/// Supported formats:
		///		- Portable Maps: PGM, PPM, PNM, PAM, PFM
		///		- Targa: TGA, ICB, VDA, VST
		///		- Bitmap: BMP, DIB
		///		- Portable Network Graphics: PNG
		///		- Radiance: HDR, PIC
		/// </param>
		/// <returns>Loaded image on success, fallback image otherwise.</returns>
		[[nodiscard]] static Scope<Image> LoadFromFile(const std::filesystem::path& filepath);
		/// <summary>
		/// Load an image from memory.
		/// </summary>
		/// <param name="width">Width for the image.</param>
		/// <param name="height">Height for the image</param>
		/// <param name="format">Color format for the image.</param>
		/// <param name="pixelData">Raw pixel data for the image</param>
		/// <returns>
		/// Loaded image.
		/// Note: There are no validation checks for images loaded from memory!
		/// </returns>
		[[nodiscard]] static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format,
		                                                 const std::vector<uint8_t>& pixelData);
		/// <summary>
		/// Load an image from memory.
		/// </summary>
		/// <param name="width">Width for the image.</param>
		/// <param name="height">Height for the image</param>
		/// <param name="format">Color format for the image.</param>
		/// <param name="pixelData">Raw pixel data for the image</param>
		/// <returns>
		/// Loaded Image.
		/// Note: There are no validation checks for images loaded from memory!
		/// </returns>
		[[nodiscard]] static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format,
		                                                 const std::vector<uint16_t>& pixelData);
		/// <summary>
		/// Load an HDR image from memory.
		/// </summary>
		/// <param name="width">Width for the image.</param>
		/// <param name="height">Height for the image</param>
		/// <param name="format">Color format for the image.</param>
		/// <param name="pixelData">Raw pixel data for the image</param>
		/// <returns>
		/// Loaded Image.
		/// Note: There are no validation checks for images loaded from memory!
		/// </returns>
		[[nodiscard]] static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format,
		                                                 const std::vector<float>& pixelData);
		/// <summary>
		/// Load the fallback image.
		/// </summary>
		/// <returns>Fallback image.</returns>
		[[nodiscard]] static Scope<Image> LoadFallback();

		/// <summary>
		/// Check if the given file is a supported image.
		/// </summary>
		/// <param name="filepath">Path to a file</param>
		/// <returns>True if given file is an image, false otherwise.</returns>
		[[nodiscard]] static bool IsSupportedImageFile(const std::filesystem::path& filepath);

		/// <summary>
		/// Flip an image on its X axis.
		/// </summary>
		/// <param name="img">Image to flip.</param>
		/// <returns>Flipped image.</returns>
		[[nodiscard]] static Scope<Image> FlipX(const Image* img);
		/// <summary>
		/// Flip an image on its Y axis.
		/// </summary>
		/// <param name="img">Image to flip.</param>
		/// <returns>Flipped image.</returns>
		[[nodiscard]] static Scope<Image> FlipY(const Image* img);
		/// <summary>
		/// Rotate an image by 90 degrees clockwise.
		/// </summary>
		/// <param name="img">Image to flip.</param>
		/// <returns>Rotated image.</returns>
		[[nodiscard]] static Scope<Image> Rotate90Clockwise(const Image* img);
		/// <summary>
		/// Rotate an image by 90 degrees counter clockwise.
		/// </summary>
		/// <param name="img">Image to flip.</param>
		/// <returns>Rotated image.</returns>
		[[nodiscard]] static Scope<Image> Rotate90CounterClockwise(const Image* img);
		/// <summary>
		/// Convert a RGB image to RGBA.
		/// </summary>
		/// <param name="img">Image to convert.</param>
		/// <returns>Converted image.</returns>
		[[nodiscard]] static Scope<Image> ConvertRGBToRGBA(const Image* img);

		inline static constexpr std::array<std::string_view, 15> SupportedImageFormatSuffixes
		{
			".pgm", ".ppm", ".pnm", ".pam", ".pfm",
			".tga", ".icb", ".vda", ".vst",
			".bmp", ".dib",
			".png",
			".hdr", ".pic",
			".qoi"
		};

	protected:
		/// <summary>
		/// Flip raw pixel data on X axis.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Flipped raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> FlipX(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Flip raw pixel data on Y axis.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Flipped raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> FlipY(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Rotate raw pixel data by 90 degrees clockwise.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Rotated raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> Rotate90Clockwise(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Rotate raw pixel data by 90 degrees counter clockwise.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Rotate raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> Rotate90CounterClockwise(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Converts raw RGB pixel data to RGBA.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Converted RGBA raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> ConvertRGBToRGBA(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Converts raw RGBA pixel data to RGB.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of image in pixels.</param>
		/// <param name="height">Height of image in pixels.</param>
		/// <param name="format">Color format of the image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Converted RGB raw pixel data</returns>
		template<typename T>
		requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
		[[nodiscard]] static std::vector<T> ConvertRGBAToRGB(uint32_t width, uint32_t height, ColorFormat format, const T* data);

		/// <summary>
		/// Converts BGR16 pixel data to RGB24.
		/// </summary>
		/// <param name="source">BGR16 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB24 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertBGR16ToRGB24(std::vector<uint8_t>& source,
		                                                                        uint32_t width, uint32_t height);
		/// <summary>
		/// Converts BGR24 pixel data to RGB24.
		/// </summary>
		/// <param name="source">BGR24 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB24 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source,
		                                                                        uint32_t width, uint32_t height);
		/// <summary>
		/// Converts BGR32 pixel data to RGB32.
		/// </summary>
		/// <param name="source">BGR32 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB32 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source,
		                                                                          uint32_t width, uint32_t height);
		/// <summary>
		/// Decode BGRA indexed pixel data to RGBA.
		/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
		/// </summary>
		/// <param name="source">Indexed BGRA pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <param name="channels">Amount of channels, i.e. 4 = RGBA, 3 = RGB.</param>
		/// <param name="colorMap">Color table.</param>
		/// <returns>Decoded pixel data.</returns>
		[[nodiscard]] constexpr static std::vector<uint8_t> DecodeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height,
		                                                                  uint32_t channels, std::vector<uint8_t>& colorMap);

		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_isHDR = false;
		ColorFormat m_colorFormat = ColorFormat::NONE;
		std::filesystem::path m_filepath{};
		uint32_t m_bitsPerPixel = 0;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetBitsPerPixel() const noexcept
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetBytesPerPixel() const noexcept
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetBitsPerChannel() const noexcept
{
	return m_bitsPerPixel / std::to_underlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetBytesPerChannel() const noexcept
{
	return GetBytesPerPixel() / std::to_underlying(m_colorFormat);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Image::GetHeight() const noexcept
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::FlipX(const uint32_t width, const uint32_t height, const ColorFormat format,
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
	uint32_t stride = 0;
	const uint32_t multiplier = std::to_underlying(format);

	newData.assign(data, data + NumericCast<uint64_t>(width) * height * multiplier);
	stride = height * multiplier;

	std::vector<T> row{};
	row.resize(stride);

	for (uint32_t lowOffset = 0, highOffset = (width - 1) * stride; lowOffset < highOffset;
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::FlipY(const uint32_t width, const uint32_t height, const ColorFormat format,
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
	uint32_t stride = 0;
	const uint32_t multiplier = std::to_underlying(format);

	newData.assign(data, data + NumericCast<uint64_t>(width) * height * multiplier);
	stride = width * multiplier;

	std::vector<T> row{};
	row.resize(stride);

	for (uint32_t lowOffset = 0, highOffset = (height - 1) * stride; lowOffset < highOffset;
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90Clockwise(const uint32_t width, const uint32_t height,
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

	for(uint32_t y = 0, destCol = height - 1; y < height; ++y, --destCol)
	{
		const uint32_t offset = y * width;

		for(uint32_t x = 0; x < width; ++x)
		{
			for(uint32_t channel = 0; channel < std::to_underlying(format); ++channel)
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90CounterClockwise(const uint32_t width, const uint32_t height,
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

	std::vector<T> rotated(NumericCast<std::size_t>(width) * height * std::to_underlying(format));
	std::ranges::copy_n(data, static_cast<std::iter_difference_t<decltype(data)>>(rotated.size()), rotated.begin());
	for(uint32_t x = 0; x < width; ++x)
	{
		for(uint32_t y = 0; y < height; ++y)
		{
			uint32_t I = y;
			uint32_t J = width - 1 - x;
			while((x * height + y) > (I * width + J))
			{
				const uint32_t p = I * width + J;
				I = p % height;
				J = width - 1 - (p / height);
			}
			for(uint32_t channel = 0; channel < std::to_underlying(format); ++channel)
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBToRGBA(const uint32_t width, const uint32_t height, const ColorFormat format,
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
	if constexpr (std::same_as<T, uint8_t> || std::same_as<T, uint16_t>)
		whitePixelColor = std::numeric_limits<T>::max();
	else
		whitePixelColor = 1.0f;

	std::vector<T> newData(width * height * std::to_underlying(ColorFormat::RGBA));
	std::size_t newDataIndex = 0;
	for(std::size_t oldDataIndex = 0; oldDataIndex < NumericCast<std::size_t>(width) * height * std::to_underlying(ColorFormat::RGB);
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
requires std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, float>
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBAToRGB(const uint32_t width, const uint32_t height, const ColorFormat format,
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
	std::size_t newDataIndex = 0;
	for(std::size_t oldDataIndex = 0; oldDataIndex < NumericCast<std::size_t>(width) * height * std::to_underlying(ColorFormat::RGBA);
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

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::Image::ConvertBGR16ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height * 2; i += 2)
	{
		data[index++] = NumericCast<uint8_t>(source[i + 1u] << 1u) & 0xF8u;
		data[index++] = NumericCast<uint8_t>(NumericCast<uint8_t>((source[i + 1u]) << 6u) | NumericCast<uint8_t>(source[i] >> 2u)) & 0xF8u;
		data[index++] = NumericCast<uint8_t>(source[i] << 3u) & 0xF8u;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::Image::DecodeBGRAMap(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height, const uint32_t channels, std::vector<uint8_t>& colorMap)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0; i < width * height; i++)
	{
		if (channels == 1)
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels];
		else if (channels == 2)
		{
			data[index++] = NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels + 1u] << 1u) & 0xF8u;
			data[index++] = NumericCast<uint8_t>(NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels + 1u] << 6u) | NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels] >> 2u)) & 0xF8u;
			data[index++] = NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels] << 3u) & 0xF8u;
		}
		else if (channels == 3)
		{
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 0];
		}
		else if (channels == 4)
		{
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 2];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 1];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 0];
			data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels + 3];
		}
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::Image::ConvertBGR24ToRGB24(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	for (uint32_t i = 0; i < width * height * 3; i += 3)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::Image::ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, const uint32_t width, const uint32_t height)
{
	for (uint32_t i = 0; i < width * height * 4; i += 4)
		source[i] ^= source[i + 2] ^= source[i] ^= source[i + 2];

	return source;
}

#endif /*TRAP_IMAGE_H*/