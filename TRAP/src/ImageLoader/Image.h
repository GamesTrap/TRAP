#ifndef TRAP_IMAGE_H
#define TRAP_IMAGE_H

#include <algorithm>
#include <vector>

#include "Maths/Math.h"

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
		Image() noexcept;
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
		[[nodiscard]] virtual const void* GetPixelData() const noexcept = 0;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] virtual uint64_t GetPixelDataSize() const noexcept = 0;
		/// <summary>
		/// Retrieve the amount of bits used for a single pixel in the image.
		/// </summary>
		/// <returns>Amount of bits.</returns>
		[[nodiscard]] uint32_t GetBitsPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bytes used for a single pixel in the image.
		/// </summary>
		/// <returns>Amount of bytes.</returns>
		[[nodiscard]] uint32_t GetBytesPerPixel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bits used for a single channel of a pixel in the image.
		/// </summary>
		/// <returns>Amount of bits.</returns>
		[[nodiscard]] uint32_t GetBitsPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the amount of bytes used for a single channel of a pixel in the image.
		/// </summary>
		/// <returns>Amount of bytes.</returns>
		[[nodiscard]] uint32_t GetBytesPerChannel() const noexcept;
		/// <summary>
		/// Retrieve the width of the image.
		/// </summary>
		/// <returns>Width of the image.</returns>
		[[nodiscard]] uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the image.
		/// </summary>
		/// <returns>Height of the image.</returns>
		[[nodiscard]] uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the size of the image.
		/// </summary>
		/// <returns>Size of the image as a Math::Vec2ui.</returns>
		[[nodiscard]] Math::Vec2ui GetSize() const noexcept;
		/// <summary>
		/// Retrieve whether the image has an alpha channel or not.
		/// </summary>
		/// <returns>True if image has an alpha channel, false otherwise.</returns>
		[[nodiscard]] bool HasAlphaChannel() const noexcept;
		/// <summary>
		/// Retrieve whether the image is gray scale or not.
		/// </summary>
		/// <returns>True if image is gray scale, false otherwise.</returns>
		[[nodiscard]] bool IsImageGrayScale() const noexcept;
		/// <summary>
		/// Retrieve whether the image is colored or not.
		/// </summary>
		/// <returns>True if image is colored, false otherwise.</returns>
		[[nodiscard]] bool IsImageColored() const noexcept;
		/// <summary>
		/// Retrieve whether the image is HDR(High Dynamic Range) or not.
		/// </summary>
		/// <returns>True if image is HDR, false otherwise.</returns>
		[[nodiscard]] bool IsHDR() const noexcept;
		/// <summary>
		/// Retrieve whether the image is LDR(Low Dynamic Range) or not.
		/// </summary>
		/// <returns>True if image is LDR, false otherwise.</returns>
		[[nodiscard]] bool IsLDR() const noexcept;
		/// <summary>
		/// Retrieve the file path of the image.
		/// </summary>
		/// <returns>Path to the image file, or empty string for custom images.</returns>
		[[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept;
		/// <summary>
		/// Retrieve the color format used by the image.
		/// </summary>
		/// <returns>Color format of the image.</returns>
		[[nodiscard]] ColorFormat GetColorFormat() const noexcept;

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
		[[nodiscard]] static std::vector<T> ConvertRGBAToRGB(uint32_t width, uint32_t height, ColorFormat format, const T* data);

		/// <summary>
		/// Converts BGR16 pixel data to RGB24.
		/// </summary>
		/// <param name="source">BGR16 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB24 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertBGR16ToRGB24(std::vector<uint8_t>& source,
		                                                              uint32_t width, uint32_t height);
		/// <summary>
		/// Converts BGR24 pixel data to RGB24.
		/// </summary>
		/// <param name="source">BGR24 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB24 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source,
		                                                              uint32_t width, uint32_t height);
		/// <summary>
		/// Converts BGR32 pixel data to RGB32.
		/// </summary>
		/// <param name="source">BGR32 pixel data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>RGB32 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source,
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
		[[nodiscard]] static std::vector<uint8_t> DecodeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height,
		                                                        uint32_t channels, std::vector<uint8_t>& colorMap);

		uint32_t m_width;
		uint32_t m_height;
		bool m_isHDR;
		ColorFormat m_colorFormat;
		std::filesystem::path m_filepath;
		uint32_t m_bitsPerPixel;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] std::vector<T> TRAP::Image::FlipX(const uint32_t width, const uint32_t height, const ColorFormat format,
                                                const T* data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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
	const uint32_t multiplier = ToUnderlying(format);

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
[[nodiscard]] std::vector<T> TRAP::Image::FlipY(const uint32_t width, const uint32_t height, const ColorFormat format,
                                                const T* data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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
	const uint32_t multiplier = ToUnderlying(format);

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
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90Clockwise(const uint32_t width, const uint32_t height,
                                                            const ColorFormat format, const T* const data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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

	std::vector<T> rotated(width * height * ToUnderlying(format));

	for(uint32_t y = 0, destCol = height - 1; y < height; ++y, --destCol)
	{
		const uint32_t offset = y * width;

		for(uint32_t x = 0; x < width; ++x)
		{
			for(uint32_t channel = 0; channel < ToUnderlying(format); ++channel)
			{
				rotated[(x * height + destCol) * ToUnderlying(format) + channel] =
				data[(offset + x) * ToUnderlying(format) + channel];
			}
		}
	}

	return rotated;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::vector<T> TRAP::Image::Rotate90CounterClockwise(const uint32_t width, const uint32_t height,
                                                                   const ColorFormat format, const T* const data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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

	std::vector<T> rotated(NumericCast<std::size_t>(width) * height * ToUnderlying(format));
	std::copy_n(data, rotated.size(), rotated.begin());
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
			for(uint32_t channel = 0; channel < ToUnderlying(format); ++channel)
			{
				std::swap(rotated[(x * height + y) * ToUnderlying(format) + channel],
						  rotated[(I * width + J) * ToUnderlying(format) + channel]);
			}
		}
	}

	return rotated;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBToRGBA(const uint32_t width, const uint32_t height, const ColorFormat format,
									                       const T* data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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
	if constexpr (std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value)
		whitePixelColor = std::numeric_limits<T>::max();
	else
		whitePixelColor = 1.0f;

	std::vector<T> newData(width * height * ToUnderlying(ColorFormat::RGBA));
	std::size_t newDataIndex = 0;
	for(std::size_t oldDataIndex = 0; oldDataIndex < NumericCast<std::size_t>(width) * height * ToUnderlying(ColorFormat::RGB);
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
[[nodiscard]] std::vector<T> TRAP::Image::ConvertRGBAToRGB(const uint32_t width, const uint32_t height, const ColorFormat format,
									                       const T* data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

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

	std::vector<T> newData(width * height * ToUnderlying(ColorFormat::RGB));
	std::size_t newDataIndex = 0;
	for(std::size_t oldDataIndex = 0; oldDataIndex < NumericCast<std::size_t>(width) * height * ToUnderlying(ColorFormat::RGBA);
		oldDataIndex += 4)
	{
		newData[newDataIndex + 0] = data[oldDataIndex + 0];
		newData[newDataIndex + 1] = data[oldDataIndex + 1];
		newData[newDataIndex + 2] = data[oldDataIndex + 2];
		newDataIndex += 3;
	}

	return newData;
}

#endif /*TRAP_IMAGE_H*/