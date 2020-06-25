#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_
#include "Maths/Vec2.h"

namespace TRAP
{
	//Abstract Base Class
	class Image
	{
	public:
		enum class ColorFormat
		{
			NONE = 0,
			RGB = 1,
			RGBA = 2,
			GrayScale = 3,
			GrayScaleAlpha = 4
		};

	protected:
		Image();
	public:
		Image(const Image&) = default;
		Image& operator=(const Image&) = default;
		Image(Image&&) = default;
		Image& operator=(Image&&) = default;
		virtual ~Image() = default;
	
		virtual const void* GetPixelData() const = 0;
		virtual uint32_t GetPixelDataSize() const = 0;
		uint32_t GetBitsPerPixel() const;
		uint32_t GetBytesPerPixel() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		Math::Vec2ui GetSize() const;
		bool HasAlphaChannel() const;
		bool IsImageGrayScale() const;
		bool IsImageColored() const;
		bool IsHDR() const;
		std::string_view GetFilePath() const;
		ColorFormat GetColorFormat() const;

		static Scope<Image> LoadFromFile(const std::string& filepath);
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format,
		                                   const std::vector<uint8_t>& pixelData);
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format,
		                                   const std::vector<uint16_t>& pixelData);
		static Scope<Image> LoadFromMemory(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format,
		                                   const std::vector<float>& pixelData);
		static Scope<Image> LoadFallback();

		static Scope<Image> FlipX(const Scope<Image>& img);
		static Scope<Image> FlipY(const Scope<Image>& img);
		
	protected:
		template<typename T>
		static std::vector<T> FlipX(uint32_t width, uint32_t height, ColorFormat format, const T* data);
		template<typename T>
		static std::vector<T> FlipY(uint32_t width, uint32_t height, ColorFormat format, const T* data);

		static std::vector<uint8_t> ConvertBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> DecodeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
		static std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
		static std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static bool IsGrayScale(ColorFormat format);
		static bool IsColored(ColorFormat format);
		static bool HasAlpha(ColorFormat format);

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
			
			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
		}
		else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
			(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
		{
			const std::vector<uint16_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint16_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
		}
		else
		{
			const std::vector<uint8_t> flipped = FlipX(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint8_t*>(img->GetPixelData()));
			
			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
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

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
		}
		else if ((img->IsImageGrayScale() && img->GetBitsPerPixel() == 16 && !img->HasAlphaChannel()) ||
			(img->IsImageGrayScale() && img->GetBitsPerPixel() == 32 && img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 48 && !img->HasAlphaChannel()) ||
			(img->IsImageColored() && img->GetBitsPerPixel() == 64 && img->HasAlphaChannel()))
		{
			const std::vector<uint16_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint16_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
		}
		else
		{
			const std::vector<uint8_t> flipped = FlipY(img->GetWidth(), img->GetHeight(), img->GetColorFormat(), static_cast<const uint8_t*>(img->GetPixelData()));

			result = LoadFromMemory(img->GetWidth(), img->GetHeight(), img->GetBitsPerPixel(), img->GetColorFormat(), flipped);
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
		TRAP_ASSERT(false, "Invalid type!");
		return {};
	}
	if (format == ColorFormat::NONE || !data)
		return {};

	std::vector<T> newData{};
	uint32_t stride = 0;
	if (format == ColorFormat::GrayScale)
	{
		newData.assign(data, data + width * height);
		stride = height;
	}
	else if (format == ColorFormat::GrayScaleAlpha)
	{
		newData.assign(data, data + width * height * 2);
		stride = height * 2;
	}
	else if (format == ColorFormat::RGB)
	{
		newData.assign(data, data + width * height * 3);
		stride = height * 3;
	}
	else if (format == ColorFormat::RGBA)
	{
		newData.assign(data, data + width * height * 4);
		stride = height * 4;
	}

	std::vector<T> row{};
	row.resize(stride);
	uint32_t lowOffset = 0;
	uint32_t highOffset = (width - 1) * stride;

	for (; lowOffset < highOffset; lowOffset += stride, highOffset -= stride)
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
		TRAP_ASSERT(false, "Invalid type!");
		return {};
	}
	if (format == ColorFormat::NONE || !data)
		return {};

	std::vector<T> newData{};
	uint32_t stride = 0;
	if (format == ColorFormat::GrayScale)
	{
		newData.assign(data, data + width * height);
		stride = width;
	}
	else if (format == ColorFormat::GrayScaleAlpha)
	{
		newData.assign(data, data + width * height * 2);
		stride = width * 2;
	}
	else if (format == ColorFormat::RGB)
	{
		newData.assign(data, data + width * height * 3);
		stride = width * 3;
	}
	else if (format == ColorFormat::RGBA)
	{
		newData.assign(data, data + width * height * 4);
		stride = width * 4;
	}

	std::vector<T> row{};
	row.resize(stride);

	row.reserve(stride);
	uint32_t lowOffset = 0;
	uint32_t highOffset = (height - 1) * stride;

	for (; lowOffset < highOffset; lowOffset += stride, highOffset -= stride)
	{
		std::copy(newData.data() + lowOffset, newData.data() + lowOffset + stride, row.data());
		std::copy(newData.data() + highOffset, newData.data() + highOffset + stride, newData.data() + lowOffset);
		std::copy(row.data(), row.data() + stride, newData.data() + highOffset);
	}

	return newData;
}

#endif /*_TRAP_IMAGE_H_*/