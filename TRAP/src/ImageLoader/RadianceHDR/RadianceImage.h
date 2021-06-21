#ifndef _TRAP_RADIANCEIMAGE_H_
#define _TRAP_RADIANCEIMAGE_H_

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class RadianceImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit RadianceImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		RadianceImage(const RadianceImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		RadianceImage& operator=(const RadianceImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		RadianceImage(RadianceImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		RadianceImage& operator=(RadianceImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~RadianceImage() override = default;

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
		static float ConvertComponent(int8_t exponent, int32_t value);
		static bool Decrunch(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex, uint32_t length, std::ifstream& file);
		static bool OldDecrunch(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex, uint32_t length, std::ifstream& file);
		void WorkOnRGBE(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex, std::vector<float>& data, uint32_t dataIndex);

		int8_t eMax, eMin;
		
		std::vector<float> m_data;

		inline static constexpr uint32_t MinEncodingLength = 8; //Minimum scanline length for encoding
		inline static constexpr uint32_t MaxEncodingLength = 0x7FFF; //Maximum scanline length for encoding
		inline static constexpr uint32_t R = 0;
		inline static constexpr uint32_t G = 1;
		inline static constexpr uint32_t B = 2;
		inline static constexpr uint32_t E = 3;
	};
}

#endif /*_TRAP_IMAGE_RADIANCE_H_*/