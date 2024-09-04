#include "TRAPPCH.h"
#include "BMPImage.h"

#include "FileSystem/FileSystem.h"
#include "Maths/Math.h"
#include "Utils/ImageUtils.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Utils/Expected.h"

namespace
{
	enum class BMPErrorCode
	{
		InvalidBitField,
		InvalidBitFieldMaskParameter,
		FailedReadingBitmapFileHeader,
		InvalidMagicNumber,
		FailedReadingDIBHeader,
		UnsupportedOS21X,
		UnsupportedOS22X,
		InvalidNumberOfPlanes,
		UnsupportedBPP,
		InvalidCompression,
		UnsupportedRLE4,
		UnsupportedPNG,
		UnsupportedJPEG,
		UnsupportedCMYK,
		InvalidWidth,
		InvalidHeight,
		InvalidImageSizeForUncompressed,
		InvalidBitPerPixelForRLE8,
		FailedReadingBitFieldMasks,
		InvalidColorTableSize,
		FailedReadingColorTable,
		FailedReadingEncodedPixelData,
		FailedToOpenFile,
		FailedRLE8Decompression
	};

	[[nodiscard]] constexpr std::string BMPErrorCodeToString(const BMPErrorCode errorCode)
	{
		switch(errorCode)
		{
		case BMPErrorCode::InvalidBitField:
			return "Invalid bitfield detected!";

		case BMPErrorCode::InvalidBitFieldMaskParameter:
			return "Invalid masks parameter size, expected 3 or 4!";

		case BMPErrorCode::FailedReadingBitmapFileHeader:
			return "Failed to read bitmap file header!";

		case BMPErrorCode::InvalidMagicNumber:
			return "Invalid or unsupported magic number found, expected \"BM\"!";

		case BMPErrorCode::FailedReadingDIBHeader:
			return "Failed to read DIB header!";

		case BMPErrorCode::UnsupportedOS21X:
			return "OS/2 1.x BMPs are unsupported!";

		case BMPErrorCode::UnsupportedOS22X:
			return "OS/2 2.x BMPs are unsupported!";

		case BMPErrorCode::InvalidNumberOfPlanes:
			return "Invalid number of planes found, expected 1!";

		case BMPErrorCode::UnsupportedBPP:
			return "Unsupported bits per pixel detected, only 8, 16, 24, 32 bits per pixel images are supported!";

		case BMPErrorCode::InvalidCompression:
			return "Invalid compressin type!";

		case BMPErrorCode::UnsupportedRLE4:
			return "RLE4 encoded BMP images are unsupported!";

		case BMPErrorCode::UnsupportedPNG:
			return "PNG encoded BMP images are unsupported!";

		case BMPErrorCode::UnsupportedJPEG:
			return "JPEG encoded BMP images are unsupported!";

		case BMPErrorCode::UnsupportedCMYK:
			return "CMYK/CMYKRLE8/CMYKRLE4 encoded BMP images are unsupported!";

		case BMPErrorCode::InvalidWidth:
			return "Invalid width detected!";

		case BMPErrorCode::InvalidHeight:
			return "Invalid height detected!";

		case BMPErrorCode::InvalidImageSizeForUncompressed:
			return "Image size 0 is invalid for compressed BMP images!";

		case BMPErrorCode::InvalidBitPerPixelForRLE8:
			return "RLE8 compressed image must have 8 bits per pixel!";

		case BMPErrorCode::FailedReadingBitFieldMasks:
			return "Failed to read bitfield masks!";

		case BMPErrorCode::InvalidColorTableSize:
			return "Invalid color table size detected!";

		case BMPErrorCode::FailedReadingColorTable:
			return "Failed to read color table!";

		case BMPErrorCode::FailedReadingEncodedPixelData:
			return "Failed to read encoded pixel data!";

		case BMPErrorCode::FailedToOpenFile:
			return "Failed to open file for reading";

		case BMPErrorCode::FailedRLE8Decompression:
			return "Failed to decompress RLE8 pixel data!";
		}

		return "";
	}

	//-------------------------------------------------------------------------------------------------------------------//

	enum class BMPCompression : u32
	{
		Uncompressed = 0u,
		RLE8 = 1u,
		RLE4 = 2u,
		BitFields = 3u,
		JPEG = 4u,
		PNG = 5u,
		AlphaBitFields = 6u,
		CMYK = 11u,
		CMYKRLE8 = 12u,
		CMYKRLE4 = 13u
	};
	MAKE_ENUM_FLAG(BMPCompression);

	//-------------------------------------------------------------------------------------------------------------------//

	struct BitField
	{
		u32 Start;
		u32 Span;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	struct Header
	{
		std::array<char, 2u> MagicNumber{}; //Used to check format
		// u32 Size = 0u; //File size in bytes (unused)
		// u16 Reserved0 = 0u; //Reserved (unused)
		// u16 Reserved1 = 0u; //Reserved (unused)
		u32 DataOffset = 0u; //Offset from file start to first pixel data
	};

	//-------------------------------------------------------------------------------------------------------------------//

	struct InfoHeader
	{
		u32 HeaderSize = 0u; //Size of this header in bytes
		u32 Width = 0u;
		i32 Height = 0;
		u16 Planes = 0u; //Always 1
		u16 BitsPerPixel = 0u; //1, 4, 8, 16, 24, 32
		BMPCompression Compression = BMPCompression::Uncompressed;
		u32 SizeImage = 0u; //Size of the image in bytes
		//i32 XPixelsPerMeter = 0; //Unused
		//i32 YPixelsPerMeter = 0; //Unused
		u32 CLRUsed = 0u; //Amount of colors in palette
		//u32 CLRImportant = 0u; //Amount of important colors in palette (Unused)
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert given value to 8 bits.
	/// @param value Value to convert.
	/// @param bitSpan Bit span.
	/// @return Value as 8 bits.
	[[nodiscard]] constexpr u8 Make8Bits(u32 value, const u32 bitSpan) noexcept
	{
		u32 output = 0u;

		if (bitSpan == 8u)
			return NumericCast<u8>(value);
		if (bitSpan > 8u)
			return NumericCast<u8>(value >> (bitSpan - 8u));

		value <<= (8u - bitSpan); //Shift it up into the most significant bits.
		while(value != 0u)
		{
			output |= value;
			value >>= bitSpan;
		}

		return NumericCast<u8>(output);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Applies a bit field on the given value.
	/// @param x Value to apply bit field to.
	/// @param bitField Bit field to apply.
	/// @return Value with bit field applied.
	[[nodiscard]] constexpr u32 ApplyBitField(const u32 x, const BitField& bitField) noexcept
	{
		return x >> bitField.Start & (BIT(bitField.Span) - 1u);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Parse a bit field.
	/// @param mask Mask.
	/// @return BitField if parsed successfully, error otherwise.
	[[nodiscard]] constexpr TRAP::Expected<BitField, BMPErrorCode> ParseBitfield(const u32 mask) noexcept
	{
		u32 bit = 0u;
		for (; bit < 32u && ((mask & BIT(bit)) == 0u); ++bit);

		if(bit >= 32u)
		{
			//Absent BitMasks are valid.
			return BitField{};
		}

		BitField result{};

		result.Start = bit;
		for (; bit < 32u && ((mask & BIT(bit)) != 0u); ++bit);
		result.Span = bit - result.Start;

		//If there are more set bits, there was a gap, which is invalid
		if(bit >= 32u || ((mask & ~(BIT(bit) - 1u)) == 0u))
			return result;

		return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitField);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Validate bit fields via the given masks.
	/// @param masks Mask to validate the bit fields against.
	/// @param bitPerPixel Bits per pixel used by the image.
	/// @return Validated BitFields on success, error otherwise.
	[[nodiscard]] constexpr TRAP::Expected<std::vector<BitField>, BMPErrorCode> ValidateBitFields(const std::span<const u32> masks,
												                                                  const u32 bitsPerPixel) noexcept
	{
		if(masks.size() != 3u && masks.size() != 4u)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitFieldMaskParameter);

		std::vector<BitField> outBitFields(masks.size());

		u32 totalMask = 0u;
		BitField totalField{};

		for(usize i = 0u; i < outBitFields.size(); ++i)
		{
			//No overlapping masks.
			if ((totalMask & masks[i]) != 0u)
				return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitField);

			totalMask |= masks[i];

			if (const auto bitField = ParseBitfield(masks[i]))
				outBitFields[i] = *bitField;
			else
				return TRAP::Unexpected(bitField.Error());

			//Make sure it fits in bit size
			if (outBitFields[i].Start + outBitFields[i].Span > bitsPerPixel)
				return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitField);
		}

		if (totalMask == 0u)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitField);

		//Check for contiguous-ity between fields, too.
		if (const auto bitField = ParseBitfield(totalMask))
			totalField = *bitField;
		else
			return TRAP::Unexpected(bitField.Error());

		return outBitFields;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decode run length encoded 8-bit RGBA BMP data.
	/// @param width Width of the uncompressed pixel data.
	/// @param height Height of the uncompressed pixel data.
	/// @param compressedImageData Compressed image data.
	/// @param colorTable Color table.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLE8RGBA(const u32 width, const u32 height,
	                                                       const std::span<const u8> compressedImageData,
														   const std::span<const u8> colorTable)
	{
		constexpr u32 channels = 4u;

		std::vector<u8> uncompressedPixelData(NumericCast<usize>(width) * height * channels);

		constexpr u8 EndOfScanline = 0u;
		constexpr u8 EndMarker = 1u;
		constexpr u8 DeltaMarker = 2u;

		u32 x = 0u, y = 0u;

		u32 dataIndex = 0u;

		//Compressed RGBA
		while (dataIndex < compressedImageData.size())
		{
			const u8 firstByte = compressedImageData[dataIndex++];

			if (firstByte != 0u) //Run mode
			{
				const u8 secondByte = compressedImageData[dataIndex++] * channels;

				for (u8 t = 0u; t < firstByte; t++) //Copy pixel data (also converts BGRA to RGBA)
				{
					uncompressedPixelData[((y * width + x) * channels) + 0u] = (colorTable)[secondByte + 2u];
					uncompressedPixelData[((y * width + x) * channels) + 1u] = (colorTable)[secondByte + 1u];
					uncompressedPixelData[((y * width + x) * channels) + 2u] = (colorTable)[secondByte + 0u];
					uncompressedPixelData[((y * width + x) * channels) + 3u] = (colorTable)[secondByte + 3u];
					x++;
				}
			}
			else
			{
				const u8 secondByte = compressedImageData[dataIndex++];

				if (secondByte == EndOfScanline)
				{
					x = 0u;
					y++;
					continue;
				}
				if (secondByte == DeltaMarker)
				{
					x += compressedImageData[dataIndex++];
					y += compressedImageData[dataIndex++];
					continue;
				}

				if (secondByte == EndMarker)
					return uncompressedPixelData;

				//Absolute mode (literal pixel values, not encoded)
				for (u8 t = 0u; t < secondByte; t++) //Copy pixel data (also converts BGRA to RGBA)
				{
					const u8 pixel = compressedImageData[dataIndex++] * channels;
					uncompressedPixelData[((y * width + x) * channels) + 0u] = (colorTable)[pixel + 2u];
					uncompressedPixelData[((y * width + x) * channels) + 1u] = (colorTable)[pixel + 1u];
					uncompressedPixelData[((y * width + x) * channels) + 2u] = (colorTable)[pixel + 0u];
					uncompressedPixelData[((y * width + x) * channels) + 3u] = (colorTable)[pixel + 3u];
					x++;
				}

				//Check for padding byte
				if (secondByte % 2u != 0u)
					dataIndex++;
			}
		}

		return {};
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Decode run length encoded 8-bit BMP data.
	/// @param width Width of the uncompressed pixel data.
	/// @param height Height of the uncompressed pixel data.
	/// @param compressedImageData Compressed image data.
	[[nodiscard]] constexpr std::vector<u8> DecodeRLE8GrayScale(const uint32_t width, const uint32_t height,
	                                                            const std::span<const u8> compressedImageData)
	{
		std::vector<u8> uncompressedPixelData(NumericCast<usize>(width) * height * 1u);

		constexpr u8 EndOfScanline = 0u;
		constexpr u8 EndMarker = 1u;
		constexpr u8 DeltaMarker = 2u;

		u32 x = 0u, y = 0u;

		u32 dataIndex = 0u;

		//Compressed Grayscale
		while (dataIndex < compressedImageData.size())
		{
			const u8 firstByte = compressedImageData[dataIndex++];

			if (firstByte != 0u) //Run mode
			{
				const u8 secondByte = compressedImageData[dataIndex++];

				for (u8 t = 0u; t < firstByte; t++)
				{
					uncompressedPixelData[x + (y * width)] = secondByte;
					x++;
				}
			}
			else
			{
				const u8 secondByte = compressedImageData[dataIndex++];

				if (secondByte == EndOfScanline)
				{
					x = 0u;
					y++;
					continue;
				}
				if (secondByte == DeltaMarker)
				{
					x += compressedImageData[dataIndex++];
					y += compressedImageData[dataIndex++];
					continue;
				}

				if (secondByte == EndMarker)
					return uncompressedPixelData;

				//Absolute mode (literal pixel values, not encoded)
				for (u8 t = 0u; t < secondByte; t++)
				{
					const u8 pixel = compressedImageData[dataIndex++];
					uncompressedPixelData[x + (y * width)] = pixel;
					x++;
				}

				//Check for padding byte
				if (secondByte % 2u != 0u)
					dataIndex++;
			}
		}

		return {};
	}

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	[[nodiscard]] T ReadLittleEndian(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		T data{};

		file.read(reinterpret_cast<char*>(&data), sizeof(T));

		if constexpr(!std::same_as<T, u8> && !std::same_as<T, i8> &&
		             TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Little)
		{
			TRAP::Utils::Memory::SwapBytes(data);
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<Header, BMPErrorCode> LoadHeader(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		Header header{};
		header.MagicNumber = {ReadLittleEndian<char>(file), ReadLittleEndian<char>(file)};
		file.ignore(4); //Discard file size
		file.ignore(2); //Discard reserved
		file.ignore(2); //Discard reserved
		header.DataOffset = ReadLittleEndian<u32>(file);

		if(!file.good())
			return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingBitmapFileHeader);

		if(std::get<0u>(header.MagicNumber) != 'B' || std::get<1u>(header.MagicNumber) != 'M')
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidMagicNumber);

		return header;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr bool IsValidCompression(const BMPCompression compression)
	{
		switch(compression)
		{
		case BMPCompression::Uncompressed:
			[[fallthrough]];
		case BMPCompression::RLE8:
			[[fallthrough]];
		case BMPCompression::RLE4:
			[[fallthrough]];
		case BMPCompression::BitFields:
			[[fallthrough]];
		case BMPCompression::JPEG:
			[[fallthrough]];
		case BMPCompression::PNG:
			[[fallthrough]];
		case BMPCompression::AlphaBitFields:
			[[fallthrough]];
		case BMPCompression::CMYK:
			[[fallthrough]];
		case BMPCompression::CMYKRLE8:
			[[fallthrough]];
		case BMPCompression::CMYKRLE4:
			return true;

		default:
			return false;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<InfoHeader, BMPErrorCode> LoadInfoHeader(std::ifstream& file)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		InfoHeader infoHeader{};

		infoHeader.HeaderSize = ReadLittleEndian<u32>(file);
		infoHeader.Width = ReadLittleEndian<u32>(file);
		infoHeader.Height = ReadLittleEndian<i32>(file);
		infoHeader.Planes = ReadLittleEndian<u16>(file);
		infoHeader.BitsPerPixel = ReadLittleEndian<u16>(file);
		infoHeader.Compression = static_cast<BMPCompression>(ReadLittleEndian<u32>(file));
		infoHeader.SizeImage = ReadLittleEndian<u32>(file);
		file.ignore(4); //Discard XPixelsPerMeter
		file.ignore(4); //Discard YPixelsPerMeter
		infoHeader.CLRUsed = ReadLittleEndian<u32>(file);
		file.ignore(4); //Discard CLRImportant

		if(!file.good())
			return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingDIBHeader);

		if(infoHeader.HeaderSize == 12u)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedOS21X);
		if(infoHeader.HeaderSize == 64u || infoHeader.HeaderSize == 16u)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedOS22X);
		if(infoHeader.Planes != 1u)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidNumberOfPlanes);
		if(infoHeader.BitsPerPixel < 8u)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedBPP);
		if(!IsValidCompression(infoHeader.Compression))
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidCompression);
		if(infoHeader.Compression == BMPCompression::RLE4)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedRLE4);
		if(infoHeader.Compression == BMPCompression::JPEG)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedJPEG);
		if(infoHeader.Compression == BMPCompression::PNG)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedPNG);
		if(infoHeader.Compression == BMPCompression::CMYK || infoHeader.Compression == BMPCompression::CMYKRLE8 || infoHeader.Compression == BMPCompression::CMYKRLE4)
			return TRAP::MakeUnexpected(BMPErrorCode::UnsupportedCMYK);
		if(infoHeader.Width == 0u)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidWidth);
		if(infoHeader.Height == 0)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidHeight);
		if(infoHeader.SizeImage == 0u && infoHeader.Compression != BMPCompression::Uncompressed)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidImageSizeForUncompressed);
		if(infoHeader.Compression == BMPCompression::RLE8 && infoHeader.BitsPerPixel != 8u)
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidBitPerPixelForRLE8);

		return infoHeader;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u32>, BMPErrorCode> LoadBitFieldMasks(std::ifstream& file, const BMPCompression compression)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		switch(compression)
		{
		case BMPCompression::BitFields:
		{
			std::vector<u32> masks{ReadLittleEndian<u32>(file), ReadLittleEndian<u32>(file), ReadLittleEndian<u32>(file)};

			if(!file.good())
				return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingBitFieldMasks);

			return masks;
		}

		case BMPCompression::AlphaBitFields:
		{
			std::vector<u32> masks{ReadLittleEndian<u32>(file), ReadLittleEndian<u32>(file), ReadLittleEndian<u32>(file), ReadLittleEndian<u32>(file)};

			if(!file.good())
				return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingBitFieldMasks);

			return masks;
		}

		default:
			return {};
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	constexpr void FillUnusedAlphaChannel(const std::span<u8> pixelData)
	{
		//Check if alpha is used
		bool alphaUsed = false;
		for (u32 i = 3u; i < pixelData.size(); i += 4u)
		{
			if (pixelData[i] > 0u)
			{
				alphaUsed = true;
				break;
			}
		}

		//If alpha is unused set all alpha bytes to 255 (full white)
		if (!alphaUsed)
		{
			for (u32 i = 3u; i < pixelData.size(); i += 4u)
				pixelData[i] = 255u;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, BMPErrorCode> LoadColorTable(std::ifstream& file, const InfoHeader& infoHeader)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		//Color table is only used by image with <= 8 bits per pixel
		if(infoHeader.BitsPerPixel > 8u)
			return {};

		if(infoHeader.CLRUsed > NumericCast<u32>(TRAP::Math::Pow<f32>(2u, infoHeader.BitsPerPixel))) //Max 2^(bits per pixel) entries allowed
			return TRAP::MakeUnexpected(BMPErrorCode::InvalidColorTableSize);

		std::vector<u8> colorTable(NumericCast<usize>(4u) * infoHeader.CLRUsed);
		if(colorTable.empty()) //When CLRUsed is 0 then default to 2^n (bits per pixel) entries
			colorTable.resize(4u * NumericCast<usize>(TRAP::Math::Pow<f32>(2u, infoHeader.BitsPerPixel)));

		if(!file.read(reinterpret_cast<char*>(colorTable.data()), NumericCast<std::streamsize>(colorTable.size())).good())
			return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingColorTable);

        FillUnusedAlphaChannel(colorTable);

		return colorTable;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] TRAP::Expected<std::vector<u8>, BMPErrorCode> LoadEncodedPixelData(std::ifstream& file,
	                                                                                 const u32 offsetBeginToPixelData,
																					 const InfoHeader& infoHeader)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                             (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

		//Jump to pixel data
		file.seekg(offsetBeginToPixelData, std::ios::beg);

		std::vector<u8> imageData{};
		if(infoHeader.Compression == BMPCompression::Uncompressed) //infoHeader.SizeImage may be 0 for uncompressed images!
		{
			const u32 rowSize = NumericCast<u32>(TRAP::Math::Ceil<f32>(NumericCast<f32>((infoHeader.BitsPerPixel * infoHeader.Width) / 32u)) * 4u);
			const u32 pixelDataSize = rowSize * TRAP::Math::Abs(infoHeader.Height);

			imageData.resize(pixelDataSize);
		}
		else
			imageData.resize(infoHeader.SizeImage);

		if(!file.read(reinterpret_cast<char*>(imageData.data()), NumericCast<std::streamsize>(imageData.size())))
			return TRAP::MakeUnexpected(BMPErrorCode::FailedReadingEncodedPixelData);

		if(infoHeader.Compression == BMPCompression::Uncompressed && infoHeader.BitsPerPixel == 32u)
			FillUnusedAlphaChannel(imageData);

		return imageData;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	struct FileData
	{
		InfoHeader Header{};
		std::vector<u32> BitFieldMasks{};
		std::vector<u8> ColorTable{};
		std::vector<u8> EncodedPixelData{};
	};

	[[nodiscard]] TRAP::Expected<FileData, BMPErrorCode> LoadDataFromFile(const std::filesystem::path& filePath)
	{
		ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open())
			return TRAP::MakeUnexpected(BMPErrorCode::FailedToOpenFile);

		const auto header = LoadHeader(file);
		if(!header)
			return TRAP::Unexpected(header.Error());

		const auto infoHeader = LoadInfoHeader(file);
		if(!infoHeader)
			return TRAP::Unexpected(infoHeader.Error());

		const auto bitfieldMasks = LoadBitFieldMasks(file, infoHeader->Compression);
		if(!bitfieldMasks)
			return TRAP::Unexpected(bitfieldMasks.Error());

		const auto colorTable = LoadColorTable(file, *infoHeader);
		if(!colorTable)
			return TRAP::Unexpected(colorTable.Error());

		const auto encodedPixelData = LoadEncodedPixelData(file, header->DataOffset, *infoHeader);
		if(!encodedPixelData)
			return TRAP::Unexpected(encodedPixelData.Error());

		return FileData{*infoHeader, *bitfieldMasks, *colorTable, *encodedPixelData};
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

	//-------------------------------------------------------------------------------------------------------------------//

	template<typename T>
	requires std::same_as<T, u16> || std::same_as<T, u32>
	[[nodiscard]] constexpr std::vector<u8> DecodeBitFieldPixelData(const std::span<const u8> encodedPixelData,
	                                                                const std::span<const BitField> bitFields,
																	const u32 width, const u32 height,
																	const u32 bitsPerPixel)
	{
		std::vector<u8> data(NumericCast<usize>(width) * height * 4u);

		u32 index = 0u;
		for (u32 i = 0u; i < width * height * bitsPerPixel / 8u - 1u;)
		{
			T value{};

			if constexpr(std::same_as<T, u16>)
			{
				value = NumericCast<u16>(encodedPixelData[i]) +
					    NumericCast<u16>(encodedPixelData[i + 1u] << 8u);
			}
			else /*if constexpr(std::same_as<T, u32>)*/
			{
				value =  NumericCast<u32>(encodedPixelData[i]) +
						(NumericCast<u32>(encodedPixelData[i + 1u]) << 8u) +
						(NumericCast<u32>(encodedPixelData[i + 2u]) << 16u) +
						(NumericCast<u32>(encodedPixelData[i + 3u]) << 24u);
			}

			data[index++] = Make8Bits(ApplyBitField(value, bitFields[0u]), bitFields[0u].Span);
			data[index++] = Make8Bits(ApplyBitField(value, bitFields[1u]), bitFields[1u].Span);
			data[index++] = Make8Bits(ApplyBitField(value, bitFields[2u]), bitFields[2u].Span);
			if (bitFields.size() == 4u && bitFields[3u].Span != 0u)
				data[index++] = Make8Bits(ApplyBitField(value, bitFields[3u]), bitFields[3u].Span);
			else
				data[index++] = 255u;

			i += sizeof(T);
		}

		return data;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Expected<DecodedData, BMPErrorCode> DecodePixelData(const FileData& fileData)
	{
		const InfoHeader& infoHeader = fileData.Header;
		const std::vector<u32>& bitfieldMasks = fileData.BitFieldMasks;
		const std::vector<u8>& colorTable = fileData.ColorTable;
		const std::vector<u8>& encodedPixelData = fileData.EncodedPixelData;

		DecodedData decodedData
		{
			.Width = infoHeader.Width,
			.Height = NumericCast<u32>(TRAP::Math::Abs<i32>(infoHeader.Height)),
			.BitsPerPixel = infoHeader.BitsPerPixel
		};

		if (infoHeader.Compression == BMPCompression::Uncompressed) //Uncompressed
		{
			if (infoHeader.BitsPerPixel == 8u && infoHeader.CLRUsed != 0u) //Color Table
			{
				decodedData.BitsPerPixel = 32u;
				decodedData.Format = TRAP::Image::ColorFormat::RGBA;
				decodedData.PixelData = TRAP::Utils::DecodeBGRAMappedPixelData(encodedPixelData, decodedData.Width, decodedData.Height, 4u, colorTable);
			}
			else if(infoHeader.BitsPerPixel == 8u && infoHeader.CLRUsed == 0u) //Grayscale
			{
				decodedData.BitsPerPixel = 8u;
				decodedData.Format = TRAP::Image::ColorFormat::GrayScale;
				decodedData.PixelData = encodedPixelData;
			}
			else if (infoHeader.BitsPerPixel == 16u) //RGB
			{
				decodedData.BitsPerPixel = 24u;
				decodedData.Format = TRAP::Image::ColorFormat::RGB;
				decodedData.PixelData = TRAP::Utils::ConvertBGR16PixelDataToRGB24(encodedPixelData, decodedData.Width, decodedData.Height);
			}
			else if (infoHeader.BitsPerPixel == 24u) //RGB
			{
				decodedData.BitsPerPixel = 24u;
				decodedData.Format = TRAP::Image::ColorFormat::RGB;
				decodedData.PixelData = TRAP::Utils::ConvertBGR24PixelDataToRGB24(encodedPixelData, decodedData.Width, decodedData.Height);
			}
			else if (infoHeader.BitsPerPixel == 32u) //RGBA
			{
				decodedData.BitsPerPixel = 32u;
				decodedData.Format = TRAP::Image::ColorFormat::RGBA;
				decodedData.PixelData = TRAP::Utils::ConvertBGRA32PixelDataToRGBA32(encodedPixelData, decodedData.Width, decodedData.Height);
			}
		}
		else if (infoHeader.Compression == BMPCompression::RLE8) //Microsoft RLE 8
		{
			if(infoHeader.CLRUsed == 0u) //Compressed Grayscale
			{
				decodedData.BitsPerPixel = 8u;
				decodedData.Format = TRAP::Image::ColorFormat::GrayScale;
				decodedData.PixelData = DecodeRLE8GrayScale(decodedData.Width, decodedData.Height, encodedPixelData); //Decode Single Channel RLE 8
			}
			else //Compressed Palette
			{
				decodedData.BitsPerPixel = 32u;
				decodedData.Format = TRAP::Image::ColorFormat::RGBA;
				decodedData.PixelData = DecodeRLE8RGBA(decodedData.Width, decodedData.Height, encodedPixelData, colorTable); //Decode Multi Channel RLE 8
			}

			if(decodedData.PixelData.empty())
				return TRAP::MakeUnexpected(BMPErrorCode::FailedRLE8Decompression);
		}
		else if (infoHeader.Compression == BMPCompression::BitFields ||
				 infoHeader.Compression == BMPCompression::AlphaBitFields) //BitFields
		{
			decodedData.Format = TRAP::Image::ColorFormat::RGBA;

			if(decodedData.BitsPerPixel == 32u)
			{
				const auto bitFields = ValidateBitFields(bitfieldMasks, decodedData.BitsPerPixel);
				if(!bitFields)
					return TRAP::Unexpected(bitFields.Error());

				decodedData.PixelData = DecodeBitFieldPixelData<u32>(encodedPixelData, *bitFields, decodedData.Width, decodedData.Height, decodedData.BitsPerPixel);
			}
			else if(decodedData.BitsPerPixel == 16u)
			{
				const auto bitFields = ValidateBitFields(bitfieldMasks, decodedData.BitsPerPixel);
				if(!bitFields)
					return TRAP::Unexpected(bitFields.Error());

				decodedData.PixelData = DecodeBitFieldPixelData<u16>(encodedPixelData, *bitFields, decodedData.Width, decodedData.Height, decodedData.BitsPerPixel);
				decodedData.BitsPerPixel = 32u;
			}
		}

		const bool needYFlip = infoHeader.Height > 0;
		if (needYFlip)
			decodedData.PixelData = TRAP::Utils::FlipPixelDataY<u8>(decodedData.Width, decodedData.Height, decodedData.BitsPerPixel / 8u, decodedData.PixelData);

		return decodedData;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::INTERNAL::BMPImage::BMPImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImageBMPPrefix, "Loading image: ", m_filepath);

	const auto decodedData = LoadDataFromFile(m_filepath).AndThen(DecodePixelData);
	if(!decodedData)
	{
		TP_ERROR(Log::ImageBMPPrefix, BMPErrorCodeToString(decodedData.Error()));
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	m_width = decodedData->Width;
	m_height = decodedData->Height;
	m_bitsPerPixel = decodedData->BitsPerPixel;
	m_colorFormat = decodedData->Format;
	m_data = decodedData->PixelData;
}
