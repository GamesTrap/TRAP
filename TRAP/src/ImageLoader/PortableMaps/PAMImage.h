#ifndef TRAP_PAMIMAGE_H
#define TRAP_PAMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PAMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit PAMImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		PAMImage(const PAMImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PAMImage& operator=(const PAMImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PAMImage(PAMImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PAMImage& operator=(PAMImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PAMImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;

		struct Header
		{
			std::string MagicNumber;
			u32 Width = 0;
			u32 Height = 0;
			u32 Depth = 0;
			u32 MaxValue = 255;
			std::string TuplType;
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::PAMImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::PAMImage::GetPixelDataSize() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(u16);

	return m_data.size();
}

#endif /*TRAP_PAMIMAGE_H*/