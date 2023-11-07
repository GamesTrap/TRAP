#ifndef TRAP_TGAIMAGE_H
#define TRAP_TGAIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class TGAImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit TGAImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		TGAImage(const TGAImage&) noexcept = default;
		/// @brief Copy assignment operator.
		TGAImage& operator=(const TGAImage&) noexcept = default;
		/// @brief Move constructor.
		TGAImage(TGAImage&&) noexcept = default;
		/// @brief Move assignment operator.
		TGAImage& operator=(TGAImage&&) noexcept = default;
		/// @brief Destructor.
		~TGAImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;
	private:
		/// @brief Decode run length encoded indexed BGRA data.
		/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
		/// @param source Run length encoded indexed BGRA data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB and so on.
		/// @param colorMap Color table.
		/// @return Decoded pixel data.
		[[nodiscard]] static constexpr std::vector<u8> DecodeRLEBGRAMap(std::vector<u8>& source, u32 width, u32 height,
		                                                                     u32 channels, std::vector<u8>& colorMap);
		/// @brief Decode run length encoded grayscale data.
		/// @param source Run length encoded grayscale data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return Decoded grayscale pixel data.
		[[nodiscard]] static constexpr std::vector<u8> DecodeRLEGrayScale(std::vector<u8>& source,
		                                                                       u32 width, u32 height);
		/// @brief Convert run length encoded BGR16 data to RGB24.
		/// @param source Run length encoded BGR16 data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return Decoded RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertRLEBGR16ToRGB24(std::vector<u8>& source,
		                                                                           u32 width, u32 height);
		/// @brief Convert run length encoded BGR24 data to RGB24.
		/// @param source Run length encoded BGR24 data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return Decoded RGB24 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertRLEBGR24ToRGB24(std::vector<u8>& source,
		                                                                           u32 width, u32 height);
		/// @brief Convert run length encoded BGRA32 data to RGBA32.
		/// @param source Run length encoded BGRA32 data.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @return Decoded RGBA32 pixel data.
		[[nodiscard]] static constexpr std::vector<u8> ConvertRLEBGRA32ToRGBA(std::vector<u8>& source,
		                                                                           u32 width, u32 height);

		std::vector<u8> m_data;

		struct Header
		{
			u8 IDLength = 0; //0 = No ImageID
			u8 ColorMapType = 0; //0 = No Color Map | 1 = Has Color Map
			u8 ImageType = 0; //0 = No Data | 1 = Uncompressed ColorMapped | 2 = Uncompressed TrueColor |
			                       //3 = Uncompressed BlackWhite | 9 = RunLengthEncoded ColorMapped |
								   //10 = RunLengthEncoded TrueColor | 11 = RunLengthEncoded BlackWhite
			//ColorMap spec
			u16 ColorMapOffset = 0;
			u16 NumOfColorMaps = 0;
			u8 ColorMapDepth = 0;
			//Image spec
			u16 XOffset = 0;
			u16 YOffset = 0;
			u16 Width = 0;
			u16 Height = 0;
			u8 BitsPerPixel = 0;
			u8 ImageDescriptor = 0;
		};

		struct ColorMapData
		{
			std::string ImageID;
			std::vector<u8> ColorMap{};
			std::vector<u8> ImageData{};
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::TGAImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::TGAImage::GetPixelDataSize() const noexcept
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<u8> TRAP::INTERNAL::TGAImage::DecodeRLEBGRAMap(std::vector<u8>& source, const u32 width,
                                                                                        const u32 height, const u32 channels,
																                        std::vector<u8>& colorMap)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * channels);

	u32 index = 0;
	for (u32 i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channels-byte color values in the file are taken verbatim
		//If RLE, the next single channels-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * channels + l) > (width * height * channels))
			count = (width * height * channels - l) / channels;

		for (u32 j = 0; j < count; j++)
		{
			if (channels == 1)
			{
				data[index++] = colorMap[NumericCast<usize>(source[i]) * channels];
				l++;
			}
			else if (channels == 2)
			{
				data[index++] = NumericCast<u8>(colorMap[source[i] * channels + 1u] << 1u) & 0xF8u;
				data[index++] = NumericCast<u8>(NumericCast<u8>((colorMap[source[i] * channels + 1u]) << 6u) |
				                                     NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] >> 2u)) & 0xF8u;
				data[index++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] << 3u) & 0xF8u;
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

[[nodiscard]] constexpr std::vector<u8> TRAP::INTERNAL::TGAImage::DecodeRLEGrayScale(std::vector<u8>& source,
                                                                                          const u32 width, const u32 height)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height);

	u32 index = 0;
	for (u32 i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"

		//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count byte color values in the file are taken verbatim
		//If RLE, the next single byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count + l) > (width * height))
			count = width * height - l;

		for (u32 j = 0; j < count; j++)
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

[[nodiscard]] constexpr std::vector<u8> TRAP::INTERNAL::TGAImage::ConvertRLEBGR16ToRGB24(std::vector<u8>& source,
                                                                                              const u32 width, const u32 height)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * 3);

	u32 index = 0;
	for (u32 i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 3 + l) > (width * height * 3))
			count = (width * height * 3 - l) / 3;

		for (u32 j = 0; j < count; j++)
		{
			data[index++] = NumericCast<u8>(source[i + 1u] << 1u) & 0xF8u;
			data[index++] = NumericCast<u8>(NumericCast<u8>(source[i + 1u] << 6u) | NumericCast<u8>(source[i] >> 2u)) & 0xF8u;
			data[index++] = NumericCast<u8>(source[i] << 3u) & 0xF8u;

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

[[nodiscard]] constexpr std::vector<u8> TRAP::INTERNAL::TGAImage::ConvertRLEBGR24ToRGB24(std::vector<u8>& source,
                                                                                              const u32 width, const u32 height)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * 3);

	u32 index = 0;
	for (u32 i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 3 + l) > (width * height * 3))
			count = (width * height * 3 - l) / 3;

		for (u32 j = 0; j < count; j++)
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

[[nodiscard]] constexpr std::vector<u8> TRAP::INTERNAL::TGAImage::ConvertRLEBGRA32ToRGBA(std::vector<u8>& source,
                                                                                              const u32 width, const u32 height)
{
	std::vector<u8> data{};
	data.resize(NumericCast<usize>(width) * height * 4);

	u32 index = 0;
	for (u32 i = 0, l = 0; i < source.size();)
	{
		//Pixels encoded in "packets"
		//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
		//If RAW, the next count channel-byte color values in the file are taken verbatim
		//If RLE, the next single channel-byte color value speaks for the next count pixels

		const bool raw = (source[i] & 0x80u) == 0; //Is this packet RAW pixels or a repeating color
		u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
		i++;

		//Prevent from writing out of data range
		if ((count * 4 + l) > (width * height * 4))
			count = (width * height * 4 - l) / 4;

		for (u32 j = 0; j < count; j++)
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