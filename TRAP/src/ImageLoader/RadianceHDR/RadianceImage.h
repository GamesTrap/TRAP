#ifndef TRAP_RADIANCEIMAGE_H
#define TRAP_RADIANCEIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class RadianceImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit RadianceImage(std::string filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		RadianceImage(const RadianceImage&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RadianceImage& operator=(const RadianceImage&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RadianceImage(RadianceImage&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RadianceImage& operator=(RadianceImage&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~RadianceImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

	private:
		static float ConvertComponent(int8_t exponent, int32_t value);
		static bool Decrunch(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex,
		                     uint32_t length, std::ifstream& file);
		static bool OldDecrunch(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex,
		                        uint32_t length, std::ifstream& file);
		void WorkOnRGBE(std::vector<std::array<uint8_t, 4>>& scanline, uint32_t scanlineIndex,
		                std::vector<float>& data, uint32_t dataIndex);

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

#endif /*TRAP_IMAGE_RADIANCE_H*/