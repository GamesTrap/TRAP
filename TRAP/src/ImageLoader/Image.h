#ifndef _TRAP_IMAGE_H_
#define _TRAP_IMAGE_H_

#include "TRAPPCH.h"

namespace TRAP::INTERNAL
{
	std::vector<uint8_t> DecodeBGR16(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeBGR24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeBGRA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeBGRAMap8(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEBGRAMap8(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
	std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeRLEBGR16(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeRLEBGR24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
	std::vector<uint8_t> DecodeRLEBGRA32(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
}

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

		static std::vector<uint8_t> FlipY(unsigned int width, unsigned int height, ImageFormat format, uint8_t* data);
		static std::vector<uint16_t> FlipY(unsigned int width, unsigned int height, ImageFormat format, uint16_t* data);
		static std::vector<float> FlipY(unsigned int width, unsigned int height, ImageFormat format, float* data);

		static std::vector<uint8_t> FlipX(unsigned int width, unsigned int height, ImageFormat format, uint8_t* data);
	};
}

#endif /*_TRAP_IMAGE_H_*/