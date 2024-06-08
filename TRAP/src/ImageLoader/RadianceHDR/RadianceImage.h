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
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		std::vector<f32> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::RadianceImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::RadianceImage::GetPixelDataSize() const noexcept
{
	return m_data.size() * sizeof(f32);
}

#endif /*TRAP_IMAGE_RADIANCE_H*/
