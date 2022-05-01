#ifndef TRAP_PPMIMAGE_H
#define TRAP_PPMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PPMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit PPMImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		PPMImage(const PPMImage&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PPMImage& operator=(const PPMImage&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PPMImage(PPMImage&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PPMImage& operator=(PPMImage&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PPMImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

		/// <summary>
		/// Save an TRAP::Image as a Portable Pixmap (PPM) file.
		/// </summary>
		/// <param name="img">Image to save.</param>
		/// <param name="filepath">File path to save the image to.</param>
		static void Save(const Image* const img, const std::filesystem::path& filepath);

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

#endif /*TRAP_PPMIMAGE_H*/