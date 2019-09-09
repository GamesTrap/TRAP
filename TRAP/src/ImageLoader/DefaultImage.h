#ifndef _TRAP_DEFAULTIMAGE_H_
#define _TRAP_DEFAULTIMAGE_H_

#include "TRAPPCH.h"

#include "Image.h"

namespace TRAP::INTERNAL
{
	class DefaultImage final : public Image
	{
	public:
		explicit DefaultImage(std::string filepath);

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
		std::string m_filepath;
		std::vector<uint8_t> m_data;
	};
}
#endif /*_TRAP_DEFAULTIMAGE_H_*/