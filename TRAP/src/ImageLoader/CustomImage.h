#ifndef _TRAP_CUSTOMIMAGE_H_
#define _TRAP_CUSTOMIMAGE_H_

#include "Image.h"

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
		CustomImage(std::string filepath, uint32_t width, uint32_t height, ColorFormat format,
		            std::vector<uint8_t> pixelData);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of Image.</param>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image.</param>
		/// <param name="format">Color format of the pixelData.</param>
		/// <param name="pixelData">Raw pixel data.</param>
		CustomImage(std::string filepath, uint32_t width, uint32_t height, ColorFormat format,
		            std::vector<uint16_t> pixelData);
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of Image.</param>
		/// <param name="width">Width for the Image.</param>
		/// <param name="height">Height for the Image.</param>
		/// <param name="format">Color format of the pixelData.</param>
		/// <param name="pixelData">Raw pixel data.</param>
		CustomImage(std::string filepath, uint32_t width, uint32_t height, ColorFormat format,
		            std::vector<float> pixelData);
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

#endif /*_TRAP_CUSTOMIMAGE_H_*/