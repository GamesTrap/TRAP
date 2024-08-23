#ifndef TRAP_PAMIMAGE_H
#define TRAP_PAMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PAMImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PAMImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PAMImage(const PAMImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PAMImage& operator=(const PAMImage&) noexcept = default;
		/// @brief Move constructor.
		PAMImage(PAMImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PAMImage& operator=(PAMImage&&) noexcept = default;
		/// @brief Destructor.
		~PAMImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PAMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
	{
		const std::span data(m_data2Byte);
		return TRAP::Utils::AsBytes(data);
	}

	return m_data;
}

#endif /*TRAP_PAMIMAGE_H*/
