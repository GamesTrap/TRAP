#ifndef TRAP_RADIANCEIMAGE_H
#define TRAP_RADIANCEIMAGE_H

#include "ImageLoader/Image.h"

#include <optional>

namespace TRAP::INTERNAL
{
	class RadianceImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit RadianceImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		RadianceImage(const RadianceImage&) noexcept = default;
		/// @brief Copy assignment operator.
		RadianceImage& operator=(const RadianceImage&) noexcept = default;
		/// @brief Move constructor.
		RadianceImage(RadianceImage&&) noexcept = default;
		/// @brief Move assignment operator.
		RadianceImage& operator=(RadianceImage&&) noexcept = default;
		/// @brief Destructor.
		~RadianceImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<f32> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::RadianceImage::GetPixelData() const noexcept
{
	const std::span data(m_data);
	return TRAP::Utils::AsBytes(data);
}

#endif /*TRAP_IMAGE_RADIANCE_H*/
