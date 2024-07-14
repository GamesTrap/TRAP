#ifndef TRAP_PFMIMAGE_H
#define TRAP_PFMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PFMImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PFMImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PFMImage(const PFMImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PFMImage& operator=(const PFMImage&) noexcept = default;
		/// @brief Move constructor.
		PFMImage(PFMImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PFMImage& operator=(PFMImage&&) noexcept = default;
		/// @brief Destructor.
		~PFMImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<f32> m_data;

		struct Header
		{
			std::string MagicNumber;
			u32 Width = 0;
			u32 Height = 0;
			f32 ByteOrder = 0.0f;
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PFMImage::GetPixelData() const noexcept
{
	const std::span data(m_data);
	return TRAP::Utils::AsBytes(data);
}

#endif /*TRAP_PFMIMAGE_H*/