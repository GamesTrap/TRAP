#ifndef _TRAP_PPMIMAGE_H_
#define _TRAP_PPMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PPMImage final : public Image
	{
	public:
		explicit PPMImage(std::string filepath);
		PPMImage(const PPMImage&) = default;
		PPMImage& operator=(const PPMImage&) = default;
		PPMImage(PPMImage&&) = default;
		PPMImage& operator=(PPMImage&&) = default;
		~PPMImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
	};
}

#endif /*_TRAP_PPMIMAGE_H_*/