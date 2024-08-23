#ifndef TRAP_PGMIMAGE_H
#define TRAP_PGMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PGMImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PGMImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PGMImage(const PGMImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PGMImage& operator=(const PGMImage&) noexcept = default;
		/// @brief Move constructor.
		PGMImage(PGMImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PGMImage& operator=(PGMImage&&) noexcept = default;
		/// @brief Destructor.
		~PGMImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PGMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
	{
		const std::span data(m_data2Byte);
		return TRAP::Utils::AsBytes(data);
	}

	return m_data;
}

#endif /*TRAP_PGMIMAGE_H*/
