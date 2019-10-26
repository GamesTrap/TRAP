#ifndef _TRAP_PFMIMAGE_H_
#define _TRAP_PFMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PFMImage final : public Image
	{
	public:
		explicit PFMImage(std::string filepath);

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
		uint32_t m_width;
		uint32_t m_height;
		std::vector<float> m_data;
		ImageFormat m_format;
	};
}

#endif /*_TRAP_PFMIMAGE_H_*/