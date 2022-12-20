#ifndef TRAP_CUSTOMIMAGE_H
#define TRAP_CUSTOMIMAGE_H

#include "Image.h"

namespace TRAP::INTERNAL
{
	class CustomImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of image.</param>
		/// <param name="width">Width for the image.</param>
		/// <param name="height">Height for the image.</param>
		/// <param name="format">Color format of the pixel data.</param>
		/// <param name="pixelData">Raw pixel data.</param>
		template<typename T>
		CustomImage(std::filesystem::path filepath, uint32_t width, uint32_t height, ColorFormat format,
		            std::vector<T> pixelData);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		CustomImage(const CustomImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CustomImage& operator=(const CustomImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CustomImage(CustomImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CustomImage& operator=(CustomImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~CustomImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const noexcept override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		std::vector<float> m_dataHDR;
	};
}

template<typename T>
TRAP::INTERNAL::CustomImage::CustomImage(std::filesystem::path filepath, const uint32_t width, const uint32_t height,
                                         const ColorFormat format, std::vector<T> pixelData)
	: m_data(), m_data2Byte(), m_dataHDR()
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	static_assert(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	              std::is_same<T, float>::value, "Invalid type!");

	if (format == ColorFormat::NONE)
	{
		TRAP_ASSERT(false, "CustomImage(): Invalid color format!");
		return;
	}
	if (pixelData.empty())
	{
		TRAP_ASSERT(false, "CustomImage(): Invalid pixel data provided!");
		return;
	}

	m_bitsPerPixel = sizeof(T) * 8 * static_cast<uint32_t>(format);

	m_width = width;
	m_height = height;
	m_colorFormat = format;
	m_filepath = std::move(filepath);

	if constexpr(std::is_same<T, float>::value)
	{
		m_isHDR = true;
		m_dataHDR = std::move(pixelData);
	}
	else if constexpr (std::is_same<T, uint16_t>::value)
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

#endif /*TRAP_CUSTOMIMAGE_H*/