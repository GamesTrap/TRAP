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
		PAMImage(const PAMImage&) = default;
		PAMImage& operator=(const PAMImage&) = default;
		PAMImage(PAMImage&&) = default;
		PAMImage& operator=(PAMImage&&) = default;
		~PAMImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
	};
}

#endif /*_TRAP_PAMIMAGE_H_*/