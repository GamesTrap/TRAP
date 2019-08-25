#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_

#include "TRAPPCH.h"
#include "Maths/Math.h"

namespace TRAP::INTERNAL
{
	std::vector<uint8_t> ConvertBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
}

//-------------------------------------------------------------------------------------------------------------------//

namespace TRAP
{
	enum class ImageFormat
	{
		NONE = 0,
		RGB,
		RGBA,
		Gray_Scale,
		Gray_Scale_Alpha
	};

	//Abstract Base Class
	class Image
	{
	public:
		Image() = default;
		Image(const Image&) = default;
		Image& operator=(const Image&) = default;
		Image(Image&&) = default;
		Image& operator=(Image&&) = default;
		virtual ~Image() = default;

		virtual void* GetPixelData() = 0;
		virtual uint32_t GetPixelDataSize() const = 0;
		virtual uint32_t GetBitsPerPixel() const = 0;
		virtual uint32_t GetBytesPerPixel() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bool HasAlphaChannel() const = 0;
		virtual bool IsImageCompressed() const = 0;
		virtual bool IsImageGrayScale() const = 0;
		virtual bool IsImageColored() const = 0;
		virtual bool IsHDR() const = 0;
		virtual std::string GetFilePath() const = 0;
		virtual ImageFormat GetFormat() const = 0;

		static std::unique_ptr<Image> LoadFromFile(const std::string& filepath);
		static std::unique_ptr<Image> LoadFallback();
		
		template<typename T>
		static std::vector<T> FlipY(unsigned int width, unsigned int height, ImageFormat format, T* data);
		template<typename T>
		static std::vector<T> FlipX(unsigned int width, unsigned int height, ImageFormat format, T* data);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
std::vector<T> TRAP::Image::FlipY(const unsigned int width, const unsigned int height, const ImageFormat format, T* data)
{
	if (!(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, float>::value))
	{
		TP_ASSERT(false, "Invalid type!");
		return {};
	}
	if (format == ImageFormat::NONE || !data)
		return {};

	std::vector<T> newData{};
	uint32_t stride;
	if (format == ImageFormat::Gray_Scale)
	{
		newData.assign(data, data + width * height);
		stride = width;
	}
	else if (format == ImageFormat::Gray_Scale_Alpha)
	{
		newData.assign(data, data + width * height * 2);
		stride = width * 2;
	}
	else if (format == ImageFormat::RGB)
	{
		newData.assign(data, data + width * height * 3);
		stride = width * 3;
	}
	else if (format == ImageFormat::RGBA)
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

//-------------------------------------------------------------------------------------------------------------------//

template <typename T>
std::vector<T> TRAP::Image::FlipX(const unsigned int width, const unsigned int height, const ImageFormat format, T* data)
{
	if (!(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value || std::is_same<T, float>::value))
	{
		TP_ASSERT(false, "Invalid type!");
		return {};
	}
	if (format == ImageFormat::NONE || !data)
		return {};

	std::vector<T> newData{};
	uint32_t stride;
	if (format == ImageFormat::Gray_Scale)
	{
		newData.assign(data, data + width * height);
		stride = height;
	}
	else if (format == ImageFormat::Gray_Scale_Alpha)
	{
		newData.assign(data, data + width * height * 2);
		stride = height * 2;
	}
	else if (format == ImageFormat::RGB)
	{
		newData.assign(data, data + width * height * 3);
		stride = height * 3;
	}
	else if (format == ImageFormat::RGBA)
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

#endif /*_TRAP_IMAGE_H_*/