#ifndef _TRAP_PGMIMAGE_H_
#define _TRAP_PGMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PGMImage final : public Image
	{
	public:
		explicit PGMImage(std::string filepath);
		PGMImage(const PGMImage&) = default;
		PGMImage& operator=(const PGMImage&) = default;
		PGMImage(PGMImage&&) = default;
		PGMImage& operator=(PGMImage&&) = default;
		~PGMImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
	};
}

#endif /*_TRAP_PGMIMAGE_H_*/