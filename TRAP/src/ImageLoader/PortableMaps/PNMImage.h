#ifndef _TRAP_PNMIMAGE_H_
#define _TRAP_PNMIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit PNMImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		PNMImage(const PNMImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		PNMImage& operator=(const PNMImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		PNMImage(PNMImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		PNMImage& operator=(PNMImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~PNMImage() = default;

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
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
	};
}

#endif /*_TRAP_PNMIMAGE_H_*/