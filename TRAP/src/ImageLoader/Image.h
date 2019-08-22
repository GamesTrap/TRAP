#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_

#include "TRAPPCH.h"
#include "Maths/Math.h"

namespace TRAP::INTERNAL
{
	std::vector<uint8_t> ConvertBGR16ToRGB16(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertBGRA32ToRGBA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecoeBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertRLEBGR16ToRGB(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> ConvertRLEBGR24ToRGB(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
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

	unsigned int yt, yb;
	std::vector<T> newData{};
	if (format == ImageFormat::Gray_Scale)
		newData.assign(data, data + width * height);
	else if (format == ImageFormat::Gray_Scale_Alpha)
		newData.assign(data, data + width * height * 2);
	else if (format == ImageFormat::RGB)
		newData.assign(data, data + width * height * 3);
	else if (format == ImageFormat::RGBA)
		newData.assign(data, data + width * height * 4);

	for (yt = 0, yb = height - 1; yt <= yb; yt++, yb--)
		for (unsigned int x = 0; x < width; x++)
		{
			if (format == ImageFormat::Gray_Scale)
			{
				const T temp{ newData[(x + yb * width) + 0] };
				newData[x + yb * width + 0] = newData[x + yt * width + 0];
				newData[x + yt * width + 0] = temp;
			}
			else if (format == ImageFormat::Gray_Scale_Alpha)
			{
				const Math::tVec2<T> temp{ newData[2 * (x + yb * width) + 0], newData[2 * (x + yb * width) + 1] };
				newData[2 * (x + yb * width) + 0] = newData[2 * (x + yt * width) + 0];
				newData[2 * (x + yb * width) + 1] = newData[2 * (x + yt * width) + 1];
				newData[2 * (x + yt * width) + 0] = temp.x;
				newData[2 * (x + yt * width) + 1] = temp.y;
			}
			else if (format == ImageFormat::RGB)
			{
				const Math::tVec3<T> temp{ newData[3 * (x + yb * width) + 0], newData[3 * (x + yb * width) + 1], newData[3 * (x + yb * width) + 2] };
				newData[3 * (x + yb * width) + 0] = newData[3 * (x + yt * width) + 0];
				newData[3 * (x + yb * width) + 1] = newData[3 * (x + yt * width) + 1];
				newData[3 * (x + yb * width) + 2] = newData[3 * (x + yt * width) + 2];
				newData[3 * (x + yt * width) + 0] = temp.x;
				newData[3 * (x + yt * width) + 1] = temp.y;
				newData[3 * (x + yt * width) + 2] = temp.z;
			}
			else if (format == ImageFormat::RGBA)
			{
				const Math::tVec4<T> temp{ newData[4 * (x + yb * width) + 0], newData[4 * (x + yb * width) + 1], newData[4 * (x + yb * width) + 2], newData[4 * (x + yb * width) + 3] };
				newData[4 * (x + yb * width) + 0] = newData[4 * (x + yt * width) + 0];
				newData[4 * (x + yb * width) + 1] = newData[4 * (x + yt * width) + 1];
				newData[4 * (x + yb * width) + 2] = newData[4 * (x + yt * width) + 2];
				newData[4 * (x + yb * width) + 3] = newData[4 * (x + yt * width) + 3];
				newData[4 * (x + yt * width) + 0] = temp.x;
				newData[4 * (x + yt * width) + 1] = temp.y;
				newData[4 * (x + yt * width) + 2] = temp.z;
				newData[4 * (x + yt * width) + 3] = temp.w;
			}
			else
				return {};
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

	unsigned int xt, xb;
	std::vector<T> newData{};
	if (format == ImageFormat::Gray_Scale)
		newData.assign(data, data + width * height);
	else if (format == ImageFormat::Gray_Scale_Alpha)
		newData.assign(data, data + width * height * 2);
	else if (format == ImageFormat::RGB)
		newData.assign(data, data + width * height * 3);
	else if (format == ImageFormat::RGBA)
		newData.assign(data, data + width * height * 4);

	for (xt = 0, xb = width - 1; xt <= xb; xt++, xb--)
		for (unsigned int y = 0; y < height; y++)
		{
			if (format == ImageFormat::Gray_Scale)
			{
				const T temp{ newData[(y + xb * height) + 0] };
				newData[y + xb * height + 0] = newData[y + xt * height + 0];
				newData[y + xt * height + 0] = temp;
			}
			else if (format == ImageFormat::Gray_Scale_Alpha)
			{
				const TRAP::Math::tVec2<T> temp{ newData[2 * (y + xb * height) + 0], newData[2 * (y + xb * height) + 1] };
				newData[2 * (y + xb * height) + 0] = newData[2 * (y + xt * height) + 0];
				newData[2 * (y + xb * height) + 1] = newData[2 * (y + xt * height) + 1];
				newData[2 * (y + xt * height) + 0] = temp.x;
				newData[2 * (y + xt * height) + 1] = temp.y;
			}
			else if (format == ImageFormat::RGB)
			{
				const TRAP::Math::tVec3<T> temp{ newData[3 * (y + xb * height) + 0], newData[3 * (y + xb * height) + 1], newData[3 * (y + xb * height) + 2] };
				newData[3 * (y + xb * height) + 0] = newData[3 * (y + xt * height) + 0];
				newData[3 * (y + xb * height) + 1] = newData[3 * (y + xt * height) + 1];
				newData[3 * (y + xb * height) + 2] = newData[3 * (y + xt * height) + 2];
				newData[3 * (y + xt * height) + 0] = temp.x;
				newData[3 * (y + xt * height) + 1] = temp.y;
				newData[3 * (y + xt * height) + 2] = temp.z;
			}
			else if (format == ImageFormat::RGBA)
			{
				const Math::tVec4<T> temp{ newData[4 * (y + xb * height) + 0], newData[4 * (y + xb * height) + 1], newData[4 * (y + xb * height) + 2], newData[4 * (y + xb * height) + 3] };
				newData[4 * (y + xb * height) + 0] = newData[4 * (y + xt * height) + 0];
				newData[4 * (y + xb * height) + 1] = newData[4 * (y + xt * height) + 1];
				newData[4 * (y + xb * height) + 2] = newData[4 * (y + xt * height) + 2];
				newData[4 * (y + xb * height) + 3] = newData[4 * (y + xt * height) + 3];
				newData[4 * (y + xt * height) + 0] = temp.x;
				newData[4 * (y + xt * height) + 1] = temp.y;
				newData[4 * (y + xt * height) + 2] = temp.z;
				newData[4 * (y + xt * height) + 3] = temp.w;
			}
			else
				return {};
		}

	return newData;
}

#endif /*_TRAP_IMAGE_H_*/