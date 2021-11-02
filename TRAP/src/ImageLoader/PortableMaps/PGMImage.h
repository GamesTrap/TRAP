#ifndef TRAP_PGMIMAGE_H
#define TRAP_PGMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PGMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit PGMImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		PGMImage(const PGMImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		PGMImage& operator=(const PGMImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		PGMImage(PGMImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		PGMImage& operator=(PGMImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~PGMImage() override = default;

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

		struct Header
		{
			std::string MagicNumber = "";
			uint32_t Width = 0;
			uint32_t Height = 0;
			uint32_t MaxValue = 255;
		};
	};
}

#endif /*TRAP_PGMIMAGE_H*/