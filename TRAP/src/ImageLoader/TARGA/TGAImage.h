#ifndef _TRAP_TGAIMAGE_H_
#define _TRAP_TGAIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{	
	class TGAImage final : public Image
	{
	public:
		explicit TGAImage(std::string filepath);
		TGAImage(const TGAImage&) = default;
		TGAImage& operator=(const TGAImage&) = default;
		TGAImage(TGAImage&&) = default;
		TGAImage& operator=(TGAImage&&) = default;
		~TGAImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		static std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
		static std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		
		std::vector<uint8_t> m_data;
	};
}

#endif /*_TRAP_TGAIMAGE_H_*/