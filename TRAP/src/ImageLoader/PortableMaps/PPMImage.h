#ifndef TRAP_PPMIMAGE_H
#define TRAP_PPMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PPMImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PPMImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PPMImage(const PPMImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PPMImage& operator=(const PPMImage&) noexcept = default;
		/// @brief Move constructor.
		PPMImage(PPMImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PPMImage& operator=(PPMImage&&) noexcept = default;
		/// @brief Destructor.
		~PPMImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

		/// @brief Save an TRAP::Image as a Portable Pixmap (PPM) file.
		/// @param img Image to save.
		/// @param filepath File path to save the image to.
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