#ifndef TRAP_TGAIMAGE_H
#define TRAP_TGAIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class TGAImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit TGAImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		TGAImage(const TGAImage&) noexcept = default;
		/// @brief Copy assignment operator.
		TGAImage& operator=(const TGAImage&) noexcept = default;
		/// @brief Move constructor.
		TGAImage(TGAImage&&) noexcept = default;
		/// @brief Move assignment operator.
		TGAImage& operator=(TGAImage&&) noexcept = default;
		/// @brief Destructor.
		~TGAImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;
	private:
		std::vector<u8> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::TGAImage::GetPixelData() const noexcept
{
	return m_data;
}

#endif /*TRAP_TGAIMAGE_H*/
