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
		enum class ColorFormat : u8
		{
			GrayScale = 1u,
			GrayScaleAlpha = 2u,
			RGB = 3u,
			RGBA = 4u
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
		///     - Quite OK Image: QOI
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
		/// @return Converted image on success, nullptr otherwise.
		[[nodiscard]] static Scope<Image> ConvertRGBToRGBA(const Image& img);

	protected:
		u32 m_width = 0u;
		u32 m_height = 0u;
		bool m_isHDR = false;
		ColorFormat m_colorFormat = ColorFormat::RGBA;
		std::filesystem::path m_filepath{};
		u32 m_bitsPerPixel = 0u;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetChannelsPerPixel() const noexcept
{
	switch(m_colorFormat)
	{
	case TRAP::Image::ColorFormat::GrayScale:
		return 1u;

	case TRAP::Image::ColorFormat::GrayScaleAlpha:
		return 2u;

	case TRAP::Image::ColorFormat::RGB:
		return 3u;

	case TRAP::Image::ColorFormat::RGBA:
		return 4u;
	}

	TRAP_ASSERT(false, "GetChannelsPerPixel(): Unknown TRAP::Image::ColorFormat value!");
	return 0u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBitsPerPixel() const noexcept
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBytesPerPixel() const noexcept
{
	return m_bitsPerPixel / 8u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBitsPerChannel() const noexcept
{
	return m_bitsPerPixel / GetChannelsPerPixel();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Image::GetBytesPerChannel() const noexcept
{
	return GetBytesPerPixel() / GetChannelsPerPixel();
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

#endif /*TRAP_IMAGE_H*/
