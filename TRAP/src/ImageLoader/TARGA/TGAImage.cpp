#include "TRAPPCH.h"
#include "TGAImage.h"

#include "FileSystem/FileSystem.h"
#include "Utils/ImageUtils.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Utils/Expected.h"

namespace
{
	enum class ColorMapType : u8
	{
		None,
		ColorMapped
	};

	enum class ImageType : u8
	{
		NoData = 0u,
		UncompressedColorMapped = 1u,
		UncompressedRGB = 2u,
		UncompressedGrayScale = 3u,
		RLEColorMapped = 9u,
		RLERGB = 10u,
		RLEGrayScale = 11u
	};

	struct Header
	{
		u8 IDLength = 0u; //0 = No ImageID
		ColorMapType ColorMapping = ColorMapType::None;
		ImageType Type = ImageType::NoData;

		//ColorMap spec
		u16 ColorMapOffset = 0u; //Offset to first color map entry
		u16 NumOfColorMapEntries = 0u; //Number of entries in the color map
		u8 ColorMapEntryBitSize = 0u; //Number of bits in each color map entry

		//Image spec
		u16 XOffset = 0u;
		u16 YOffset = 0u;
		u16 Width = 0u;
		u16 Height = 0u;
		u8 BitsPerPixel = 0u;

		bool NeedsXFlip = false;
		bool NeedsYFlip = false;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decode run length encoded indexed BGRA data.
	/// Output format depends on channel count, if it is 4, output is RGBA, if it is 3, output is RGB and so on.
	/// @param source Run length encoded indexed BGRA data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @param channels Amount of channels, i.e. 4 = RGBA, 3 = RGB and so on.
	/// @param colorMap Color table.
	/// @return Decoded pixel data.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLEBGRAMap(const std::span<const u8> source, const u32 width,
	                                                         const u32 height, const u32 channels,
															 const std::span<const u8> colorMap)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height * channels);

		u32 index = 0u;
		for (u32 i = 0u, l = 0u; i < source.size();)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channels-byte color values in the file are taken verbatim
			//If RLE, the next single channels-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0u; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			++i;

			//Prevent from writing out of data range
			if ((count * channels + l) > (width * height * channels))
				count = (width * height * channels - l) / channels;

			for (u32 j = 0u; j < count; j++)
			{
				if (channels == 1u)
				{
					data[index++] = colorMap[NumericCast<usize>(source[i]) * channels];
					++l;
				}
				else if (channels == 2u)
				{
					data[index++] = NumericCast<u8>(colorMap[source[i] * channels + 1u] << 1u) & 0xF8u;
					data[index++] = NumericCast<u8>(NumericCast<u8>((colorMap[source[i] * channels + 1u]) << 6u) |
													NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] >> 2u)) & 0xF8u;
					data[index++] = NumericCast<u8>(colorMap[NumericCast<usize>(source[i]) * channels] << 3u) & 0xF8u;
					l += 2u;
				}
				else if (channels == 3u)
				{
					data[index++] = colorMap[source[i] * channels + 2u];
					data[index++] = colorMap[source[i] * channels + 1u];
					data[index++] = colorMap[source[i] * channels + 0u];
					l += 3u;
				}
				else if (channels == 4u)
				{
					data[index++] = colorMap[source[i] * channels + 2u];
					data[index++] = colorMap[source[i] * channels + 1u];
					data[index++] = colorMap[source[i] * channels + 0u];
					data[index++] = colorMap[source[i] * channels + 3u];
					l += 4u;
				}

				if (raw) //In RAW mode, keep advancing to subsequent values
					++i; //In RLE mode, just repeat the packet[1] color
			}
			if (!raw) //After outputting count values, advance if RLE
				++i;
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decode run length encoded grayscale data.
	/// @param source Run length encoded grayscale data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded grayscale pixel data.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLEGrayScale(const std::span<const u8> source, const u32 width,
	                                                           const u32 height)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height);

		u32 index = 0u;
		for (u32 i = 0u, l = 0u; i < source.size();)
		{
			//Pixels encoded in "packets"

			//First byte is RAW/RLE flag(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count byte color values in the file are taken verbatim
			//If RLE, the next single byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0u; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			++i;

			//Prevent from writing out of data range
			if ((count + l) > (width * height))
				count = width * height - l;

			for (u32 j = 0u; j < count; ++j)
			{
				data[index++] = source[i];

				if (raw) //In RAW mode, keep advancing to subsequent values
					++i; //In RLE mode, just repeat the packet[1] color
				++l;
			}
			if (!raw) //After outputting count values, advance if RLE
				++i;
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert run length encoded BGR16 data to RGB24.
	/// @param source Run length encoded BGR16 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGB24 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGR16ToRGB24(const std::span<const u8> source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height * 3u);

		u32 index = 0u;
		for (u32 i = 0u, l = 0u; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0u; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			++i;

			//Prevent from writing out of data range
			if ((count * 3u + l) > (width * height * 3u))
				count = (width * height * 3u - l) / 3u;

			for (u32 j = 0u; j < count; ++j)
			{
				data[index++] = NumericCast<u8>(source[i + 1u] << 1u) & 0xF8u;
				data[index++] = NumericCast<u8>(NumericCast<u8>(source[i + 1u] << 6u) | NumericCast<u8>(source[i] >> 2u)) & 0xF8u;
				data[index++] = NumericCast<u8>(source[i] << 3u) & 0xF8u;

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 2u; //IN RLE mode, just repeat the packet[1] RGB color
				l += 3u;
			}
			if (!raw) //After outputting count RGB values, advance if RLE
				i += 2u;
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert run length encoded BGR24 data to RGB24.
	/// @param source Run length encoded BGR24 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGB24 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGR24ToRGB24(const std::span<const u8> source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height * 3u);

		u32 index = 0u;
		for (u32 i = 0u, l = 0u; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0u; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			++i;

			//Prevent from writing out of data range
			if ((count * 3u + l) > (width * height * 3u))
				count = (width * height * 3u - l) / 3u;

			for (u32 j = 0u; j < count; ++j)
			{
				data[index++] = source[i + 2u]; //Red
				data[index++] = source[i + 1u]; //Green
				data[index++] = source[i + 0u]; //Blue

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 3u; //IN RLE mode, just repeat the packet[1] RGB color
				l += 3u;
			}
			if (!raw) //After outputting count RGB values, advance if RLE
				i += 3u;
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert run length encoded BGRA32 data to RGBA32.
	/// @param source Run length encoded BGRA32 data.
	/// @param width Width of the image.
	/// @param height Height of the image.
	/// @return Decoded RGBA32 pixel data.
	[[nodiscard]] constexpr std::vector<u8> ConvertRLEBGRA32ToRGBA(const std::span<const u8> source, const u32 width,
	                                                               const u32 height)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height * 4u);

		u32 index = 0u;
		for (u32 i = 0u, l = 0u; i < source.size();)
		{
			//Pixels encoded in "packets"
			//First byte is RAW/RLE flags(upper bit) and count(1-128 as 0-127 in lower 7 bits)
			//If RAW, the next count channel-byte color values in the file are taken verbatim
			//If RLE, the next single channel-byte color value speaks for the next count pixels

			const bool raw = (source[i] & 0x80u) == 0u; //Is this packet RAW pixels or a repeating color
			u32 count = (source[i] & 0x7Fu) + 1u; //How many RAW pixels or color repeats
			++i;

			//Prevent from writing out of data range
			if ((count * 4u + l) > (width * height * 4u))
				count = (width * height * 4u - l) / 4u;

			for (u32 j = 0u; j < count; ++j)
			{
				data[index++] = source[i + 2u]; //Red
				data[index++] = source[i + 1u]; //Green
				data[index++] = source[i + 0u]; //Blue
				data[index++] = source[i + 3u]; //Alpha

				if (raw) //In RAW mode, keep advancing to subsequent values
					i += 4u; //IN RLE mode, just repeat the packet[1] RGBA color
				l += 4u;
			}
			if (!raw) //After outputting count RGBA values, advance if RLE
				i += 4u;
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool IsValidColorMapType(const ColorMapType colorMapType) noexcept
	{
		return colorMapType == ColorMapType::None || colorMapType == ColorMapType::ColorMapped;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool IsValidImageType(const ImageType imageType) noexcept
	{
		return imageType == ImageType::NoData || imageType == ImageType::UncompressedColorMapped ||
		       imageType == ImageType::UncompressedRGB || imageType == ImageType::UncompressedGrayScale ||
		       imageType == ImageType::RLEColorMapped || imageType == ImageType::RLERGB ||
			   imageType == ImageType::RLEGrayScale;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool IsValidColorMapEntryBitSize(const u8 colorMapEntryBitSize) noexcept
	{
		return colorMapEntryBitSize == 15u || colorMapEntryBitSize == 16u ||
		       colorMapEntryBitSize == 24u || colorMapEntryBitSize == 32u;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool IsValidBitsPerPixel(const u8 bitsPerPixel) noexcept
	{
		return bitsPerPixel == 8u || bitsPerPixel == 16u || bitsPerPixel == 24u || bitsPerPixel == 32u;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, std::string> ReadHeader(std::ifstream& file)
	{
		Header header{};

		header.IDLength = NumericCast<u8>(file.get());
		header.ColorMapping = static_cast<ColorMapType>(NumericCast<u8>(file.get()));
		header.Type = static_cast<ImageType>(NumericCast<u8>(file.get()));
		file.read(reinterpret_cast<char*>(&header.ColorMapOffset), sizeof(u16));
		file.read(reinterpret_cast<char*>(&header.NumOfColorMapEntries), sizeof(u16));
		header.ColorMapEntryBitSize = NumericCast<u8>(file.get());
		file.read(reinterpret_cast<char*>(&header.XOffset), sizeof(u16));
		file.read(reinterpret_cast<char*>(&header.YOffset), sizeof(u16));
		file.read(reinterpret_cast<char*>(&header.Width), sizeof(u16));
		file.read(reinterpret_cast<char*>(&header.Height), sizeof(u16));
		header.BitsPerPixel = NumericCast<u8>(file.get());

		const std::bitset<8u> imageDescriptorBits(NumericCast<u8>(file.get()));
		const bool isTopRight = imageDescriptorBits.test(4u) && imageDescriptorBits.test(5u);
		const bool isBottomRight = imageDescriptorBits.test(4u) && !imageDescriptorBits.test(5u);
		const bool isBottomLeft = !imageDescriptorBits.test(4u) && !imageDescriptorBits.test(5u);

		if(isTopRight || isBottomRight)
			header.NeedsXFlip = true;
		if(isBottomLeft || isBottomRight)
			header.NeedsYFlip = true;

		//File uses little-endian
		//Convert to machines endian
		if constexpr (TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Little)
		{
			TRAP::Utils::Memory::SwapBytes(header.ColorMapOffset);
			TRAP::Utils::Memory::SwapBytes(header.NumOfColorMapEntries);
			TRAP::Utils::Memory::SwapBytes(header.XOffset);
			TRAP::Utils::Memory::SwapBytes(header.YOffset);
			TRAP::Utils::Memory::SwapBytes(header.Width);
			TRAP::Utils::Memory::SwapBytes(header.Height);
		}

		//Validate header data

		if(!IsValidColorMapType(header.ColorMapping))
			return TRAP::MakeUnexpected("ColorMapType is invalid, expected 0 or 1!");
		if(!IsValidImageType(header.Type))
			return TRAP::MakeUnexpected("ImageType is invalid, expected 0, 1, 2, 3, 9, 10, 11!");
		if(header.ColorMapping == ColorMapType::ColorMapped && !IsValidColorMapEntryBitSize(header.ColorMapEntryBitSize))
			return TRAP::MakeUnexpected("Color map entry size is invalid, expected 16, 24, 32!");
		if(!IsValidBitsPerPixel(header.BitsPerPixel))
			return TRAP::MakeUnexpected("Bits per pixel is invalid, expected 8, 16, 24, 32!");
		if (header.Width < 2u)
			return TRAP::MakeUnexpected("Image width is invalid or unsupported!");
		if (header.Height < 2u)
			return TRAP::MakeUnexpected("Image height is invalid or unsupported!");

		if (header.Type == ImageType::NoData)
			return TRAP::MakeUnexpected("Image doesn't contain pixel data!");

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, std::string> ReadColorMap(std::ifstream& file, const Header& header)
	{
		if(header.ColorMapping == ColorMapType::None)
			return TRAP::MakeUnexpected("Tried to read color map while ColorMapType is None!");

		std::vector<u8> colorMap(NumericCast<usize>(header.ColorMapEntryBitSize / 8u) * header.NumOfColorMapEntries);

		if(!file.read(reinterpret_cast<char*>(colorMap.data()), NumericCast<std::streamsize>(colorMap.size())))
			return TRAP::MakeUnexpected("Failed to load color map!");

		return colorMap;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct Footer
	{
		u32 ExtensionAreaOffset = 0u;
		u32 DeveloperDirectoryOffset = 0u;
		std::size_t FooterOffset = 0u;
		std::string Signature = std::string(17, '\0');
	};

	[[nodiscard]] TRAP::Optional<Footer> ReadFooter(std::ifstream& file)
	{
		const auto startPos = file.tellg();

		Footer footer{};

		file.seekg(-26, std::ios::end);
		footer.FooterOffset = file.tellg();

		file.read(reinterpret_cast<char*>(&footer.ExtensionAreaOffset), sizeof(u32));
		file.read(reinterpret_cast<char*>(&footer.DeveloperDirectoryOffset), sizeof(u32));
		file.read(footer.Signature.data(), NumericCast<std::streamsize>(footer.Signature.size()));

		file.seekg(startPos);

		if(footer.Signature == "TRUEVISION-XFILE.")
			return footer;

		return TRAP::NullOpt;
	}

	[[nodiscard]] std::size_t GetRLEPixelDataEndOffset(std::ifstream& file)
	{
		const auto footer = ReadFooter(file);
		if(!footer)
		{
			const auto currPos = file.tellg();
			file.seekg(0, std::ios::end);
			const auto endPos = file.tellg();
			file.seekg(currPos);

			return endPos;
		}

		std::size_t endPos = footer->FooterOffset;
		if(footer->ExtensionAreaOffset != 0u)
			endPos = footer->ExtensionAreaOffset;
		else if(footer->DeveloperDirectoryOffset != 0u)
			endPos = footer->DeveloperDirectoryOffset;

		return endPos;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, std::string> ReadPixelData(std::ifstream& file, const Header& header)
	{
		std::vector<u8> rawPixelData{};

		if(header.Type == ImageType::RLEColorMapped ||
		   header.Type == ImageType::RLEGrayScale ||
		   header.Type == ImageType::RLERGB) //All RLE formats
		{
			rawPixelData.resize(GetRLEPixelDataEndOffset(file));
			if (!file.read(reinterpret_cast<char*>(rawPixelData.data()), NumericCast<std::streamsize>(rawPixelData.size())))
				return TRAP::MakeUnexpected("Failed to read pixel data!");
		}
		else //Uncompressed
		{
			rawPixelData.resize(NumericCast<usize>(header.Width) * header.Height * (header.BitsPerPixel / 8u));
			if (!file.read(reinterpret_cast<char*>(rawPixelData.data()), NumericCast<std::streamsize>(rawPixelData.size())))
				return TRAP::MakeUnexpected("Failed to read pixel data!");
		}

		return rawPixelData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct FileData
	{
		Header FileHeader{};
		std::vector<u8> ColorMap{};
		std::vector<u8> EncodedPixelData{};
	};

	[[nodiscard]] TRAP::Expected<FileData, std::string> LoadDataFromFile(const std::filesystem::path& filePath)
	{
		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open())
			return TRAP::MakeUnexpected("Failed to open file for reading");

		const auto header = ReadHeader(file);
		if(!header)
			return TRAP::MakeUnexpected(header.Error());

		if (header->IDLength != 0u)
			file.seekg(header->IDLength, std::ios::cur); //Skip unused image identification field

		std::vector<u8> colorMapData{};
		if(header->ColorMapping == ColorMapType::ColorMapped)
		{
			if(auto colorMap = ReadColorMap(file, *header))
				colorMapData = std::move(*colorMap);
			else
				return TRAP::MakeUnexpected(colorMap.Error());
		}

		const auto encodedPixelData = ReadPixelData(file, *header);
		if(!encodedPixelData)
			return TRAP::MakeUnexpected(encodedPixelData.Error());

		return FileData{*header, colorMapData, *encodedPixelData};
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct DecodedData
	{
		u32 Width{};
		u32 Height{};
		u32 BitsPerPixel{};
		TRAP::Image::ColorFormat Format = TRAP::Image::ColorFormat::RGBA;
		std::vector<u8> PixelData{};
	};

	[[nodiscard]] constexpr TRAP::Expected<DecodedData, std::string> DecodePixelData(const FileData& fileData)
	{
		const auto& header = fileData.FileHeader;
		const auto& colorMapData = fileData.ColorMap;
		const auto& encodedPixelData = fileData.EncodedPixelData;

		DecodedData decodedData{};
		decodedData.Width = header.Width;
		decodedData.Height = header.Height;
		decodedData.BitsPerPixel = header.BitsPerPixel;

		switch (header.Type)
		{
		case ImageType::UncompressedColorMapped:
		{
			if (header.BitsPerPixel != 8u)
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");

			decodedData.PixelData = TRAP::Utils::DecodeBGRAMappedPixelData(encodedPixelData, header.Width, header.Height,
								                                           header.ColorMapEntryBitSize / 8u, colorMapData);
			decodedData.BitsPerPixel = header.ColorMapEntryBitSize;
			if(decodedData.BitsPerPixel == 16u)
				decodedData.BitsPerPixel = 24u;
			break;
		}

		case ImageType::UncompressedRGB:
		{
			if(header.BitsPerPixel == 16u)
			{
				decodedData.BitsPerPixel = 24u;
				decodedData.PixelData = TRAP::Utils::ConvertBGR16PixelDataToRGB24(encodedPixelData, header.Width, header.Height);
			}
			else if(header.BitsPerPixel == 24u)
				decodedData.PixelData = TRAP::Utils::ConvertBGR24PixelDataToRGB24(encodedPixelData, header.Width, header.Height);
			else if(header.BitsPerPixel == 32u)
				decodedData.PixelData = TRAP::Utils::ConvertBGRA32PixelDataToRGBA32(encodedPixelData, header.Width, header.Height);
			else
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");
			break;
		}

		case ImageType::UncompressedGrayScale:
		{
			if (header.BitsPerPixel != 8u)
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");

			decodedData.PixelData = std::vector<u8>(encodedPixelData.begin(), encodedPixelData.end());
			break;
		}

		case ImageType::RLEColorMapped:
		{
			if (header.BitsPerPixel != 8u)
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");

			decodedData.PixelData = DecodeRLEBGRAMap(encodedPixelData, header.Width, header.Height,
									                 header.ColorMapEntryBitSize / 8u, colorMapData);
			decodedData.BitsPerPixel = header.ColorMapEntryBitSize;
			if(decodedData.BitsPerPixel == 16u)
				decodedData.BitsPerPixel = 24u;
			break;
		}

		case ImageType::RLERGB:
		{
			if(header.BitsPerPixel == 16u)
			{
				decodedData.BitsPerPixel = 24u;
				decodedData.PixelData = ConvertRLEBGR16ToRGB24(encodedPixelData, header.Width, header.Height);
			}
			else if(header.BitsPerPixel == 24u)
				decodedData.PixelData = ConvertRLEBGR24ToRGB24(encodedPixelData, header.Width, header.Height);
			else if(header.BitsPerPixel == 32u)
				decodedData.PixelData = ConvertRLEBGRA32ToRGBA(encodedPixelData, header.Width, header.Height);
			else
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");
			break;
		}

		case ImageType::RLEGrayScale:
		{
			if (header.BitsPerPixel != 8u)
				return TRAP::MakeUnexpected("Invalid bits per pixel detected!");

			decodedData.PixelData = DecodeRLEGrayScale(encodedPixelData, header.Width, header.Height);
			break;
		}

		case ImageType::NoData: //Shouldn't be reached because of the above check!
			return TRAP::MakeUnexpected("Image doesn't contain pixel data!");
		}

		decodedData.Format = static_cast<TRAP::Image::ColorFormat>(decodedData.BitsPerPixel / 8u);

		if (header.NeedsXFlip)
			decodedData.PixelData = TRAP::Utils::FlipPixelDataX<u8>(header.Width, header.Height, decodedData.BitsPerPixel / 8u, decodedData.PixelData);
		if (header.NeedsYFlip)
			decodedData.PixelData = TRAP::Utils::FlipPixelDataY<u8>(header.Width, header.Height, decodedData.BitsPerPixel / 8u, decodedData.PixelData);

		return decodedData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::TGAImage::TGAImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImageTGAPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	const auto decodedData = LoadDataFromFile(m_filepath).AndThen(DecodePixelData);
	if(!decodedData)
	{
		TP_ERROR(Log::ImageBMPPrefix, decodedData.Error());
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	m_width = decodedData->Width;
	m_height = decodedData->Height;
	m_bitsPerPixel = decodedData->BitsPerPixel;
	m_colorFormat = decodedData->Format;
	m_data = decodedData->PixelData;
}
