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
		PPMImage(const PPMImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PPMImage& operator=(const PPMImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PPMImage(PPMImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PPMImage& operator=(PPMImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PPMImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

		/// <summary>
		/// Save an TRAP::Image as a Portable Pixmap (PPM) file.
		/// </summary>
		/// <param name="img">Image to save.</param>
		/// <param name="filepath">File path to save the image to.</param>
		static void Save(const Image* img, const std::filesystem::path& filepath);

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;

		struct Header
		{
			std::string MagicNumber;
			u32 Width = 0;
			u32 Height = 0;
			u32 MaxValue = 255;
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::PPMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::PPMImage::GetPixelDataSize() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(u16);

	return m_data.size();
}

#endif /*TRAP_PPMIMAGE_H*/