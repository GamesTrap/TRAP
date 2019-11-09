#ifndef _TRAP_CUSTOMIMAGE_H_
#define _TRAP_CUSTOMIMAGE_H_

#include "Image.h"

namespace TRAP::INTERNAL
{
	class CustomImage final : public Image
	{
	public:
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, std::vector<uint8_t> pixelData);
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, std::vector<uint16_t> pixelData);
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, std::vector<float> pixelData);
		CustomImage(const CustomImage&) = default;
		CustomImage& operator=(const CustomImage&) = default;
		CustomImage(CustomImage&&) = default;
		CustomImage& operator=(CustomImage&&) = default;
		~CustomImage() = default;

		void* GetPixelData() override;
		uint32_t GetPixelDataSize() const override;
		uint32_t GetBitsPerPixel() const override;
		uint32_t GetBytesPerPixel() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		bool HasAlphaChannel() const override;
		bool IsImageGrayScale() const override;
		bool IsImageColored() const override;
		bool IsHDR() const override;
		std::string_view GetFilePath() const override;
		ImageFormat GetFormat() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		std::vector<float> m_dataHDR;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_bitsPerPixel;
		ImageFormat m_format;
		std::string m_filepath;
	};
}

#endif /*_TRAP_CUSTOMIMAGE_H_*/