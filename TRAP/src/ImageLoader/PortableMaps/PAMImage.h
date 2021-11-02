#ifndef TRAP_PAMIMAGE_H
#define TRAP_PAMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PAMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit PAMImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		PAMImage(const PAMImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		PAMImage& operator=(const PAMImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		PAMImage(PAMImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		PAMImage& operator=(PAMImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~PAMImage() override = default;

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
			std::uint32_t Width = 0;
			std::uint32_t Height = 0;
			std::uint32_t Depth = 0;
			std::uint32_t MaxValue = 255;
			std::string TuplType = "";
		};
	};
}

#endif /*TRAP_PAMIMAGE_H*/