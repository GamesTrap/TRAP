#ifndef _TRAP_PNMIMAGE_H_
#define _TRAP_PNMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNMImage final : public Image
	{
	public:
		explicit PNMImage(std::string filepath);
		PNMImage(const PNMImage&) = default;
		PNMImage& operator=(const PNMImage&) = default;
		PNMImage(PNMImage&&) = default;
		PNMImage& operator=(PNMImage&&) = default;
		~PNMImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
	};
}

#endif /*_TRAP_PNMIMAGE_H_*/