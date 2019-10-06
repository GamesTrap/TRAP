#ifndef _TRAP_EMPTYIMAGE_H_
#define _TRAP_EMPTYIMAGE_H_

#include "Image.h"

namespace TRAP::INTERNAL
{
	class EmptyImage final : public Image
	{
	public:
		explicit EmptyImage(ImageFormat format, uint32_t width, uint32_t height);

		void* GetPixelData() override;
		uint32_t GetPixelDataSize() const override;
		uint32_t GetBitsPerPixel() const override;
		uint32_t GetBytesPerPixel() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		bool HasAlphaChannel() const override;
		bool IsImageCompressed() const override;
		bool IsImageGrayScale() const override;
		bool IsImageColored() const override;
		bool IsHDR() const override;
		std::string GetFilePath() const override;
		ImageFormat GetFormat() const override;

	private:
		ImageFormat m_format;
		bool m_isImageColored;
		bool m_isImageGrayScale;
		bool m_hasAlphaChannel;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_bitPerPixel;
	};
}

#endif /*_TRAP_EMPTYIMAGE_H_*/