#ifndef _TRAP_TGAIMAGE_H_
#define _TRAP_TGAIMAGE_H_

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class TGAImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit TGAImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		TGAImage(const TGAImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		TGAImage& operator=(const TGAImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		TGAImage(TGAImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		TGAImage& operator=(TGAImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~TGAImage() override = default;

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
		static std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height,
		                                             uint32_t channels, std::vector<uint8_t>& colorMap);
		static std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source,
		                                               uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source,
		                                                   uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source,
		                                                   uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source,
		                                                   uint32_t width, uint32_t height);

		std::vector<uint8_t> m_data;

		struct Header
		{
			uint8_t IDLength = 0; //0 = No ImageID
			uint8_t ColorMapType = 0; //0 = No Color Map | 1 = Has Color Map
			uint8_t ImageType = 0; //0 = No Data | 1 = Uncompressed ColorMapped | 2 = Uncompressed TrueColor |
			                       //3 = Uncompressed BlackWhite | 9 = RunLengthEncoded ColorMapped |
								   //10 = RunLengthEncoded TrueColor | 11 = RunLengthEncoded BlackWhite
			//ColorMap spec
			uint16_t ColorMapOffset = 0;
			uint16_t NumOfColorMaps = 0;
			uint8_t ColorMapDepth = 0;
			//Image spec
			uint16_t XOffset = 0;
			uint16_t YOffset = 0;
			uint16_t Width = 0;
			uint16_t Height = 0;
			uint8_t BitsPerPixel = 0;
			uint8_t ImageDescriptor = 0;
		};

		struct ColorMapData
		{
			std::string ImageID = "";
			std::vector<uint8_t> ColorMap{};
			std::vector<uint8_t> ImageData{};
		};
	};
}

#endif /*_TRAP_TGAIMAGE_H_*/