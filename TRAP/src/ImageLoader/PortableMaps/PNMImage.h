#ifndef TRAP_PNMIMAGE_H
#define TRAP_PNMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit PNMImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		PNMImage(const PNMImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PNMImage& operator=(const PNMImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PNMImage(PNMImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PNMImage& operator=(PNMImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PNMImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const noexcept override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;

		struct Header
		{
			std::string MagicNumber = "";
			std::uint32_t Width = 0;
			std::uint32_t Height = 0;
			std::uint32_t MaxValue = 255;
		};
	};
}

#endif /*TRAP_PNMIMAGE_H*/