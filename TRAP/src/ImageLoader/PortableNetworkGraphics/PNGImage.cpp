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

#include "Utils/String/String.h"
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
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

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
	std::array<uint8_t, 8> MagicNumber{};
	file.read(reinterpret_cast<char*>(MagicNumber.data()), MagicNumber.size() * sizeof(uint8_t));

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

	//File uses big-endian
	const bool needSwap = Utils::GetEndian() != Utils::Endian::Big;

	//Load Chunk
	Data data{};
	{
		NextChunk nextChunk{};
		nextChunk.MagicNumber.resize(4);
		AlreadyLoaded alreadyLoaded{};
		while (nextChunk.MagicNumber != "IEND")
		{
			file.read(reinterpret_cast<char*>(&nextChunk.Length), sizeof(uint32_t));
			file.read(nextChunk.MagicNumber.data(), NumericCast<std::streamsize>(nextChunk.MagicNumber.size()));
			if (needSwap)
				Utils::Memory::SwapBytes(nextChunk.Length);

			if (nextChunk.Length > 2147483647)
			{
				file.close();
				TP_ERROR(Log::ImagePNGPrefix, "Chunk length ", nextChunk.Length, " is invalid!");
				TP_WARN(Log::ImagePNGPrefix, "Using default image!");
				return;
			}

			if (!ProcessChunk(nextChunk, file, data, alreadyLoaded, needSwap))
			{
				file.close();
				return;
			}
		}
		if (nextChunk.Length > std::numeric_limits<int32_t>::max())
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
			TP_ERROR(Log::ImagePNGPrefix, "Bit depth: ", NumericCast<uint32_t>(data.BitDepth), " is unsupported!");
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
			TP_ERROR(Log::ImagePNGPrefix, "Bit depth: ", NumericCast<uint32_t>(data.BitDepth), " is unsupported!");
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
	std::vector<uint8_t> decompressedData{};

	if (data.InterlaceMethod == 0)
		decompressedData.resize(GetRawSizeIDAT(m_width, m_height, m_bitsPerPixel));
	else if (data.InterlaceMethod == 1)
	{
		//Adam7 Interlaced: Expected size is the sum of the 7 sub-images sizes
		std::size_t expectedSize = 0;
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

	if (!DecompressData(data.CompressedData.data(),
	                    data.CompressedData.size(), decompressedData.data(),
						decompressedData.size()))
	{
		decompressedData.clear();
		return;
	}

	std::vector<uint8_t> raw(GetRawSize(m_width, m_height, m_bitsPerPixel), 0);
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
				TP_ERROR(Log::ImagePNGPrefix, "Indexed with bit depth: ", NumericCast<uint32_t>(data.BitDepth),
					" is invalid!");
			else
				TP_ERROR(Log::ImagePNGPrefix, "Indexed with bit depth: ", NumericCast<uint32_t>(data.BitDepth),
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

inline constexpr std::array<std::string_view, 11> UnusedChunks
{
	"cHRM", "gAMA", "iCCP", "hIST", "pHYs", "sPLT", "tIME", "iTXt", "tEXt", "zTXt", "eXIf"
};
[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data,
                                                          AlreadyLoaded& alreadyLoaded, const bool needSwap)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (nextChunk.MagicNumber == "IHDR" && !alreadyLoaded.IHDR)
	{
		alreadyLoaded.IHDR = true;
		if (ProcessIHDR(file, data, needSwap))
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

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessIHDR(std::ifstream& file, Data& data, const bool needSwap)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	IHDRChunk ihdrChunk{};
	//Read in IHDR Chunk
	file.read(reinterpret_cast<char*>(&ihdrChunk.Width), sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.Height), sizeof(uint32_t));
	ihdrChunk.BitDepth = NumericCast<uint8_t>(file.get());
	ihdrChunk.ColorType = NumericCast<uint8_t>(file.get());
	ihdrChunk.CompressionMethod = NumericCast<uint8_t>(file.get());
	ihdrChunk.FilterMethod = NumericCast<uint8_t>(file.get());
	ihdrChunk.InterlaceMethod = NumericCast<uint8_t>(file.get());
	file.read(reinterpret_cast<char*>(ihdrChunk.CRC.data()), ihdrChunk.CRC.size());

	//Convert to machines endian
	if (needSwap)
	{
		Utils::Memory::SwapBytes(ihdrChunk.Width);
		Utils::Memory::SwapBytes(ihdrChunk.Height);
	}

	const std::array<uint8_t, 17> CRCData
	{
		'I', 'H', 'D', 'R',
		NumericCast<uint8_t>(ihdrChunk.Width >> 24u), NumericCast<uint8_t>(ihdrChunk.Width >> 16u),
		NumericCast<uint8_t>(ihdrChunk.Width >> 8u), NumericCast<uint8_t>(ihdrChunk.Width),
		NumericCast<uint8_t>(ihdrChunk.Height >> 24u), NumericCast<uint8_t>(ihdrChunk.Height >> 16u),
		NumericCast<uint8_t>(ihdrChunk.Height >> 8u), NumericCast<uint8_t>(ihdrChunk.Height),
		ihdrChunk.BitDepth, ihdrChunk.ColorType, ihdrChunk.CompressionMethod, ihdrChunk.FilterMethod,
		ihdrChunk.InterlaceMethod
	};

	const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//TODO Treat image as sRGB
	std::array<uint8_t, 4> CRC{};
	const uint8_t renderingIntent = NumericCast<uint8_t>(file.get());
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	const std::array<uint8_t, 5> CRCData{ 's', 'R', 'G', 'B', renderingIntent };

	const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcesstRNS(std::ifstream& file, const uint32_t length, Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//TODO Use this chunk!
	switch (data.ColorType)
	{
	case 0:
	{
		std::array<uint8_t, 4> CRC{};
		const uint8_t grayAlpha1 = NumericCast<uint8_t>(file.get());
		const uint8_t grayAlpha2 = NumericCast<uint8_t>(file.get());
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		const std::array<uint8_t, 6> CRCData{ 't', 'R', 'N', 'S', grayAlpha1, grayAlpha2 };

		const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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
		std::array<uint8_t, 4> CRC{};
		const uint8_t redAlpha1 = NumericCast<uint8_t>(file.get());
		const uint8_t redAlpha2 = NumericCast<uint8_t>(file.get());
		const uint8_t greenAlpha1 = NumericCast<uint8_t>(file.get());
		const uint8_t greenAlpha2 = NumericCast<uint8_t>(file.get());
		const uint8_t blueAlpha1 = NumericCast<uint8_t>(file.get());
		const uint8_t blueAlpha2 = NumericCast<uint8_t>(file.get());
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		const std::array<uint8_t, 10> CRCData
		{
			't', 'R', 'N', 'S',
			redAlpha1, redAlpha2,
			greenAlpha1, greenAlpha2,
			blueAlpha1, blueAlpha2
		};

		const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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
		std::vector<uint8_t> paletteAlpha(length);
		std::array<uint8_t, 4> CRC{};
		file.read(reinterpret_cast<char*>(paletteAlpha.data()),
		          NumericCast<std::streamsize>(paletteAlpha.size()));
		file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

		std::vector<uint8_t> CRCData(paletteAlpha.size() + 4);
		CRCData[0] = 't';
		CRCData[1] = 'R';
		CRCData[2] = 'N';
		CRCData[3] = 'S';
		for (uint32_t i = 0; i < paletteAlpha.size(); i++)
			CRCData[i + 4] = paletteAlpha[i];

		const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
		if(crc != CRC)
		{
			TP_ERROR(Log::ImagePNGPrefix, "tRNS CRC: ", Utils::Hash::ConvertHashToString(CRC), " is wrong!");
			TP_WARN(Log::ImagePNGPrefix, "Using default image!");
			return false;
		}

		for (uint32_t i = 0; i < paletteAlpha.size(); i++)
			data.Palette[i].Alpha = paletteAlpha[i];

		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessPLTE(std::ifstream& file, Data& data, const uint32_t length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (length % 3 != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "PLTE length ", length, " is not divisible by 3!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (data.ColorType == 0 || data.ColorType == 4)
	{
		TP_ERROR(Log::ImagePNGPrefix, "PLTE invalid usage! This chunk should not appear with color type: ",
			     NumericCast<uint32_t>(data.ColorType), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	std::vector<RGBA> paletteData(length / 3);
	std::array<uint8_t, 4> CRC{};
	uint32_t paletteIndex = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		RGBA rgba{};
		rgba.Red = NumericCast<uint8_t>(file.get());
		i++;
		rgba.Green = NumericCast<uint8_t>(file.get());
		i++;
		rgba.Blue = NumericCast<uint8_t>(file.get());

		paletteData[paletteIndex++] = rgba;
	}
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	std::vector<uint8_t> CRCData(paletteData.size() * 3 + 4);
	CRCData[0] = 'P';
	CRCData[1] = 'L';
	CRCData[2] = 'T';
	CRCData[3] = 'E';
	uint32_t j = 0;
	for (const auto& i : paletteData)
	{
		CRCData[j++ + 4] = i.Red;
		CRCData[j++ + 4] = i.Green;
		CRCData[j++ + 4] = i.Blue;
	}

	const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::ProcessIDAT(std::ifstream& file, Data& data, const uint32_t length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	std::vector<uint8_t> compressedData(length);
	std::array<uint8_t, 4> CRC{};
	file.read(reinterpret_cast<char*>(compressedData.data()), NumericCast<std::streamsize>(compressedData.size()));
	file.read(reinterpret_cast<char*>(CRC.data()), CRC.size());

	std::vector<uint8_t> CRCData(compressedData.size() + 4);
	CRCData[0] = 'I';
	CRCData[1] = 'D';
	CRCData[2] = 'A';
	CRCData[3] = 'T';
	for (uint32_t i = 0; i < compressedData.size(); i++)
		CRCData[i + 4] = compressedData[i];

	const std::array<uint8_t, 4> crc = Utils::Hash::CRC32(CRCData.data(), CRCData.size());
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
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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
		TP_ERROR(Log::ImagePNGPrefix, "Color type: TrueColor(", NumericCast<uint32_t>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 3 && (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 &&
		ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: Indexed-Color(", NumericCast<uint32_t>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 4 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: GrayScale Alpha(", NumericCast<uint32_t>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}
	if (ihdrChunk.ColorType == 6 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Color type: TrueColor Alpha(", NumericCast<uint32_t>(ihdrChunk.ColorType),
			") doesnt allow a bit depth of ", NumericCast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only Deflate/Inflate Compression Method is defined by the ISO
	if (ihdrChunk.CompressionMethod != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Compression method: ", NumericCast<uint32_t>(ihdrChunk.CompressionMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only Adaptive filtering with 5 basic filter types is defined by the ISO
	if (ihdrChunk.FilterMethod != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Filter method: ", NumericCast<uint32_t>(ihdrChunk.CompressionMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	//Only No Interlace and Adam7 Interlace are defined by the ISO
	if (ihdrChunk.InterlaceMethod != 0 && ihdrChunk.InterlaceMethod != 1)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Interlace method: ", NumericCast<uint32_t>(ihdrChunk.InterlaceMethod),
			" is unsupported!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::DecompressData(const uint8_t* const source,
                                                            const std::size_t sourceLength,
															uint8_t* const destination,
										                    const std::size_t destinationLength)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	if (sourceLength < 2)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Compressed zlib data is too small!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error, size of zlib data too small
	}
	if ((source[0] * 256 + source[1]) % 31 != 0)
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! 256 * source[0](", source[0], ") + source[1](",
			source[1], ") must be a multiple of 31(", NumericCast<uint32_t>(source[0] * 256 + source[1]), ")!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false; //Error: 256 * source[0] + source[1] must be a multiple of 31, the FCHECK value is supposed to be made this way
	}

	const uint32_t CM = source[0] & 15u;
	const uint32_t CINFO = (source[0] >> 4u) & 15u;
	//FCHECK = source[1] & 31u; //FCHECK is already tested above
	const uint32_t FDICT = (source[1] >> 5u) & 1u;
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

	if (!Utils::Decompress::Inflate(source + 2, sourceLength - 2, destination, destinationLength))
	{
		TP_ERROR(Log::ImagePNGPrefix, "Decompression failed! Inflate error!");
		TP_WARN(Log::ImagePNGPrefix, "Using default image!");
		return false;
	}

	const uint8_t* const buf = &source[sourceLength - 4];
	const std::array<uint8_t, 4> adler32 =
	{
		buf[0],
		buf[1],
		buf[2],
		buf[3]
	};
	const std::array<uint8_t, 4> checksum = Utils::Hash::Adler32(destination, destinationLength);

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

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::UnFilterScanline(uint8_t* const recon,
	                                                          const uint8_t* const scanline,
	                                                          const uint8_t* const precon,
	                                                          const std::size_t byteWidth,
	                                                          const uint8_t filterType,
	                                                          const std::size_t length)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//For PNG Filter Method 0
	//UnFilter a PNG Image Scanline by Scanline.
	//When the pixels are smaller than 1 Byte, the Filter works Byte per Byte (byteWidth = 1)
	//precon is the previous unFiltered Scanline, recon the result, scanline the current one the incoming
	//Scanlines do NOT include the FilterType byte, that one is given in the parameter filterType instead
	//recon and scanline MAY be the same memory address!
	//precon must be disjoint.
	std::size_t i = 0;
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
		std::size_t j = 0;
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
			std::size_t j = 0;

			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i] + (precon[i] >> 1u);

			//Unroll independent paths of the Paeth predictor.
			//A 6x and 8x version is also possible but that adds too much code.
			//Whether this speeds up anything at all depends on compiler and settings.
			if(byteWidth >= 4)
			{
				for(; i + 3 < length; i += 4, j += 4)
				{
					uint8_t s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					uint8_t s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];
					uint8_t s2 = scanline[i + 2], r2 = recon[j + 2], p2 = precon[i + 2];
					uint8_t s3 = scanline[i + 3], r3 = recon[j + 3], p3 = precon[i + 3];

					recon[i + 0] = NumericCast<uint8_t>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<uint8_t>(s1 + ((r1 + p1) >> 1u));
					recon[i + 2] = NumericCast<uint8_t>(s2 + ((r2 + p2) >> 1u));
					recon[i + 3] = NumericCast<uint8_t>(s3 + ((r3 + p3) >> 1u));
				}
			}
			else if(byteWidth >= 3)
			{
				for(; i + 2 < length; i += 3, j += 3)
				{
					uint8_t s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					uint8_t s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];
					uint8_t s2 = scanline[i + 2], r2 = recon[j + 2], p2 = precon[i + 2];

					recon[i + 0] = NumericCast<uint8_t>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<uint8_t>(s1 + ((r1 + p1) >> 1u));
					recon[i + 2] = NumericCast<uint8_t>(s2 + ((r2 + p2) >> 1u));
				}
			}
			else if(byteWidth >= 2)
			{
				for(; i + 1 < length; i += 2, j += 2)
				{
					uint8_t s0 = scanline[i + 0], r0 = recon[j + 0], p0 = precon[i + 0];
					uint8_t s1 = scanline[i + 1], r1 = recon[j + 1], p1 = precon[i + 1];

					recon[i + 0] = NumericCast<uint8_t>(s0 + ((r0 + p0) >> 1u));
					recon[i + 1] = NumericCast<uint8_t>(s1 + ((r1 + p1) >> 1u));
				}
			}

			for(; i != length; ++i, ++j)
				recon[i] = NumericCast<uint8_t>(scanline[i] + ((recon[j] + precon[i]) >> 1u));
		}
		else
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i];
			std::size_t j = 0;
			for (i = byteWidth; i != length; ++i, ++j)
				recon[i] = scanline[i] + (recon[j] >> 1u);
		}
		break;
	}

	case 4:
	{
		if (precon != nullptr)
		{
			std::size_t j = 0;

			for (i = 0; i != byteWidth; ++i)
				recon[i] = (scanline[i] + precon[i]);

			//Unroll independent paths of the Paeth predictor.
			//A 6x and 8x version is also possible but that adds too much code.
			//Whether this speeds up anything at all depends on compiler and settings.
			if (byteWidth >= 4)
			{
				for (; i + 3 < length; i += 4, j += 4)
				{
					const uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1];
					const uint8_t s2 = scanline[i + 2], s3 = scanline[i + 3];
					const uint8_t r0 = recon[j + 0], r1 = recon[j + 1], r2 = recon[j + 2], r3 = recon[j + 3];
					const uint8_t p0 = precon[i + 0], p1 = precon[i + 1], p2 = precon[i + 2], p3 = precon[i + 3];
					const uint8_t q0 = precon[j + 0], q1 = precon[j + 1], q2 = precon[j + 2], q3 = precon[j + 3];
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
					const uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1], s2 = scanline[i + 2];
					const uint8_t r0 = recon[j + 0], r1 = recon[j + 1], r2 = recon[j + 2];
					const uint8_t p0 = precon[i + 0], p1 = precon[i + 1], p2 = precon[i + 2];
					const uint8_t q0 = precon[j + 0], q1 = precon[j + 1], q2 = precon[j + 2];
					recon[i + 0] = s0 + PaethPredictor(r0, p0, q0);
					recon[i + 1] = s1 + PaethPredictor(r1, p1, q1);
					recon[i + 2] = s2 + PaethPredictor(r2, p2, q2);
				}
			}
			else if (byteWidth >= 2)
			{
				for (; i + 1 < length; i += 2, j += 2)
				{
					const uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1];
					const uint8_t r0 = recon[j + 0], r1 = recon[j + 1];
					const uint8_t p0 = precon[i + 0], p1 = precon[i + 1];
					const uint8_t q0 = precon[j + 0], q1 = precon[j + 1];
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
			std::size_t j = 0;
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

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::UnFilter(uint8_t* const out, const uint8_t* const in,
									                  const uint32_t width, const uint32_t height,
                                                      const uint32_t bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//For PNG Filter Method 0
	//This function unFilters a single image(e.g. without interlacing this is called once, with Adam7 seven times)
	//out must have enough bytes allocated already, in must have the scanlines + 1 filterType byte per scanline
	//width and height are image dimensions or dimensions of reduced image, bitsPerPixel is bits per pixel
	//in and out are allowed to be the same memory address
	//(but are not the same size since in has the extra filter Bytes)
	const uint8_t* prevLine = nullptr;

	//byteWidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise
	const std::size_t byteWidth = (bitsPerPixel + 7u) / 8u;
	const std::size_t lineBytes = (width * bitsPerPixel + 7u) / 8u;

	for (uint32_t y = 0; y < height; ++y)
	{
		const std::size_t outIndex = lineBytes * y;
		const std::size_t inIndex = (1 + lineBytes) * y; //The extra filterByte added to each row
		const uint8_t filterType = in[inIndex];

		if (!UnFilterScanline(&out[outIndex], &in[inIndex + 1], prevLine, byteWidth, filterType, lineBytes))
			return false;

		prevLine = &out[outIndex];
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint8_t TRAP::INTERNAL::PNGImage::PaethPredictor(uint16_t a, const uint16_t b, const uint16_t c)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	uint16_t pa = NumericCast<uint16_t>(Math::Abs(b - c));
	const uint16_t pb = NumericCast<uint16_t>(Math::Abs(a - c));
	const uint16_t pc = NumericCast<uint16_t>(Math::Abs(a + b - c - c));

	//Return input value associated with smallest of pa, pb, pc(with certain priority if equal)
	if (pb < pa)
	{
		a = b;
		pa = pb;
	}

	return NumericCast<uint8_t>((pc < pa) ? c : a);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::size_t TRAP::INTERNAL::PNGImage::GetRawSizeIDAT(const uint32_t width, const uint32_t height,
                                                                   const uint32_t bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//In an IDAT chunk, each scanline is a multiple of 8 bits and in addition has one extra byte per line: the filter byte.
	//+ 1 for the filter byte, and possibly plus padding bits per line
	const std::size_t line = (NumericCast<std::size_t>(width / 8u) * bitsPerPixel) +
	                         1u + ((width & 7u) * bitsPerPixel + 7u) / 8u;

	return NumericCast<std::size_t>(height) * line;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::size_t TRAP::INTERNAL::PNGImage::GetRawSize(const uint32_t width, const uint32_t height,
                                                               const uint32_t bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const std::size_t n = NumericCast<std::size_t>(width) * NumericCast<std::size_t>(height);
	return ((n / 8u) * bitsPerPixel) + ((n & 7u) * bitsPerPixel + 7u) / 8u;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::PNGImage::PostProcessScanlines(uint8_t* const out, uint8_t* const in, const uint32_t width,
                                                                  const uint32_t height, const uint32_t bitsPerPixel,
													              const uint8_t interlaceMethod)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

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
		std::array<uint32_t, 7> passW{}, passH{};
		std::array<std::size_t, 8> filterPassStart{}, paddedPassStart{}, passStart{};

		Adam7GetPassValues(passW, passH, filterPassStart, paddedPassStart, passStart, width, height, bitsPerPixel);

		for (uint32_t i = 0; i != 7; ++i)
		{
			if (!UnFilter(&in[paddedPassStart[i]], &in[filterPassStart[i]], passW[i], passH[i], bitsPerPixel))
				return false;
		}

		Adam7DeInterlace(out, in, width, height, bitsPerPixel);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr std::array<uint32_t, 7> ADAM7_IX = { 0, 4, 0, 2, 0, 1, 0 }; /*X start values*/
constexpr std::array<uint32_t, 7> ADAM7_IY = { 0, 0, 4, 0, 2, 0, 1 }; /*Y start values*/
constexpr std::array<uint32_t, 7> ADAM7_DX = { 8, 8, 4, 4, 2, 2, 1 }; /*X delta values*/
constexpr std::array<uint32_t, 7> ADAM7_DY = { 8, 8, 8, 4, 4, 2, 2 }; /*Y delta values*/

void TRAP::INTERNAL::PNGImage::Adam7GetPassValues(std::array<uint32_t, 7>& passW,
	                                              std::array<uint32_t, 7>& passH,
	                                              std::array<std::size_t, 8>& filterPassStart,
	                                              std::array<std::size_t, 8>& paddedPassStart,
	                                              std::array<std::size_t, 8>& passStart,
	                                              const uint32_t width,
	                                              const uint32_t height,
	                                              const uint32_t bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//"padded" is only relevant if bitsPerPixel is less than 8 and a scanline or image does not end at a full byte

	//The passStart values have 8 values: The 8th one indicates the byte after the end of the 7th(= last) pass
	uint32_t i = 0;

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
		paddedPassStart[i + 1] = paddedPassStart[i] + NumericCast<std::size_t>(passH[i]) *
		                         ((passW[i] * bitsPerPixel + 7u) / 8u);
		//Only padded at end of reduced image
		passStart[i + 1] = passStart[i] + (passH[i] * passW[i] * bitsPerPixel + 7u) / 8u;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PNGImage::Adam7DeInterlace(uint8_t* const out, const uint8_t* const in, const uint32_t width,
                                                const uint32_t height, const uint32_t bitsPerPixel)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	//out has the following size in bits: width * height * bitsPerPixel.
	//in is possibly bigger due to padding bits between reduced images.
	//out must be big enough AND must be 0 everywhere
	//if bitsPerPixel < 8 in the current implementation(because that is likely a little bit faster)
	//NOTE: Comments about padding bits are only relevant if bitsPerPixel < 8

	std::array<uint32_t, 7> passW{}, passH{};
	std::array<std::size_t, 8> filterPassStart{}, paddedPassStart{}, passStart{};

	Adam7GetPassValues(passW, passH, filterPassStart, paddedPassStart, passStart, width, height, bitsPerPixel);

	if(bitsPerPixel < 8)
		return;

	for (uint32_t i = 0; i != 7; ++i)
	{
		const std::size_t byteWidth = bitsPerPixel / 8u;
		for (uint32_t y = 0; y < passH[i]; ++y)
		{
			for (uint32_t x = 0; x < passW[i]; ++x)
			{
				const std::size_t pixelInStart = passStart[i] + (y * passW[i] + x) * byteWidth;
				const std::size_t pixelOutStart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) *
				                                   width + ADAM7_IX[i] + x * ADAM7_DX[i]) * byteWidth;
				for (uint32_t b = 0; b < byteWidth; ++b)
					out[pixelOutStart + b] = in[pixelInStart + b];
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint16_t> TRAP::INTERNAL::PNGImage::ConvertTo2Byte(std::vector<uint8_t>& raw)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	std::vector<uint16_t> result(raw.size() / 2, 0);
	uint32_t resultIndex = 0;
	if (Utils::GetEndian() == Utils::Endian::Big)
	{
		for (uint32_t i = 0; i < raw.size(); i += 2)
		{
			const uint16_t val = (NumericCast<uint16_t>(raw[i + 1u] << 8u)) | raw[i];
			result[resultIndex++] = val;
		}
	}
	else
	{
		for (uint32_t i = 0; i < raw.size(); i += 2)
		{
			const uint16_t val = (NumericCast<uint16_t>(raw[i + 1u] << 8u)) | raw[i];
			result[resultIndex++] = val;
		}

		for (uint16_t& i : result)
			Utils::Memory::SwapBytes(i);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<uint8_t> TRAP::INTERNAL::PNGImage::ResolveIndexed(std::vector<uint8_t>& raw, const uint32_t width,
                                                                            const uint32_t height, const Data& data)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	std::vector<uint8_t> result(NumericCast<std::size_t>(width) * height * 4, 0);
	uint32_t resultIndex = 0;
	for (const uint8_t& element : raw)
	{
		const RGBA rgba = data.Palette[element];
		result[resultIndex++] = rgba.Red;
		result[resultIndex++] = rgba.Green;
		result[resultIndex++] = rgba.Blue;
		result[resultIndex++] = rgba.Alpha;
	}

	return result;
}