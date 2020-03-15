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
		std::vector<uint8_t> m_data;
	};
}

#endif /*_TRAP_TGAIMAGE_H_*/