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
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

		/// @brief Save an TRAP::Image as a Portable Pixmap (PPM) file.
		/// @param img Image to save.
		/// @param filepath File path to save the image to.
		static void Save(const Image& img, const std::filesystem::path& filepath);

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PPMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
	{
		const std::span data(m_data2Byte);
		return TRAP::Utils::AsBytes(data);
	}

	return m_data;
}

#endif /*TRAP_PPMIMAGE_H*/
