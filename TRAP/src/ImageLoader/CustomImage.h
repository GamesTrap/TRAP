#ifndef _TRAP_CUSTOMIMAGE_H_
#define _TRAP_CUSTOMIMAGE_H_

#include "Image.h"
#include "Embed.h"

namespace TRAP::INTERNAL
{
	class CustomImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of Image.</param>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image.</param>
		/// <param name="format">Color format of the pixelData.</param>
		/// <param name="pixelData">Raw pixel data.</param>
		template<typename T>
		CustomImage(std::string filepath, uint32_t width, uint32_t height, ColorFormat format,
		            std::vector<T> pixelData);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		CustomImage(const CustomImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		CustomImage& operator=(const CustomImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		CustomImage(CustomImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		CustomImage& operator=(CustomImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~CustomImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the Image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the Image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

	private:
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		std::vector<float> m_dataHDR;
	};
}

template<typename T>
TRAP::INTERNAL::CustomImage::CustomImage(std::string filepath, const uint32_t width, const uint32_t height,
                                         const ColorFormat format, std::vector<T> pixelData)
	: m_data(), m_data2Byte(), m_dataHDR()
{
	TP_PROFILE_FUNCTION();

	if constexpr (!(std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value ||
	                std::is_same<T, float>::value))
	{
		TRAP_CORE_ASSERT(false, "Invalid type!");
		return;
	}
	if (format == ColorFormat::NONE)
	{
		TRAP_CORE_ASSERT(false, "Invalid ColorFormat!");
		return;
	}
	if (pixelData.empty())
	{
		TRAP_CORE_ASSERT(false, "Invalid pixel data provided!");
		return;
	}

	if(format == ColorFormat::NONE)
	{
		TP_ERROR(Log::ImagePrefix, "ColorFormat is invalid!");
		TP_WARN(Log::ImagePrefix, "Using Default Image!");
		m_width = 32;
		m_height = 32;
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 32;
		m_data = std::vector<uint8_t>{ Embed::DefaultImageData.begin(), Embed::DefaultImageData.end() };
		m_isHDR = false;
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

#endif /*_TRAP_CUSTOMIMAGE_H_*/