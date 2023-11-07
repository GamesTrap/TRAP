#ifndef TRAP_CUSTOMIMAGE_H
#define TRAP_CUSTOMIMAGE_H

#include "Image.h"

namespace TRAP::INTERNAL
{
	class CustomImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of image.
		/// @param width Width for the image.
		/// @param height Height for the image.
		/// @param format Color format of the pixel data.
		/// @param pixelData Raw pixel data.
		template<typename T>
		requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
		CustomImage(std::filesystem::path filepath, u32 width, u32 height, ColorFormat format,
		            std::vector<T> pixelData);
		/// @brief Copy constructor.
		CustomImage(const CustomImage&) noexcept = default;
		/// @brief Copy assignment operator.
		CustomImage& operator=(const CustomImage&) noexcept = default;
		/// @brief Move constructor.
		CustomImage(CustomImage&&) noexcept = default;
		/// @brief Move assignment operator.
		CustomImage& operator=(CustomImage&&) noexcept = default;
		/// @brief Destructor.
		~CustomImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
		std::vector<f32> m_dataHDR;
	};
}

template<typename T>
requires std::same_as<T, u8> || std::same_as<T, u16> || std::same_as<T, f32>
TRAP::INTERNAL::CustomImage::CustomImage(std::filesystem::path filepath, const u32 width, const u32 height,
                                         const ColorFormat format, std::vector<T> pixelData)
	: Image(std::move(filepath), width, height, format)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (m_colorFormat == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "CustomImage(): Invalid color format!");
		return;
	}
	if (pixelData.empty())
	{
		TRAP_ASSERT(false, "CustomImage(): Invalid pixel data provided!");
		return;
	}

	m_bitsPerPixel = sizeof(T) * 8u * std::to_underlying(m_colorFormat);

	if constexpr(std::same_as<T, f32>)
	{
		m_isHDR = true;
		m_dataHDR = std::move(pixelData);
	}
	else if constexpr (std::same_as<T, u16>)
	{
		m_isHDR = false;
		m_data2Byte = std::move(pixelData);
	}
	else
	{
		m_isHDR = false;
		m_data = std::move(pixelData);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::CustomImage::GetPixelData() const noexcept
{
	if(!m_dataHDR.empty())
		return m_dataHDR.data();

	if(!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::CustomImage::GetPixelDataSize() const noexcept
{
	if(!m_dataHDR.empty())
		return m_dataHDR.size() * sizeof(f32);

	if(!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(u16);

	return m_data.size();
}

#endif /*TRAP_CUSTOMIMAGE_H*/