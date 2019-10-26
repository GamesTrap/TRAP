#ifndef _TRAP_PAMIMAGE_H_
#define _TRAP_PAMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PAMImage final : public Image
	{
	public:
		explicit PAMImage(std::string filepath);

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
		std::string GetFilePath() const override;
		ImageFormat GetFormat() const override;

	private:
		std::string m_filepath;
		uint32_t m_bitsPerPixel;
		bool m_isImageGrayScale;
		bool m_isImageColored;
		bool m_hasAlphaChannel;
		uint32_t m_width;
		uint32_t m_height;
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		ImageFormat m_format;
	};
}

#endif /*_TRAP_PAMIMAGE_H_*/