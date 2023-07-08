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
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] constexpr uint64_t GetPixelDataSize() const noexcept override;
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
		[[nodiscard]] static constexpr std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height,
		                                                                     uint32_t channels, std::vector<uint8_t>& colorMap);
		/// <summary>
		/// Decode run length encoded grayscale data.
		/// </summary>
		/// <param name="source">Run length encoded grayscale data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded grayscale pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source,
		                                                                       uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGR16 data to RGB24.
		/// </summary>
		/// <param name="source">Run length encoded BGR16 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGB24 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source,
		                                                                           uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGR24 data to RGB24.
		/// </summary>
		/// <param name="source">Run length encoded BGR24 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGB24 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source,
		                                                                           uint32_t width, uint32_t height);
		/// <summary>
		/// Convert run length encoded BGRA32 data to RGBA32.
		/// </summary>
		/// <param name="source">Run length encoded BGRA32 data.</param>
		/// <param name="width">Width of the image.</param>
		/// <param name="height">Height of the image.</param>
		/// <returns>Decoded RGBA32 pixel data.</returns>
		[[nodiscard]] static constexpr std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source,
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
			std::string ImageID;
			std::vector<uint8_t> ColorMap{};
			std::vector<uint8_t> ImageData{};
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::TGAImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t TRAP::INTERNAL::TGAImage::GetPixelDataSize() const noexcept
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::INTERNAL::TGAImage::DecodeRLEBGRAMap(std::vector<uint8_t>& source, const uint32_t width,
                                                                                        const uint32_t height, const uint32_t channels,
																                        std::vector<uint8_t>& colorMap)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * channels);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channels-byte color values in the file are taken verbatim
		//If RLE, the next single channels-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		uint32_t count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * channels + l) > (width * height * channels))
			count = (width * height * channels - l) / channels;

		for (uint32_t j = 0; j < count; j++)
		{
			if (channels == 1)
			{
				data[index++] = colorMap[NumericCast<std::size_t>(source[i]) * channels];
				l++;
			}
			else if (channels == 2)
			{
				data[index++] = NumericCast<uint8_t>(colorMap[source[i] * channels + 1u] << 1u) & 0xF8u;
				data[index++] = NumericCast<uint8_t>(NumericCast<uint8_t>((colorMap[source[i] * channels + 1u]) << 6u) |
				                                     NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels] >> 2u)) & 0xF8u;
				data[index++] = NumericCast<uint8_t>(colorMap[NumericCast<std::size_t>(source[i]) * channels] << 3u) & 0xF8u;
			}
			else if (channels == 3)
			{
				data[index++] = colorMap[source[i] * channels + 2];
				data[index++] = colorMap[source[i] * channels + 1];
				data[index++] = colorMap[source[i] * channels + 0];
				l += 3;
			}
			else if (channels == 4)
			{
				data[index++] = colorMap[source[i] * channels + 2];
				data[index++] = colorMap[source[i] * channels + 1];
				data[index++] = colorMap[source[i] * channels + 0];
				data[index++] = colorMap[source[i] * channels + 3];
				l += 4;
			}

			if (raw) //In RAW mode, keep advancing to subsequent values
				i++; //In RLE mode, just repeat the packet[1] color
		}
		if (!raw) //After outputting count values, advance if RLE
			i++;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::INTERNAL::TGAImage::DecodeRLEGrayScale(std::vector<uint8_t>& source,
                                                                                          const uint32_t width, const uint32_t height)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count byte color values in the file are taken verbatim
		//If RLE, the next single byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		uint32_t count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count + l) > (width * height))
			count = width * height - l;

		for (uint32_t j = 0; j < count; j++)
		{
			data[index++] = source[i];

			if (raw) //In RAW mode, keep advancing to subsequent values
				i++; //In RLE mode, just repeat the packet[1] color
			l++;
		}
		if (!raw) //After outputting count values, advance if RLE
			i++;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source,
                                                                                              const uint32_t width, const uint32_t height)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		uint32_t count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 3 + l) > (width * height * 3))
			count = (width * height * 3 - l) / 3;

		for (uint32_t j = 0; j < count; j++)
		{
			data[index++] = NumericCast<uint8_t>(source[i + 1u] << 1u) & 0xF8u;
			data[index++] = NumericCast<uint8_t>(NumericCast<uint8_t>(source[i + 1u] << 6u) | NumericCast<uint8_t>(source[i] >> 2u)) & 0xF8u;
			data[index++] = NumericCast<uint8_t>(source[i] << 3u) & 0xF8u;

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 2; //IN RLE mode, just repeat the packet[1] RGB color
			l += 3;
		}
		if (!raw) //After outputting count RGB values, advance if RLE
			i += 2;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source,
                                                                                              const uint32_t width, const uint32_t height)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * 3);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		uint32_t count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 3 + l) > (width * height * 3))
			count = (width * height * 3 - l) / 3;

		for (uint32_t j = 0; j < count; j++)
		{
			data[index++] = source[i + 2]; //Red
			data[index++] = source[i + 1]; //Green
			data[index++] = source[i];     //Blue

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 3; //IN RLE mode, just repeat the packet[1] RGB color
			l += 3;
		}
		if (!raw) //After outputting count RGB values, advance if RLE
			i += 3;
	}

	return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<uint8_t> TRAP::INTERNAL::TGAImage::ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source,
                                                                                              const uint32_t width, const uint32_t height)
{
	std::vector<uint8_t> data{};
	data.resize(NumericCast<std::size_t>(width) * height * 4);

	uint32_t index = 0;
	for (uint32_t i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		uint32_t count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 4 + l) > (width * height * 4))
			count = (width * height * 4 - l) / 4;

		for (uint32_t j = 0; j < count; j++)
		{
			data[index++] = source[i + 2]; //Red
			data[index++] = source[i + 1]; //Green
			data[index++] = source[i];     //Blue
			data[index++] = source[i + 3]; //Alpha

			if (raw) //In RAW mode, keep advancing to subsequent values
				i += 4; //IN RLE mode, just repeat the packet[1] RGBA color
			l += 4;
		}
		if (!raw) //After outputting count RGBA values, advance if RLE
			i += 4;
	}

	return data;
}

#endif /*TRAP_TGAIMAGE_H*/