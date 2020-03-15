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
		PFMImage(const PFMImage&) = default;
		PFMImage& operator=(const PFMImage&) = default;
		PFMImage(PFMImage&&) = default;
		PFMImage& operator=(PFMImage&&) = default;
		~PFMImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

	private:
		std::vector<float> m_data;
	};
}

#endif /*_TRAP_PFMIMAGE_H_*/