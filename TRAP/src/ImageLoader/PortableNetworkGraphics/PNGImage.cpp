/*
LodePNG version 20230410

Copyright (c) 2005-2023 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.

Modified by Jan "GamesTrap" Schuerkamp
*/

#include "TRAPPCH.h"
#include "PNGImage.h"

#include "FileSystem/FileSystem.h"
#include "Maths/Math.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"
#include "Utils/Hash/CRC32.h"
#include "Utils/Decompress/Inflate.h"
#include "Utils/Hash/Adler32.h"
#include "Utils/Hash/ConvertHashToString.h"

TRAP::INTERNAL::PNGImage::PNGImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	TP_DEBUG(Log::ImagePNGPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImagePNGPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	//Read in MagicNumber
	std::array<u8, 8> MagicNumber{};
	file.read(reinterpret_cast<char*>(MagicNumber.data()), MagicNumber.size() * sizeof(u8));

	//Check MagicNumber
	if (std::get<0>(MagicNumber) != 0x89 || std::get<1>(MagicNumber) != 0x50 ||
	    std::get<2>(MagicNumber) != 0x4E || std::get<3>(MagicNumber) != 0x47 ||
		std::get<4>(MagicNumber) != 0x0D || std::get<5>(MagicNumber) != 0x0A ||
		std::get<6>(MagicNumber) != 0x1A || std::get<7>(MagicNumber) != 0x0A)
	{
		file.close();
		std::string mNum{};
		for (const auto& i : MagicNumber)
			mNum += std::to_string(i);
		TP_ERROR(Log::ImagePNGPrefix, "Magic number ", mNum, "is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	//Load Chunk
	Data data{};
	{
		NextChunk nextChunk{};
		nextChunk.MagicNumber.resize(4);
		AlreadyLoaded alreadyLoaded{};
		while (nextChunk.MagicNumber != "IEND")
		{
			file.read(reinterpret_cast<char*>(&nextChunk.Length), sizeof(u32));
			file.read(nextChunk.MagicNumber.data(), NumericCast<std::streamsize>(nextChunk.MagicNumber.size()));
			if constexpr (Utils::GetEndian() != Utils::Endian::Big)
				Utils::Memory::SwapBytes(nextChunk.Length);

			if (nextChunk.Length > 2147483647)
			{
				file.close();
				TP_ERROR(Log::ImagePNGPrefix, "Chunk length ", nextChunk.Length, " is invalid!");
				TP_WARN(Log::ImagePNGPrefix, "Using default image!");
				return;
			}

			if (!ProcessChunk(nextChunk, file, data, alreadyLoaded))
			{
				file.close();
				return;
			}
		}
		if (nextChunk.Length > std::numeric_limits<i32>::max())
		{
			file.close();
			TP_ERROR(Log::ImagePNGPrefix, "Chunk length ", nextChunk.Length, " is invalid!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return;
		}
	}

	file.close();

	m_width = data.Width;
	m_height = data.Height;

	if (data.CompressionMethod != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Compression method ", data.CompressionMethod, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	switch (data.ColorType)
	{
	case 0: //GrayScale
	{
		switch (data.BitDepth)
		{
		case 1:
			[[fallthrough]];
		case 2:
			[[fallthrough]];
		case 4:
			TP_ERROR(Log::ImagePNGPrefix, "Bit depth: ", NumericCast<u32>(data.BitDepth), " is unsupported!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return;

		case 8:
			[[fallthrough]];
		case 16:
			m_bitsPerPixel = data.BitDepth;
			break;

		default:
			break;
		}
		break;
	}

	case 2: //TrueColor
	{
		if (data.BitDepth == 8)
			m_bitsPerPixel = 24;
		else if (data.BitDepth == 16)
			m_bitsPerPixel = 48;
		break;
	}

	case 3: //Indexed Color
	{
		switch (data.BitDepth)
		{
		case 1:
			[[fallthrough]];
		case 2:
			[[fallthrough]];
		case 4:
			TP_ERROR(Log::ImagePNGPrefix, "Bit depth: ", NumericCast<u32>(data.BitDepth), " is unsupported!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return;

		case 8:
			m_bitsPerPixel = 8;
			break;

		default:
			break;
		}
		break;
	}

	case 4: //GrayScale Alpha
	{
		if (data.BitDepth == 8)
			m_bitsPerPixel = 16;
		else if (data.BitDepth == 16)
			m_bitsPerPixel = 32;
		break;
	}

	case 6: //TrueColor Alpha
	{
		if (data.BitDepth == 8)
			m_bitsPerPixel = 32;
		else if (data.BitDepth == 16)
			m_bitsPerPixel = 64;
		break;
	}

	default:
		break;
	}

	if (data.InterlaceMethod != 0 && data.InterlaceMethod != 1)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Interlace method ", data.InterlaceMethod, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}
	std::vector<u8> decompressedData{};

	if (data.InterlaceMethod == 0)
		decompressedData.resize(GetRawSizeIDAT(m_width, m_height, m_bitsPerPixel));
	else if (data.InterlaceMethod == 1)
	{
		//Adam7 Interlaced: Expected size is the sum of the 7 sub-images sizes
		usize expectedSize = 0;
		expectedSize += GetRawSizeIDAT((m_width + 7u) >> 3u, (m_height + 7u) >> 3u, m_bitsPerPixel);
		if (m_width > 4)
			expectedSize += GetRawSizeIDAT((m_width + 3u) >> 3u, (m_height + 7u) >> 3u, m_bitsPerPixel);
		expectedSize += GetRawSizeIDAT((m_width + 3u) >> 2u, (m_height + 3u) >> 3u, m_bitsPerPixel);
		if (m_width > 2)
			expectedSize += GetRawSizeIDAT((m_width + 1u) >> 2u, (m_height + 3u) >> 2u, m_bitsPerPixel);
		expectedSize += GetRawSizeIDAT((m_width + 1u) >> 1u, (m_height + 1u) >> 2u, m_bitsPerPixel);
		if (m_width > 1)
			expectedSize += GetRawSizeIDAT((m_width + 0u) >> 1u, (m_height + 1u) >> 1u, m_bitsPerPixel);
		expectedSize += GetRawSizeIDAT((m_width + 0u), (m_height + 0u) >> 1u, m_bitsPerPixel);

		decompressedData.resize(expectedSize);
	}

	if (!DecompressData(data.CompressedData, decompressedData))
	{
		decompressedData.clear();
		return;
	}

	std::vector<u8> raw(GetRawSize(m_width, m_height, m_bitsPerPixel), 0);
	if (!PostProcessScanlines(raw.data(), decompressedData.data(), m_width, m_height, m_bitsPerPixel,
	                          data.InterlaceMethod))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Deinterlacing failed!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return;
	}

	switch (data.ColorType)
	{
	case 0: //GrayScale
	{
		m_colorFormat = ColorFormat::GrayScale;
		if (data.BitDepth == 8)
			m_data = raw;
		else
			m_data2Byte = ConvertTo2Byte(raw);
		break;
	}

	case 2: //TrueColor
	{
		m_colorFormat = ColorFormat::RGB;
		if (data.BitDepth == 8)
			m_data = raw;
		else
			m_data2Byte = ConvertTo2Byte(raw);
		break;
	}

	case 3: //Indexed Color
	{
		m_colorFormat = ColorFormat::RGBA;
		m_bitsPerPixel = 32;
		if(data.Palette.empty())
		{
			TP_ERROR(Log::ImagePNGPrefix, "Indexed missing palette!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return;
		}

		if (data.BitDepth == 8)
			m_data = ResolveIndexed(raw, m_width, m_height, data);
		else
		{
			if (data.BitDepth > 8)
				TP_ERROR(Log::ImagePNGPrefix, "Indexed with bit depth: ", NumericCast<u32>(data.BitDepth),
					" is invalid!");
			else
				TP_ERROR(Log::ImagePNGPrefix, "Indexed with bit depth: ", NumericCast<u32>(data.BitDepth),
					" is unsupported!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return;
		}
		break;
	}

	case 4: //GrayScale Alpha
	{
		m_colorFormat = ColorFormat::GrayScaleAlpha;
		if (data.BitDepth == 8)
			m_data = raw;
		else
			m_data2Byte = ConvertTo2Byte(raw);
		break;
	}

	case 6: //TrueColor Alpha
	{
		m_colorFormat = ColorFormat::RGBA;
		if (data.BitDepth == 8)
			m_data = raw;
		else
			m_data2Byte = ConvertTo2Byte(raw);
		break;
	}

	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	constexpr std::array<std::string_view, 11> UnusedChunks
	{
		"cHRM", "gAMA", "iCCP", "hIST", "pHYs", "sPLT", "tIME", "iTXt", "tEXt", "zTXt", "eXIf"
	};
}
[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data,
                                                          AlreadyLoaded& alreadyLoaded)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	if (nextChunk.MagicNumber == "IHDR" && !alreadyLoaded.IHDR)
	{
		alreadyLoaded.IHDR = true;
		if (ProcessIHDR(file, data))
			return true;
	}

	if (alreadyLoaded.IHDR)
	{
		const bool unusedChunk = std::ranges::any_of(UnusedChunks, [&nextChunk](const std::string_view magicNum)
		{
			return magicNum == nextChunk.MagicNumber;
		});
		if(unusedChunk)
		{
			//Optional chunk (ignored)
			file.seekg(4 + nextChunk.Length, std::ios_base::cur); //4 = Magic number.
			return true;
		}

		if (nextChunk.MagicNumber == "sBIT" && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			if (ProcesssBIT(file, data))
				return true;
		}
		if (nextChunk.MagicNumber == "sRGB" && !alreadyLoaded.sRGB && !alreadyLoaded.PLTE &&
		    !alreadyLoaded.IDAT)
		{
			alreadyLoaded.sRGB = true;
			if (ProcesssRGB(file))
				return true;
		}
		if (nextChunk.MagicNumber == "bKGD" && !alreadyLoaded.IDAT)
		{
			if (ProcessbKGD(file, data))
				return true;
		}
		if (nextChunk.MagicNumber == "tRNS" && !alreadyLoaded.tRNS && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.tRNS = true;
			if (ProcesstRNS(file, nextChunk.Length, data))
				return true;
		}
		if (nextChunk.MagicNumber == "PLTE" && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			if (ProcessPLTE(file, data, nextChunk.Length))
				return true;
		}
		if (nextChunk.MagicNumber == "IDAT")
		{
			if (ProcessIDAT(file, data, nextChunk.Length))
				return true;
		}
		if (nextChunk.MagicNumber == "IEND")
			return true;
	}

	TP_ERROR(Log::ImagePNGPrefix, "Invalid or multiple usage of chunk magic number: ", nextChunk.MagicNumber, "!");
	TP_WARN(Log::ImagePNGPrefix, "Using default image!");
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessIHDR(std::ifstream& file, Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	IHDRChunk ihdrChunk{};
	//Read in IHDR Chunk
	file.read(reinterpret_cast<char*>(&ihdrChunk.Width), sizeof(u32));
	file.read(reinterpret_cast<char*>(&ihdrChunk.Height), sizeof(u32));
	ihdrChunk.BitDepth = NumericCast<u8>(file.get());
	ihdrChunk.ColorType = NumericCast<u8>(file.get());
	ihdrChunk.CompressionMethod = NumericCast<u8>(file.get());
	ihdrChunk.FilterMethod = NumericCast<u8>(file.get());
	ihdrChunk.InterlaceMethod = NumericCast<u8>(file.get());
	file.read(reinterpret_cast<char*>(ihdrChunk.CRC.data()), ihdrChunk.CRC.size());

	//Convert to machines endian
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
	{
		Utils::Memory::SwapBytes(ihdrChunk.Width);
		Utils::Memory::SwapBytes(ihdrChunk.Height);
	}

	const std::array<u8, 17> CRCData
	{
		'I', 'H', 'D', 'R',
		Utils::Memory::GetByteFromInteger<3>(ihdrChunk.Width),
		Utils::Memory::GetByteFromInteger<2>(ihdrChunk.Width),
		Utils::Memory::GetByteFromInteger<1>(ihdrChunk.Width),
		Utils::Memory::GetByteFromInteger<0>(ihdrChunk.Width),
		Utils::Memory::GetByteFromInteger<3>(ihdrChunk.Height),
		Utils::Memory::GetByteFromInteger<2>(ihdrChunk.Height),
		Utils::Memory::GetByteFromInteger<1>(ihdrChunk.Height),
		Utils::Memory::GetByteFromInteger<0>(ihdrChunk.Height),
		ihdrChunk.BitDepth, ihdrChunk.ColorType, ihdrChunk.CompressionMethod, ihdrChunk.FilterMethod,
		ihdrChunk.InterlaceMethod
	};

	const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
	if(crc != ihdrChunk.CRC)
	{
		TP_ERROR(Log::ImagePNGPrefix, "IHDR CRC: ", Utils::Hash::ConvertHashToString(ihdrChunk.CRC), " is wrong!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	if (!IHDRCheck(ihdrChunk))
		return false;

	data.Width = ihdrChunk.Width;
	data.Height = ihdrChunk.Height;
	data.BitDepth = ihdrChunk.BitDepth;
	data.ColorType = ihdrChunk.ColorType;
	data.CompressionMethod = ihdrChunk.CompressionMethod; //Always 0 others are unsupported extensions!
	data.FilterMethod = ihdrChunk.FilterMethod; //Always 0 others are unsupported extensions!
	data.InterlaceMethod = ihdrChunk.InterlaceMethod; //Always 0 or 1 others are unsupported extensions!

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcesssBIT(std::ifstream& file, const Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch (data.ColorType)
	{
	case 0:
	{
		file.seekg(4 + 1, std::ios::cur);
		return true;
	}

	case 2:
		[[fallthrough]];
	case 3:
	{
		file.seekg(4 + 3, std::ios::cur);
		return true;
	}

	case 4:
	{
		file.seekg(4 + 2, std::ios::cur);
		return true;
	}

	case 6:
	{
		file.seekg(4 + 4, std::ios::cur);
		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcesssRGB(std::ifstream& file)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	//TODO Treat image as sRGB
	std::array<u8, 4> CRC{};
	const u8 renderingIntent = static_cast<u8>(file.get());
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	const std::array<u8, 5> CRCData{ 's', 'R', 'G', 'B', renderingIntent };

	const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
	if(CRC != crc)
	{
		TP_ERROR(Log::ImagePNGPrefix, "sRGB CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessbKGD(std::ifstream& file, const Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch (data.ColorType)
	{
	case 0:
		[[fallthrough]];
	case 4:
	{
		file.seekg(4 + 2, std::ios::cur);
		return true;
	}

	case 2:
		[[fallthrough]];
	case 6:
	{
		file.seekg(4 + 6, std::ios::cur);
		return true;
	}

	case 3:
	{
		file.seekg(4 + 1, std::ios::cur);
		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcesstRNS(std::ifstream& file, const u32 length, Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	//TODO Use this chunk!
	switch (data.ColorType)
	{
	case 0:
	{
		std::array<u8, 4> CRC{};
		const u8 grayAlpha1 = static_cast<u8>(file.get());
		const u8 grayAlpha2 = static_cast<u8>(file.get());
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		const std::array<u8, 6> CRCData{ 't', 'R', 'N', 'S', grayAlpha1, grayAlpha2 };

		const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
		if(crc != CRC)
		{
			TP_ERROR(Log::ImagePNGPrefix, "tRNS CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return false;
		}

		return true;
	}

	case 2:
	{
		std::array<u8, 4> CRC{};
		const u8 redAlpha1 = static_cast<u8>(file.get());
		const u8 redAlpha2 = static_cast<u8>(file.get());
		const u8 greenAlpha1 = static_cast<u8>(file.get());
		const u8 greenAlpha2 = static_cast<u8>(file.get());
		const u8 blueAlpha1 = static_cast<u8>(file.get());
		const u8 blueAlpha2 = static_cast<u8>(file.get());
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		const std::array<u8, 10> CRCData
		{
			't', 'R', 'N', 'S',
			redAlpha1, redAlpha2,
			greenAlpha1, greenAlpha2,
			blueAlpha1, blueAlpha2
		};

		const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
		if(crc != CRC)
		{
			TP_ERROR(Log::ImagePNGPrefix, "tRNS CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return false;
		}

		return true;
	}

	case 3:
	{
		std::vector<u8> paletteAlpha(length);
		std::array<u8, 4> CRC{};
		file.read(reinterpret_cast<char*>(paletteAlpha.data()),
		          NumericCast<std::streamsize>(paletteAlpha.size()));
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		std::vector<u8> CRCData(paletteAlpha.size() + 4);
		CRCData[0] = 't';
		CRCData[1] = 'R';
		CRCData[2] = 'N';
		CRCData[3] = 'S';
		for (u32 i = 0; i < paletteAlpha.size(); i++)
			CRCData[i + 4] = paletteAlpha[i];

		const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
		if(crc != CRC)
		{
			TP_ERROR(Log::ImagePNGPrefix, "tRNS CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return false;
		}

		for (u32 i = 0; i < paletteAlpha.size(); i++)
			data.Palette[i].Alpha = paletteAlpha[i];

		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessPLTE(std::ifstream& file, Data& data, const u32 length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	if (length % 3 != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "PLTE length ", length, " is not divisible by 3!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (data.ColorType == 0 || data.ColorType == 4)
	{
		TP_ERROR(Log::ImagePNGPrefix, "PLTE invalid usage! This chunk should not appear with color type: ",
			     NumericCast<u32>(data.ColorType), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	std::vector<RGBA> paletteData(length / 3);
	std::array<u8, 4> CRC{};
	u32 paletteIndex = 0;
	for (u32 i = 0; i < length; i++)
	{
		RGBA rgba{};
		rgba.Red = NumericCast<u8>(file.get());
		i++;
		rgba.Green = NumericCast<u8>(file.get());
		i++;
		rgba.Blue = NumericCast<u8>(file.get());

		paletteData[paletteIndex++] = rgba;
	}
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	std::vector<u8> CRCData(paletteData.size() * 3 + 4);
	CRCData[0] = 'P';
	CRCData[1] = 'L';
	CRCData[2] = 'T';
	CRCData[3] = 'E';
	u32 j = 0;
	for (const auto& i : paletteData)
	{
		CRCData[j++ + 4] = i.Red;
		CRCData[j++ + 4] = i.Green;
		CRCData[j++ + 4] = i.Blue;
	}

	const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
	if(crc != CRC)
	{
		TP_ERROR(Log::ImagePNGPrefix, "PLTE CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	data.Palette = paletteData;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessIDAT(std::ifstream& file, Data& data, const u32 length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	std::vector<u8> compressedData(length);
	std::array<u8, 4> CRC{};
	file.read(reinterpret_cast<char*>(compressedData.data()), NumericCast<std::streamsize>(compressedData.size()));
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	std::vector<u8> CRCData(compressedData.size() + 4);
	CRCData[0] = 'I';
	CRCData[1] = 'D';
	CRCData[2] = 'A';
	CRCData[3] = 'T';
	for (u32 i = 0; i < compressedData.size(); i++)
		CRCData[i + 4] = compressedData[i];

	const std::array<u8, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
	if(crc != CRC)
	{
		TP_ERROR(Log::ImagePNGPrefix, "IDAT CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	data.CompressedData.insert(data.CompressedData.end(), compressedData.begin(), compressedData.end());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::IHDRCheck(const IHDRChunk& ihdrChunk)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	//Check if Width is (in)valid
	if(ihdrChunk.Width == 0u)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Width ", ihdrChunk.Width, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Check if Height is (in)valid
	if (ihdrChunk.Height == 0u)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Height ", ihdrChunk.Height, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Check if Bit Depth is a valid number
	if (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 && ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8 &&
	    ihdrChunk.BitDepth != 16)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Bit depth ", ihdrChunk.BitDepth, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Check if Color Type is a valid number
	if (ihdrChunk.ColorType != 0 && ihdrChunk.ColorType != 2 && ihdrChunk.ColorType != 3 &&
	    ihdrChunk.ColorType != 4 && ihdrChunk.ColorType != 6)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type ", ihdrChunk.ColorType, " is invalid!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Check if Color Type matches Bit Depth
	if (ihdrChunk.ColorType == 2 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: TrueColor(", NumericCast<u32>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<u32>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 3 && (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 &&
		ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: Indexed-Color(", NumericCast<u32>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<u32>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 4 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: GrayScale Alpha(", NumericCast<u32>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<u32>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 6 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: TrueColor Alpha(", NumericCast<u32>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<u32>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only Deflate/Inflate Compression Method is defined by the ISO
	if (ihdrChunk.CompressionMethod != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Compression method: ", NumericCast<u32>(ihdrChunk.CompressionMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only Adaptive filtering with 5 basic filter types is defined by the ISO
	if (ihdrChunk.FilterMethod != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Filter method: ", NumericCast<u32>(ihdrChunk.CompressionMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only No Interlace and Adam7 Interlace are defined by the ISO
	if (ihdrChunk.InterlaceMethod != 0 && ihdrChunk.InterlaceMethod != 1)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Interlace method: ", NumericCast<u32>(ihdrChunk.InterlaceMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::DecompressData(const std::span<const u8> source,
															const std::span<u8> destination)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	if (source.size_bytes() < 2)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Compressed zlib data is too small!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error, size of zlib data too small
	}
	if ((source[0] * 256 + source[1]) % 31 != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! 256 * source[0](", source[0], ") + source[1](",
			source[1], ") must be a multiple of 31(", NumericCast<u32>(source[0] * 256 + source[1]), ")!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error: 256 * source[0] + source[1] must be a multiple of 31, the FCHECK value is supposed to be made this way
	}

	const u32 CM = source[0] & 15u;
	const u32 CINFO = (source[0] >> 4u) & 15u;
	//FCHECK = source[1] & 31u; //FCHECK is already tested above
	const u32 FDICT = (source[1] >> 5u) & 1u;
	//FLEVEL = (source[1] >> 6u) & 3u; //FLEVEL is not used here

	if (CM != 8 || CINFO > 7)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! Only compression method 8(De/Inflate) with ",
		                              "sliding window of 32K is supported by the PNG specification!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error: Only compression method 8: inflate with sliding window of 32K is supported by the PNG specification
	}
	if(FDICT != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! Additional flags should ",
			                          "not specify a preset dictionary!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error: The PNG specification says that the zlib stream should not specify a preset dictionary in the additional flags!
	}

	if (!Utils::Decompress::Inflate(source.subspan<2>(), destination))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! Inflate error!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	const u8* const buf = &source[source.size_bytes() - 4];
	const std::array<u8, 4> adler32 =
	{
		buf[0],
		buf[1],
		buf[2],
		buf[3]
	};
	const std::array<u8, 4> checksum = Utils::Hash::Adler32(destination.data(), destination.size_bytes());

	if(checksum != adler32)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! Adler32 checksum: ",
		                              Utils::Hash::ConvertHashToString(adler32),
									  " doesnt match checksum: ", Utils::Hash::ConvertHashToString(checksum), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error: Adler checksum not correct, data must be corrupt
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::UnFilterScanline(u8* const recon,
	                                                          const u8* const scanline,
	                                                          const u8* const precon,
	                                                          const usize byteWidth,
	                                                          const u8 filterType,
	                                                          const usize length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//For PNG Filter Method 0
	//UnFilter a PNG Image Scanline by Scanline.
	//When the pixels are smaller than 1 Byte, the Filter works Byte per Byte (byteWidth = 1)
	//precon is the previous unFiltered Scanline, recon the result, scanline the current one the incoming
	//Scanlines do NOT include the FilterType byte, that one is given in the parameter filterType instead
	//recon and scanline MAY be the same memory address!
	//precon must be disjoint.
	usize i = 0;
	switch (filterType)
	{
	case 0:
	{
		for (i = 0; i != length; ++i)
			recon[i] = scanline[i];
		break;
	}

	case 1:
	{
		usize j = 0;
		for (i = 0; i != byteWidth; ++i)
			recon[i] = scanline[i];
		for (i = byteWidth; i != length; ++i, ++j)
			recon[i] = scanline[i] + recon[j];
		break;
	}

	case 2:
	{
		if (precon != nullptr)
			for (i = 0; i != length; ++i)
				recon[i] = scanline[i] + precon[i];
		else
			for (i = 0; i != length; ++i)
				recon[i] = scanline[i];
		break;
	}

	case 3:
	{
		if (precon != nullptr)
		{
			usize j = 0;

			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i] + (precon[i] >> 1u);

			//Unroll independent paths of the Paeth predictor.
			//A 6x and 8x version is also possible but that adds too much code.
			//Whether this speeds up anything at all depends on compiler and settings.
			if(byteWidth >= 4)
			{
				for(; i + 3 < length; i += 4, j += 4)
				{
					u8 s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					u8 s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];
					u8 s2 = scanline[i + 2], r2 = recon[j + 2], p2 = precon[i + 2];
					u8 s3 = scanline[i + 3], r3 = recon[j + 3], p3 = precon[i + 3];

					recon[i + 0] = NumericCast<u8>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<u8>(s1 + ((r1 + p1) >> 1u));
					recon[i + 2] = NumericCast<u8>(s2 + ((r2 + p2) >> 1u));
					recon[i + 3] = NumericCast<u8>(s3 + ((r3 + p3) >> 1u));
				}
			}
			else if(byteWidth >= 3)
			{
				for(; i + 2 < length; i += 3, j += 3)
				{
					u8 s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					u8 s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];
					u8 s2 = scanline[i + 2], r2 = recon[j + 2], p2 = precon[i + 2];

					recon[i + 0] = NumericCast<u8>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<u8>(s1 + ((r1 + p1) >> 1u));
					recon[i + 2] = NumericCast<u8>(s2 + ((r2 + p2) >> 1u));
				}
			}
			else if(byteWidth >= 2)
			{
				for(; i + 1 < length; i += 2, j += 2)
				{
					u8 s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					u8 s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];

					recon[i + 0] = NumericCast<u8>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<u8>(s1 + ((r1 + p1) >> 1u));
				}
			}

			for(; i != length; ++i, ++j)
				recon[i] = NumericCast<u8>(scanline[i] + ((recon[j] + precon[i]) >> 1u));
		}
		else
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i];
			usize j = 0;
			for (i = byteWidth; i != length; ++i, ++j)
				recon[i] = scanline[i] + (recon[j] >> 1u);
		}
		break;
	}

	case 4:
	{
		if (precon != nullptr)
		{
			usize j = 0;

			for (i = 0; i != byteWidth; ++i)
				recon[i] = (scanline[i] + precon[i]);

			//Unroll independent paths of the Paeth predictor.
			//A 6x and 8x version is also possible but that adds too much code.
			//Whether this speeds up anything at all depends on compiler and settings.
			if (byteWidth >= 4)
			{
				for (; i + 3 < length; i += 4, j += 4)
				{
					const u8 s0 = scanline[i + 0], s1 = scanline[i + 1];
					const u8 s2 = scanline[i + 2], s3 = scanline[i + 3];
					const u8 r0 = recon[j + 0], r1 = recon[j + 1], r2 = recon[j + 2], r3 = recon[j + 3];
					const u8 p0 = precon[i + 0], p1 = precon[i + 1], p2 = precon[i + 2], p3 = precon[i + 3];
					const u8 q0 = precon[j + 0], q1 = precon[j + 1], q2 = precon[j + 2], q3 = precon[j + 3];
					recon[i + 0] = s0 + PaethPredictor(r0, p0, q0);
					recon[i + 1] = s1 + PaethPredictor(r1, p1, q1);
					recon[i + 2] = s2 + PaethPredictor(r2, p2, q2);
					recon[i + 3] = s3 + PaethPredictor(r3, p3, q3);
				}
			}
			else if (byteWidth >= 3)
			{
				for (; i + 2 < length; i += 3, j += 3)
				{
					const u8 s0 = scanline[i + 0], s1 = scanline[i + 1], s2 = scanline[i + 2];
					const u8 r0 = recon[j + 0], r1 = recon[j + 1], r2 = recon[j + 2];
					const u8 p0 = precon[i + 0], p1 = precon[i + 1], p2 = precon[i + 2];
					const u8 q0 = precon[j + 0], q1 = precon[j + 1], q2 = precon[j + 2];
					recon[i + 0] = s0 + PaethPredictor(r0, p0, q0);
					recon[i + 1] = s1 + PaethPredictor(r1, p1, q1);
					recon[i + 2] = s2 + PaethPredictor(r2, p2, q2);
				}
			}
			else if (byteWidth >= 2)
			{
				for (; i + 1 < length; i += 2, j += 2)
				{
					const u8 s0 = scanline[i + 0], s1 = scanline[i + 1];
					const u8 r0 = recon[j + 0], r1 = recon[j + 1];
					const u8 p0 = precon[i + 0], p1 = precon[i + 1];
					const u8 q0 = precon[j + 0], q1 = precon[j + 1];
					recon[i + 0] = s0 + PaethPredictor(r0, p0, q0);
					recon[i + 1] = s1 + PaethPredictor(r1, p1, q1);
				}
			}

			for (; i != length; ++i, ++j)
				recon[i] = (scanline[i] + PaethPredictor(recon[i - byteWidth], precon[i], precon[j]));
		}
		else
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i];
			usize j = 0;
			for (i = byteWidth; i != length; ++i, ++j)
				recon[i] = (scanline[i] + recon[j]);
		}
		break;
	}

	default:
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::UnFilter(u8* const out, const u8* const in,
									                  const u32 width, const u32 height,
                                                      const u32 bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//For PNG Filter Method 0
	//This function unFilters a single image(e.g. without interlacing this is called once, with Adam7 seven times)
	//out must have enough bytes allocated already, in must have the scanlines + 1 filterType byte per scanline
	//width and height are image dimensions or dimensions of reduced image, bitsPerPixel is bits per pixel
	//in and out are allowed to be the same memory address
	//(but are not the same size since in has the extra filter Bytes)
	const u8* prevLine = nullptr;

	//byteWidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise
	const usize byteWidth = (bitsPerPixel + 7u) / 8u;
	const usize lineBytes = (width * bitsPerPixel + 7u) / 8u;

	for (u32 y = 0; y < height; ++y)
	{
		const usize outIndex = lineBytes * y;
		const usize inIndex = (1 + lineBytes) * y; //The extra filterByte added to each row
		const u8 filterType = in[inIndex];

		if (!UnFilterScanline(&out[outIndex], &in[inIndex + 1], prevLine, byteWidth, filterType, lineBytes))
			return false;

		prevLine = &out[outIndex];
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u8 TRAP::INTERNAL::PNGImage::PaethPredictor(u16 a, const u16 b, const u16 c)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	u16 pa = NumericCast<u16>(Math::Abs(b - c));
	const u16 pb = NumericCast<u16>(Math::Abs(a - c));
	const u16 pc = NumericCast<u16>(Math::Abs(a + b - c - c));

	//Return input value associated with smallest of pa, pb, pc(with certain priority if equal)
	if (pb < pa)
	{
		a = b;
		pa = pb;
	}

	return NumericCast<u8>((pc < pa) ? c : a);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] usize TRAP::INTERNAL::PNGImage::GetRawSizeIDAT(const u32 width, const u32 height,
                                                                   const u32 bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	//In an IDAT chunk, each scanline is a multiple of 8 bits and in addition has one extra byte per line: the filter byte.
	//+ 1 for the filter byte, and possibly plus padding bits per line
	const usize line = (NumericCast<usize>(width / 8u) * bitsPerPixel) +
	                         1u + ((width & 7u) * bitsPerPixel + 7u) / 8u;

	return NumericCast<usize>(height) * line;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] usize TRAP::INTERNAL::PNGImage::GetRawSize(const u32 width, const u32 height,
                                                               const u32 bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None &&
	                                         (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	const usize n = NumericCast<usize>(width) * NumericCast<usize>(height);
	return ((n / 8u) * bitsPerPixel) + ((n & 7u) * bitsPerPixel + 7u) / 8u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::PostProcessScanlines(u8* const out, u8* const in, const u32 width,
                                                                  const u32 height, const u32 bitsPerPixel,
													              const u8 interlaceMethod)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//out must be a buffer big enough to contain full image, and in must contain the full decompressed
	//data from the IDAT chunks(with filter bytes and possible padding bits)
	//This function converts filtered-padded-interlaced data into pure 2D image buffer with the PNGs colorType.
	//Steps:
	//*) If no Adam7: 1) UnFilter 2) Remove padding bits(= possible extra bits per scanline if bitsPerPixel < 8)
	//*) If Adam7: 1) 7x UnFilter 2) 7x Remove padding bits 3) Adam7DeInterlace
	//NOTE: The in buffer will be overwritten with intermediate data!
	if (bitsPerPixel == 0)
		return false;

	if (interlaceMethod == 0)
	{
		if (!UnFilter(out, in, width, height, bitsPerPixel))
			return false;
	}
	else
	{
		//Adam7
		std::array<u32, 7> passW{}, passH{};
		std::array<usize, 8> filterPassStart{}, paddedPassStart{}, passStart{};

		Adam7GetPassValues(passW, passH, filterPassStart, paddedPassStart, passStart, width, height, bitsPerPixel);

		for (u32 i = 0; i != 7; ++i)
		{
			if (!UnFilter(&in[paddedPassStart[i]], &in[filterPassStart[i]], passW[i], passH[i], bitsPerPixel))
				return false;
		}

		Adam7DeInterlace(out, in, width, height, bitsPerPixel);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	constexpr std::array<u32, 7> ADAM7_IX = { 0, 4, 0, 2, 0, 1, 0 }; /*X start values*/
	constexpr std::array<u32, 7> ADAM7_IY = { 0, 0, 4, 0, 2, 0, 1 }; /*Y start values*/
	constexpr std::array<u32, 7> ADAM7_DX = { 8, 8, 4, 4, 2, 2, 1 }; /*X delta values*/
	constexpr std::array<u32, 7> ADAM7_DY = { 8, 8, 8, 4, 4, 2, 2 }; /*Y delta values*/
}

void TRAP::INTERNAL::PNGImage::Adam7GetPassValues(std::array<u32, 7>& passW,
	                                              std::array<u32, 7>& passH,
	                                              std::array<usize, 8>& filterPassStart,
	                                              std::array<usize, 8>& paddedPassStart,
	                                              std::array<usize, 8>& passStart,
	                                              const u32 width,
	                                              const u32 height,
	                                              const u32 bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//"padded" is only relevant if bitsPerPixel is less than 8 and a scanline or image does not end at a full byte

	//The passStart values have 8 values: The 8th one indicates the byte after the end of the 7th(= last) pass
	u32 i = 0;

	//Calculate width and height in pixels of each pass
	for (i = 0; i != 7; ++i)
	{
		passW[i] = (width + ADAM7_DX[i] - ADAM7_IX[i] - 1) / ADAM7_DX[i];
		passH[i] = (height + ADAM7_DY[i] - ADAM7_IY[i] - 1) / ADAM7_DY[i];
		if (passW[i] == 0)
			passH[i] = 0;
		if (passH[i] == 0)
			passW[i] = 0;
	}

	std::get<0>(filterPassStart) = std::get<0>(paddedPassStart) = std::get<0>(passStart) = 0;
	for (i = 0; i != 7; ++i)
	{
		//If passW[i] is 0, its 0 bytes, not 1(no filterType-byte)
		filterPassStart[i + 1] = filterPassStart[i] + (((passW[i] != 0u) && (passH[i] != 0u)) ?
			passH[i] * (1u + (passW[i] * bitsPerPixel + 7u) / 8u) : 0);
		//Bits padded if needed to fill full byte at the end of each scanline
		paddedPassStart[i + 1] = paddedPassStart[i] + NumericCast<usize>(passH[i]) *
		                         ((passW[i] * bitsPerPixel + 7u) / 8u);
		//Only padded at end of reduced image
		passStart[i + 1] = passStart[i] + (passH[i] * passW[i] * bitsPerPixel + 7u) / 8u;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PNGImage::Adam7DeInterlace(u8* const out, const u8* const in, const u32 width,
                                                const u32 height, const u32 bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	//out has the following size in bits: width * height * bitsPerPixel.
	//in is possibly bigger due to padding bits between reduced images.
	//out must be big enough AND must be 0 everywhere
	//if bitsPerPixel < 8 in the current implementation(because that is likely a little bit faster)
	//NOTE: Comments about padding bits are only relevant if bitsPerPixel < 8

	std::array<u32, 7> passW{}, passH{};
	std::array<usize, 8> filterPassStart{}, paddedPassStart{}, passStart{};

	Adam7GetPassValues(passW, passH, filterPassStart, paddedPassStart, passStart, width, height, bitsPerPixel);

	if(bitsPerPixel < 8)
		return;

	for (u32 i = 0; i != 7; ++i)
	{
		const usize byteWidth = bitsPerPixel / 8u;
		for (u32 y = 0; y < passH[i]; ++y)
		{
			for (u32 x = 0; x < passW[i]; ++x)
			{
				const usize pixelInStart = passStart[i] + (y * passW[i] + x) * byteWidth;
				const usize pixelOutStart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) *
				                                   width + ADAM7_IX[i] + x * ADAM7_DX[i]) * byteWidth;
				for (u32 b = 0; b < byteWidth; ++b)
					out[pixelOutStart + b] = in[pixelInStart + b];
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<u16> TRAP::INTERNAL::PNGImage::ConvertTo2Byte(std::vector<u8>& raw)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	std::vector<u16> result(raw.size() / 2, 0);
	u32 resultIndex = 0;
	if constexpr (Utils::GetEndian() != Utils::Endian::Big)
	{
		for (u32 i = 0; i < raw.size(); i += 2)
		{
			const u16 val = (NumericCast<u16>(raw[i + 1u] << 8u)) | raw[i];
			result[resultIndex++] = val;
		}
	}
	else
	{
		for (u32 i = 0; i < raw.size(); i += 2)
		{
			const u16 val = (NumericCast<u16>(raw[i + 1u] << 8u)) | raw[i];
			result[resultIndex++] = val;
		}

		for (u16& i : result)
			Utils::Memory::SwapBytes(i);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<u8> TRAP::INTERNAL::PNGImage::ResolveIndexed(std::vector<u8>& raw, const u32 width,
                                                                            const u32 height, const Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (GetTRAPProfileSystems() & ProfileSystems::ImageLoader) != ProfileSystems::None);

	std::vector<u8> result(NumericCast<usize>(width) * height * 4, 0);
	u32 resultIndex = 0;
	for (const u8& element : raw)
	{
		const RGBA rgba = data.Palette[element];
		result[resultIndex++] = rgba.Red;
		result[resultIndex++] = rgba.Green;
		result[resultIndex++] = rgba.Blue;
		result[resultIndex++] = rgba.Alpha;
	}

	return result;
}