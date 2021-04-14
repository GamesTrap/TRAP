#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_

#include <vector>

#include "Maths/Vec2.h"

namespace TRAP
{
	/// <summary>
	/// Abstract Image base class.
	/// </summary>
	class Image
	{
	public:
		/// <summary>
		/// Color formats.
		/// </summary>
		enum class ColorFormat
		{
			NONE = 0,
			RGB = 1,
			RGBA = 2,
			GrayScale = 3,
			GrayScaleAlpha = 4
		};

	protected:
		/// <summary>
		/// Protected Constructor.
		/// </summary>
		Image();
	public:
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Image(const Image&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Image& operator=(const Image&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Image(Image&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Image& operator=(Image&&) = default;
		/// <summary>
		/// Virtual Default Destructor.
		/// </summary>
		virtual ~Image() = default;

		/// <summary>
		/// Retrieve the raw pixel data of the Image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		virtual const void* GetPixelData() const = 0;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the Image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		virtual uint64_t GetPixelDataSize() const = 0;
		/// <summary>
		/// Retrieve the amount of bits used for a single pixel in the Image.
		/// </summary>
		/// <returns>Amount of bits.</returns>
		uint32_t GetBitsPerPixel() const;
		/// <summary>
		/// Retrieve the amount of bytes used for a single pixel in the Image.
		/// </summary>
		/// <returns>Amount of bytes.</returns>
		uint32_t GetBytesPerPixel() const;
		/// <summary>
		/// Retrieve the width of the Image.
		/// </summary>
		/// <returns>Width of the Image.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the height of the Image.
		/// </summary>
		/// <returns>Height of the Image.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the size of the Image.
		/// </summary>
		/// <returns>Size of the Image as a Math::Vec2ui.</returns>
		Math::Vec2ui GetSize() const;
		/// <summary>
		/// Retrieve whether the Image has an alpha channel or not.
		/// </summary>
		/// <returns>True if Image has an alpha channel, false otherwise.</returns>
		bool HasAlphaChannel() const;
		/// <summary>
		/// Retrieve whether the Image is gray scale or not.
		/// </summary>
		/// <returns>True if Image is gray scale, false otherwise.</returns>
		bool IsImageGrayScale() const;
		/// <summary>
		/// Retrieve whether the Image is colored or not.
		/// </summary>
		/// <returns>True if Image is colored, false otherwise.</returns>
		bool IsImageColored() const;
		/// <summary>
		/// Retrieve whether the Image is HDR(High Dynamic Range) or not.
		/// </summary>
		/// <returns>True if Image is HDR, false otherwise.</returns>
		bool IsHDR() const;
		/// <summary>
		/// Retrieve the file path of the Image.
		/// </summary>
		/// <returns>Path to the Image file, or empty string for CustomImages.</returns>
		const std::string& GetFilePath() const;
		/// <summary>
		/// Retrieve the color format used by the Image.
		/// </summary>
		/// <returns>ColorFormat of the Image.</returns>
		ColorFormat GetColorFormat() const;

		/// <summary>
		/// Load an Image from disk.
		/// </summary>
		/// <param name="filepath">
		/// Path to the Image.<br>
		/// Supported Formats:<br>
		///		- Portable Maps: PGM, PPM, PNM, PAM, PFM<br>
		///		- Targa: TGA, ICB, VDA, VST<br>
		///		- Bitmap: BMP, DIB<br>
		///		- Portable Network Graphics: PNG<br>
		///		- Radiance: HDR, PIC
		/// </param>
		/// <returns>Loaded Image on success, fallback Image otherwise.</returns>
		static Scope<Image> LoadFromFile(const std::string& filepath);
		/// <summary>
		/// Load an Image from memory.
		/// </summary>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image</param>
		/// <param name="format">Color format for the Image.</param>
		/// <param name="pixelData">Raw pixel data for the Image</param>
		/// <returns>Loaded Image.<br>Note: There are no validation checks for Images loaded from memory!</returns>
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint8_t>& pixelData);
		/// <summary>
		/// Load an Image from memory.
		/// </summary>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image</param>
		/// <param name="format">Color format for the Image.</param>
		/// <param name="pixelData">Raw pixel data for the Image</param>
		/// <returns>Loaded Image.<br>Note: There are no validation checks for Images loaded from memory!</returns>
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<uint16_t>& pixelData);
		/// <summary>
		/// Load an HDR Image from memory.
		/// </summary>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image</param>
		/// <param name="format">Color format for the Image.</param>
		/// <param name="pixelData">Raw pixel data for the Image</param>
		/// <returns>Loaded Image.<br>Note: There are no validation checks for Images loaded from memory!</returns>
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, ColorFormat format, const std::vector<float>& pixelData);
		/// <summary>
		/// Load the Fallback Image.<br>
		/// </summary>
		/// <returns>Fallback Image.</returns>
		static Scope<Image> LoadFallback();

		/// <summary>
		/// Check if the given (valid) file is a supported Image.
		/// </summary>
		/// <param name="filepath">Path to a file</param>
		/// <returns>True if given file is an Image, false otherwise.</returns>
		static bool IsSupportedImageFile(const std::string& filepath);

		/// <summary>
		/// Flip an Image on its X axis.
		/// </summary>
		/// <param name="img">Image to flip</param>
		/// <returns>Flipped Image.</returns>
		static Scope<Image> FlipX(const Scope<Image>& img);
		/// <summary>
		/// Flip an Image on its Y axis.
		/// </summary>
		/// <param name="img">Image to flip</param>
		/// <returns>Flipped Image.</returns>
		static Scope<Image> FlipY(const Scope<Image>& img);
		
	protected:
		/// <summary>
		/// Flip raw pixel data on X axis.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width">Width of Image in pixels.</param>
		/// <param name="height">Height of Image in pixels.</param>
		/// <param name="format">Color format of the Image data.</param>
		/// <param name="data">Raw pixel data.</param>
		/// <returns>Flipped raw pixel data</returns>
		template<typename T>
		static std::vector<T> FlipX(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		/// <summary>
		/// Flip raw pixel data on Y axis.
		/// </summary>
		/// <typeparam name="T">uint8_t, uint16_t or float.</typeparam>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="format"></param>
		/// <param name="data"></param>
		/// <returns></returns>
		template<typename T>
		static std::vector<T> FlipY(uint32_t width, uint32_t height, ColorFormat format, const T* data);

		//Used by multiple Image formats
		static std::vector<uint8_t> ConvertBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> DecodeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);

		uint32_t m_width;
		uint32_t m_height;
		bool m_isHDR;
		ColorFormat m_colorFormat;
		std::string m_filepath;
		uint32_t m_bitsPerPixel;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Scope<TRAP::Image> TRAP::Image::FlipX(const Scope<Image>& img)
{
	if (img)
	{
		Scope<Image> result;
		
		if (img->IsHDR())
		{
			const std::vector<float> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const float*>(img->GetPixelData()));
			
			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}
		else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
			(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
		{
			const std::vector<uint16_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint16_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}
		else
		{
			const std::vector<uint8_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint8_t*>(img->GetPixelData()));
			
			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}

		return result;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Scope<TRAP::Image> TRAP::Image::FlipY(const Scope<Image>& img)
{
	if (img)
	{
		Scope<Image> result;

		if (img->IsHDR())
		{
			const std::vector<float> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const float*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}
		else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
			(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
		{
			const std::vector<uint16_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint16_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}
		else
		{
			const std::vector<uint8_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint8_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), flipped);
		}

		return result;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
std::vector<T> TRAP::Image::FlipX(const uint32_t width, const uint32_t height, const ColorFormat format, const T* data)
{
	if (!(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, float>::value))
	{
		TRAP_CORE_ASSERT(false, "Invalid type!");
		return std::vector<T>();
	}
	if (format == ColorFormat::NONE || !data)
	{
		TRAP_CORE_ASSERT(false, "Invalid ColorFormat!");
		return std::vector<T>();
	}

	std::vector<T> newData{};
	uint32_t stride = 0;
	if (format == ColorFormat::GrayScale)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height));
		stride = height;
	}
	else if (format == ColorFormat::GrayScaleAlpha)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 2);
		stride = height * 2;
	}
	else if (format == ColorFormat::RGB)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 3);
		stride = height * 3;
	}
	else if (format == ColorFormat::RGBA)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 4);
		stride = height * 4;
	}

	std::vector<T> row{};
	row.resize(stride);

	for (uint32_t lowOffset = 0, highOffset = (width - 1) * stride; lowOffset < highOffset; lowOffset += stride, highOffset -= stride)
	{
		std::copy(newData.data() + lowOffset, newData.data() + lowOffset + stride, row.data());
		std::copy(newData.data() + highOffset, newData.data() + highOffset + stride, newData.data() + lowOffset);
		std::copy(row.data(), row.data() + stride, newData.data() + highOffset);
	}

	return newData;
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
std::vector<T> TRAP::Image::FlipY(const uint32_t width, const uint32_t height, const ColorFormat format, const T* data)
{
	if (!(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, float>::value))
	{
		TRAP_CORE_ASSERT(false, "Invalid type!");
		return std::vector<T>();
	}
	if (format == ColorFormat::NONE || !data)
	{
		TRAP_CORE_ASSERT(false, "Invalid ColorFormat!");
		return std::vector<T>();
	}

	std::vector<T> newData{};
	uint32_t stride = 0;
	if (format == ColorFormat::GrayScale)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height));
		stride = width;
	}
	else if (format == ColorFormat::GrayScaleAlpha)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 2);
		stride = width * 2;
	}
	else if (format == ColorFormat::RGB)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 3);
		stride = width * 3;
	}
	else if (format == ColorFormat::RGBA)
	{
		newData.assign(data, data + static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * 4);
		stride = width * 4;
	}

	std::vector<T> row{};
	row.resize(stride);

	for (uint32_t lowOffset = 0, highOffset = (height - 1) * stride; lowOffset < highOffset; lowOffset += stride, highOffset -= stride)
	{
		std::copy(newData.data() + lowOffset, newData.data() + lowOffset + stride, row.data());
		std::copy(newData.data() + highOffset, newData.data() + highOffset + stride, newData.data() + lowOffset);
		std::copy(row.data(), row.data() + stride, newData.data() + highOffset);
	}

	return newData;
}

#endif /*_TRAP_IMAGE_H_*/