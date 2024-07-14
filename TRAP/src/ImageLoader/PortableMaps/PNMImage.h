#ifndef TRAP_PNMIMAGE_H
#define TRAP_PNMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNMImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PNMImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PNMImage(const PNMImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PNMImage& operator=(const PNMImage&) noexcept = default;
		/// @brief Move constructor.
		PNMImage(PNMImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PNMImage& operator=(PNMImage&&) noexcept = default;
		/// @brief Destructor.
		~PNMImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

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

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PNMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
	{
		const std::span data(m_data2Byte);
		return TRAP::Utils::AsBytes(data);
	}

	return m_data;
}

#endif /*TRAP_PNMIMAGE_H*/