#ifndef _TRAP_PFMIMAGE_H_
#define _TRAP_PFMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PFMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit PFMImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		PFMImage(const PFMImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		PFMImage& operator=(const PFMImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		PFMImage(PFMImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		PFMImage& operator=(PFMImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~PFMImage() = default;

		/// <summary>
		/// Retrieve the raw pixel data of the Image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the Image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

	private:
		std::vector<float> m_data;
	};
}

#endif /*_TRAP_PFMIMAGE_H_*/