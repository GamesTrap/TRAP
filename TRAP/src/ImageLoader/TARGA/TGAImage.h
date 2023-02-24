#ifndef TRAP_TGAIMAGE_H
#define TRAP_TGAIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class TGAImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit TGAImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		TGAImage(const TGAImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TGAImage& operator=(const TGAImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TGAImage(TGAImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TGAImage& operator=(TGAImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~TGAImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] uint64_t GetPixelDataSize() const noexcept override;
	private:
		/// <summary>
		/// Decode run length encoded indexed BGRA data.
		/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
		/// </summary>
		/// <param name="source">Run length encoded indexed BGRA data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <param name="channels">Amount of channels, i.e. 4 = RGBA, 3 = RGB and so on.</param>
		/// <param name="colorMap">Color table.</param>
		/// <returns>Decoded pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height,
		                                                           uint32_t channels, std::vector<uint8_t>& colorMap);
		/// <summary>
		/// Decode run length encoded grayscale data.
		/// </summary>
		/// <param name="source">Run length encoded grayscale data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded grayscale pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source,
		                                                             uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGR16 data to RGB24.
		/// </summary>
		/// <param name="source">Run length encoded BGR16 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGB24 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source,
		                                                                 uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGR24 data to RGB24.
		/// </summary>
		/// <param name="source">Run length encoded BGR24 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGB24 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source,
		                                                                 uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGRA32 data to RGBA32.
		/// </summary>
		/// <param name="source">Run length encoded BGRA32 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGBA32 pixel data.</returns>
		[[nodiscard]] static std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source,
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

#endif /*TRAP_TGAIMAGE_H*/