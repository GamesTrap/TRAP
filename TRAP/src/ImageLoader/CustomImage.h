#ifndef _TRAP_CUSTOMIMAGE_H_
#define _TRAP_CUSTOMIMAGE_H_

#include "Image.h"

namespace TRAP::INTERNAL
{
	class CustomImage final : public Image
	{
	public:
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, std::vector<uint8_t> pixelData);
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, std::vector<uint16_t> pixelData);
		CustomImage(std::string filepath, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ColorFormat format, std::vector<float> pixelData);
		CustomImage(const CustomImage&) = default;
		CustomImage& operator=(const CustomImage&) = default;
		CustomImage(CustomImage&&) = default;
		CustomImage& operator=(CustomImage&&) = default;
		~CustomImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		std::vector<float> m_dataHDR;
	};
}

#endif /*_TRAP_CUSTOMIMAGE_H_*/