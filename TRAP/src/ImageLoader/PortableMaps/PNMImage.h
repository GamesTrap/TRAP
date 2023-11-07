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
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

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

[[nodiscard]] constexpr const void* TRAP::INTERNAL::PNMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::PNMImage::GetPixelDataSize() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(u16);

	return m_data.size();
}

#endif /*TRAP_PNMIMAGE_H*/