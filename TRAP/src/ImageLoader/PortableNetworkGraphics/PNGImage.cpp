#include "TRAPPCH.h"
#include "PNGImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Application.h"
#include "Utils/ByteSwap.h"
#include "Utils/Hash/CRC32.h"
#include "Utils/Decompress/Inflate.h"
#include "Utils/Hash/Adler32.h"

TRAP::INTERNAL::PNGImage::PNGImage(std::string filepath)
	: m_filepath(std::move(filepath)),
	m_bitsPerPixel(0),
	m_width(0),
	m_height(0),
	m_format(ImageFormat::NONE),
	m_isImageColored(false),
	m_isImageGrayScale(false),
	m_hasAlphaChannel(false)
{
	TP_DEBUG("[Image][PNG] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][PNG] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][PNG] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}

		//Read in MagicNumber
		std::array<uint8_t, 8> MagicNumber{};
		file.read(reinterpret_cast<char*>(MagicNumber.data()), MagicNumber.size() * sizeof(uint8_t));

		//Check MagicNumber
		if (MagicNumber[0] != 0x89 || MagicNumber[1] != 0x50 || MagicNumber[2] != 0x4E || MagicNumber[3] != 0x47 ||
			MagicNumber[4] != 0x0D || MagicNumber[5] != 0x0A || MagicNumber[6] != 0x1A || MagicNumber[7] != 0x0A)
		{
			file.close();
			TP_ERROR("[Image][PNG] Magic number is invalid!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}

		//File uses big-endian
		bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Big);

		//Load Chunk
		Data data{};
		{
			NextChunk nextChunk{};
			AlreadyLoaded alreadyLoaded{};
			while (!(nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'E' && nextChunk.MagicNumber[2] == 'N' && nextChunk.MagicNumber[3] == 'D'))
			{
				file.read(reinterpret_cast<char*>(&nextChunk.Length), sizeof(uint32_t));
				file.read(reinterpret_cast<char*>(nextChunk.MagicNumber.data()), nextChunk.MagicNumber.size());
				if (needSwap)
					Utils::Memory::SwapBytes(nextChunk.Length);

				if (nextChunk.Length > 2147483647)
				{
					file.close();
					TP_ERROR("[Image][PNG] Chunk Length is invalid!");
					TP_WARN("[Image][PNG] Using Default Image!");
					return;
				}

				if (!ProcessChunk(nextChunk, file, data, alreadyLoaded, needSwap))
				{
					file.close();
					return;
				}
			}
			if (nextChunk.Length > 2147483647)
			{
				file.close();
				TP_ERROR("[Image][PNG] Chunk Length is invalid!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;
			}
		}

		m_width = data.Width;
		m_height = data.Height;

		if (data.CompressionMethod != 0)
		{
			TP_ERROR("[Image][PNG] Compression Method is invalid!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}

		switch (data.ColorType)
		{
		case 0: //GrayScale
			switch (data.BitDepth)
			{
			case 1:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 2:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 4:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 8:
				m_bitsPerPixel = 8;
				break;

			case 16:
				m_bitsPerPixel = 16;
				break;

			default:
				break;
			}
			break;

		case 2: //TrueColor
			if (data.BitDepth == 8)
				m_bitsPerPixel = 24;
			else if (data.BitDepth == 16)
				m_bitsPerPixel = 48;
			break;

		case 3: //Indexed Color
			switch (data.BitDepth)
			{
			case 1:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 2:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 4:
				TP_ERROR("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;

			case 8:
				m_bitsPerPixel = 8;
				break;

			default:
				break;
			}
			break;

		case 4: //GrayScale Alpha
			if (data.BitDepth == 8)
				m_bitsPerPixel = 16;
			else if (data.BitDepth == 16)
				m_bitsPerPixel = 32;
			break;

		case 6: //TrueColor Alpha
			if (data.BitDepth == 8)
				m_bitsPerPixel = 32;
			else if (data.BitDepth == 16)
				m_bitsPerPixel = 64;
			break;

		default:
			break;
		}

		if (data.InterlaceMethod != 0 && data.InterlaceMethod != 1)
		{
			TP_ERROR("[Image][PNG] Interlace method is invalid!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}
		std::vector<uint8_t> decompressedData{};

		if (data.InterlaceMethod == 0)
			decompressedData.resize(GetRawSizeIDAT(m_width, m_height, m_bitsPerPixel));
		else if (data.InterlaceMethod == 1)
		{
			//Adam7 Interlaced: Expected size is the sum of the 7 sub-images sizes
			std::size_t expectedSize = 0;
			expectedSize += GetRawSizeIDAT((m_width + 7) >> 3, (m_height + 7) >> 3, m_bitsPerPixel);
			if (m_width > 4)
				expectedSize += GetRawSizeIDAT((m_width + 3) >> 3, (m_height + 7) >> 3, m_bitsPerPixel);
			expectedSize += GetRawSizeIDAT((m_width + 3) >> 2, (m_height + 3) >> 3, m_bitsPerPixel);
			if (m_width > 2)
				expectedSize += GetRawSizeIDAT((m_width + 1) >> 2, (m_height + 3) >> 2, m_bitsPerPixel);
			expectedSize += GetRawSizeIDAT((m_width + 1) >> 1, (m_height + 1) >> 2, m_bitsPerPixel);
			if (m_width > 1)
				expectedSize += GetRawSizeIDAT((m_width + 0) >> 1, (m_height + 1) >> 1, m_bitsPerPixel);
			expectedSize += GetRawSizeIDAT((m_width + 0), (m_height + 0) >> 1, m_bitsPerPixel);

			decompressedData.resize(expectedSize);
		}

		if (!DecompressData(data.CompressedData.data(), static_cast<uint32_t>(data.CompressedData.size()), decompressedData.data(), static_cast<uint32_t>(decompressedData.size()), needSwap))
		{
			decompressedData.clear();
			return;
		}

		std::vector<uint8_t> raw(GetRawSize(m_width, m_height, m_bitsPerPixel), 0);
		if (!PostProcessScanlines(raw.data(), decompressedData.data(), m_width, m_height, m_bitsPerPixel, data.InterlaceMethod))
		{
			TP_ERROR("[Image][PNG] DeInterlacing Failed!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}

		switch (data.ColorType)
		{
		case 0: //GrayScale
			m_format = ImageFormat::Gray_Scale;
			m_isImageGrayScale = true;
			if (data.BitDepth == 8)
				m_data = raw;
			else
				m_data2Byte = ConvertTo2Byte(raw);
			break;

		case 2: //TrueColor
			m_format = ImageFormat::RGB;
			m_isImageColored = true;
			if (data.BitDepth == 8)
				m_data = raw;
			else
				m_data2Byte = ConvertTo2Byte(raw);
			break;

		case 3: //Indexed Color
			m_format = ImageFormat::RGBA;
			m_isImageColored = true;
			m_hasAlphaChannel = true;
			m_bitsPerPixel = 32;
			if (data.BitDepth == 8)
				m_data = ResolveIndexed(raw, m_width, m_height, data);
			else
			{
				if (data.BitDepth > 8)
					TP_ERROR("[Image][PNG] Indexed with BitDepth: ", static_cast<uint32_t>(data.BitDepth), " is invalid!");
				else
					TP_ERROR("[Image][PNG] Indexed with BitDepth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
				return;
			}
			break;

		case 4: //GrayScale Alpha
			m_format = ImageFormat::Gray_Scale_Alpha;
			m_isImageGrayScale = true;
			m_hasAlphaChannel = true;
			if (data.BitDepth == 8)
				m_data = raw;
			else
			{
				//m_data2Byte = ConvertTo2Byte(raw);
				TP_ERROR("[Image][PNG] GrayScaleAlpha with BitDepth: ", static_cast<uint32_t>(data.BitDepth), " is unsupported!");
				TP_WARN("[Image][PNG] Using Default Image!");
			}
			break;

		case 6: //TrueColor Alpha
			m_format = ImageFormat::RGBA;
			m_isImageColored = true;
			m_hasAlphaChannel = true;
			if (data.BitDepth == 8)
				m_data = raw;
			else
				m_data2Byte = ConvertTo2Byte(raw);
			break;

		default:
			break;
		}

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::PNGImage::GetPixelData()
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNGImage::GetPixelDataSize() const
{
	if (!m_data2Byte.empty())
		return static_cast<uint32_t>(m_data2Byte.size());

	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNGImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNGImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNGImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::PNGImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::PNGImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::PNGImage::GetFormat() const
{
	return m_format;

}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data, AlreadyLoaded& alreadyLoaded, const bool needSwap)
{
	if (nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'H' && nextChunk.MagicNumber[2] == 'D' && nextChunk.MagicNumber[3] == 'R' && !alreadyLoaded.IHDR)
	{
		alreadyLoaded.IHDR = true;
		if (ProcessIHDR(file, data, needSwap))
			return true;
	}
	if (alreadyLoaded.IHDR)
	{
		if (nextChunk.MagicNumber[0] == 'c' && nextChunk.MagicNumber[1] == 'H' && nextChunk.MagicNumber[2] == 'R' && nextChunk.MagicNumber[3] == 'M' && !alreadyLoaded.cHRM && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.cHRM = true;
			if (ProcesscHRM(file, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'g' && nextChunk.MagicNumber[1] == 'A' && nextChunk.MagicNumber[2] == 'M' && nextChunk.MagicNumber[3] == 'A' && !alreadyLoaded.gAMA && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.gAMA = true;
			if (ProcessgAMA(file, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'i' && nextChunk.MagicNumber[1] == 'C' && nextChunk.MagicNumber[2] == 'C' && nextChunk.MagicNumber[3] == 'P' && !alreadyLoaded.iCCP && !alreadyLoaded.sRGB && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.iCCP = true;
			if (ProcessiCCP(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 's' && nextChunk.MagicNumber[1] == 'B' && nextChunk.MagicNumber[2] == 'I' && nextChunk.MagicNumber[3] == 'T' && !alreadyLoaded.sBIT && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.sBIT = true;
			if (ProcesssBIT(file, data, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 's' && nextChunk.MagicNumber[1] == 'R' && nextChunk.MagicNumber[2] == 'G' && nextChunk.MagicNumber[3] == 'B' && !alreadyLoaded.sRGB && !alreadyLoaded.iCCP && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.sRGB = true;
			if (ProcesssRGB(file, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'b' && nextChunk.MagicNumber[1] == 'K' && nextChunk.MagicNumber[2] == 'G' && nextChunk.MagicNumber[3] == 'D' && !alreadyLoaded.bKGD && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.bKGD = true;
			if (ProcessbKGD(file, data, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'h' && nextChunk.MagicNumber[1] == 'I' && nextChunk.MagicNumber[2] == 'S' && nextChunk.MagicNumber[3] == 'T' && !alreadyLoaded.hIST && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.hIST = true;
			if (ProcesshIST(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 't' && nextChunk.MagicNumber[1] == 'R' && nextChunk.MagicNumber[2] == 'N' && nextChunk.MagicNumber[3] == 'S' && !alreadyLoaded.tRNS && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.tRNS = true;
			if (ProcesstRNS(file, nextChunk.Length, data, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'p' && nextChunk.MagicNumber[1] == 'H' && nextChunk.MagicNumber[2] == 'Y' && nextChunk.MagicNumber[3] == 's' && !alreadyLoaded.pHYs && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.pHYs = true;
			if (ProcesspHYs(file, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 's' && nextChunk.MagicNumber[1] == 'P' && nextChunk.MagicNumber[2] == 'L' && nextChunk.MagicNumber[3] == 'T' && !alreadyLoaded.IDAT)
		{
			if (ProcesssPLT(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 't' && nextChunk.MagicNumber[1] == 'I' && nextChunk.MagicNumber[2] == 'M' && nextChunk.MagicNumber[3] == 'E' && !alreadyLoaded.tIME)
		{
			if (ProcesstIME(file, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'i' && nextChunk.MagicNumber[1] == 'T' && nextChunk.MagicNumber[2] == 'X' && nextChunk.MagicNumber[3] == 't')
		{
			if (ProcessiTXt(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 't' && nextChunk.MagicNumber[1] == 'E' && nextChunk.MagicNumber[2] == 'X' && nextChunk.MagicNumber[3] == 't')
		{
			if (ProcesstEXt(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'z' && nextChunk.MagicNumber[1] == 'T' && nextChunk.MagicNumber[2] == 'X' && nextChunk.MagicNumber[3] == 't')
		{
			if (ProcesszTXt(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'P' && nextChunk.MagicNumber[1] == 'L' && nextChunk.MagicNumber[2] == 'T' && nextChunk.MagicNumber[3] == 'E' && !alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			if (ProcessPLTE(file, data, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'D' && nextChunk.MagicNumber[2] == 'A' && nextChunk.MagicNumber[3] == 'T')
		{
			if (ProcessIDAT(file, data, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'E' && nextChunk.MagicNumber[2] == 'N' && nextChunk.MagicNumber[3] == 'D')
			return true;
		//Extension
		if (nextChunk.MagicNumber[0] == 'e' && nextChunk.MagicNumber[1] == 'X' && nextChunk.MagicNumber[2] == 'I' && nextChunk.MagicNumber[3] == 'f' && !alreadyLoaded.eXIf)
			if (ProcesseXIf(file, nextChunk.Length, needSwap))
				return true;
	}

	TP_ERROR("[Image][PNG] Invalid or Multiple Usage of Chunk Magic Number: ", nextChunk.MagicNumber[0], nextChunk.MagicNumber[1], nextChunk.MagicNumber[2], nextChunk.MagicNumber[3], "!");
	TP_WARN("[Image][PNG] Using Default Image!");
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessIHDR(std::ifstream& file, Data& data, const bool needSwap)
{
	IHDRChunk ihdrChunk{};
	//Read in IHDR Chunk		
	file.read(reinterpret_cast<char*>(&ihdrChunk.Width), sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.Height), sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.BitDepth), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.ColorType), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.CompressionMethod), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.FilterMethod), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.InterlaceMethod), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&ihdrChunk.CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
	{
		Utils::Memory::SwapBytes(ihdrChunk.Width);
		Utils::Memory::SwapBytes(ihdrChunk.Height);
		Utils::Memory::SwapBytes(ihdrChunk.CRC);
	}

	std::array<uint8_t, 17> CRCData
	{
		'I',
		'H',
		'D',
		'R',
		reinterpret_cast<uint8_t*>(&ihdrChunk.Width)[3],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Width)[2],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Width)[1],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Width)[0],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Height)[3],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Height)[2],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Height)[1],
		reinterpret_cast<uint8_t*>(&ihdrChunk.Height)[0],
		ihdrChunk.BitDepth,
		ihdrChunk.ColorType,
		ihdrChunk.CompressionMethod,
		ihdrChunk.FilterMethod,
		ihdrChunk.InterlaceMethod
	};

	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != ihdrChunk.CRC)
	{
		TP_ERROR("[Image][PNG] IHDR CRC: ", ihdrChunk.CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
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

bool TRAP::INTERNAL::PNGImage::ProcesscHRM(std::ifstream& file, const bool needSwap)
{
	std::array<uint8_t, 32> unusedData{};
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::array<uint8_t, 36> CRCData
	{
		'c',
		'H',
		'R',
		'M',
		unusedData[0],
		unusedData[1],
		unusedData[2],
		unusedData[3],
		unusedData[4],
		unusedData[5],
		unusedData[6],
		unusedData[7],
		unusedData[8],
		unusedData[9],
		unusedData[10],
		unusedData[11],
		unusedData[12],
		unusedData[13],
		unusedData[14],
		unusedData[15],
		unusedData[16],
		unusedData[17],
		unusedData[18],
		unusedData[19],
		unusedData[20],
		unusedData[21],
		unusedData[22],
		unusedData[23],
		unusedData[24],
		unusedData[25],
		unusedData[26],
		unusedData[27],
		unusedData[28],
		unusedData[29],
		unusedData[30],
		unusedData[31]
	};

	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] cHRM CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessgAMA(std::ifstream& file, const bool needSwap)
{
	std::array<uint8_t, 4> unusedData{};
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::array<uint8_t, 8> CRCData
	{
		'g',
		'A',
		'M',
		'A',
		unusedData[0],
		unusedData[1],
		unusedData[2],
		unusedData[3]
	};

	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] gAMA CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessiCCP(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(length + 4);
	CRCData[0] = 'i';
	CRCData[1] = 'C';
	CRCData[2] = 'C';
	CRCData[3] = 'P';
	for (uint32_t i = 0; i < length; i++)
		CRCData[i + 4] = unusedData[i];

	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] iCCP CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesssBIT(std::ifstream& file, const Data& data, const bool needSwap)
{
	//TODO Use this for decoding? Spec states that it should simplify decoding... 11.3.3.4
	switch (data.ColorType)
	{
	case 0:
	{
		uint8_t significantGrayScaleBits = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&significantGrayScaleBits), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 5> CRCData{ 's', 'B', 'I', 'T', significantGrayScaleBits };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] sBIT CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 2:
	case 3:
	{
		std::array<uint8_t, 3> significantRGBBits{};
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&significantRGBBits), significantRGBBits.size());
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 7> CRCData{ 's', 'B', 'I', 'T', significantRGBBits[0], significantRGBBits[1], significantRGBBits[2] };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] sBIT CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 4:
	{
		std::array<uint8_t, 2> significantGrayScaleAlphaBits{};
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&significantGrayScaleAlphaBits), significantGrayScaleAlphaBits.size());
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 6> CRCData{ 's', 'B', 'I', 'T', significantGrayScaleAlphaBits[0], significantGrayScaleAlphaBits[1] };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] sBIT CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 6:
	{
		std::array<uint8_t, 4> significantRGBAlphaBits{};
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&significantRGBAlphaBits), significantRGBAlphaBits.size());
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 8> CRCData
		{
			's',
			'B',
			'I',
			'T',
			significantRGBAlphaBits[0],
			significantRGBAlphaBits[1],
			significantRGBAlphaBits[2],
			significantRGBAlphaBits[3]
		};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] sBIT CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesssRGB(std::ifstream& file, const bool needSwap)
{
	uint8_t renderingIntent = 0;
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(&renderingIntent), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::array<uint8_t, 5> CRCData{ 's', 'R', 'G', 'B' };
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] sRGB CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessbKGD(std::ifstream& file, const Data& data, const bool needSwap)
{
	switch (data.ColorType)
	{
	case 0:
	case 4:
	{
		uint16_t backgroundColor = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&backgroundColor), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
		{
			Utils::Memory::SwapBytes(backgroundColor);
			Utils::Memory::SwapBytes(CRC);
		}

		std::array<uint8_t, 6> CRCData{ 'b', 'K', 'G', 'D', reinterpret_cast<uint8_t*>(&backgroundColor)[1], reinterpret_cast<uint8_t*>(&backgroundColor)[0] };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] bKGD CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 2:
	case 6:
	{
		uint16_t Red = 0;
		uint16_t Green = 0;
		uint16_t Blue = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&Red), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&Green), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&Blue), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
		{
			Utils::Memory::SwapBytes(Red);
			Utils::Memory::SwapBytes(Green);
			Utils::Memory::SwapBytes(Blue);
			Utils::Memory::SwapBytes(CRC);
		}

		std::array<uint8_t, 10> CRCData
		{
			'b',
			'K',
			'G',
			'D',
			reinterpret_cast<uint8_t*>(&Red)[1],
			reinterpret_cast<uint8_t*>(&Red)[0],
			reinterpret_cast<uint8_t*>(&Green)[1],
			reinterpret_cast<uint8_t*>(&Green)[0],
			reinterpret_cast<uint8_t*>(&Blue)[1],
			reinterpret_cast<uint8_t*>(&Blue)[0]
		};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] bKGD CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 3:
	{
		uint8_t paletteIndex = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&paletteIndex), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 5> CRCData
		{
			'b',
			'K',
			'G',
			'D',
			paletteIndex
		};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] bKGD CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	default:
		return false;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesshIST(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(length + 4);
	CRCData[0] = 'h';
	CRCData[1] = 'I';
	CRCData[2] = 'S';
	CRCData[3] = 'T';
	for (uint32_t i = 0; i < length; i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] hIST CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesstRNS(std::ifstream& file, const uint32_t length, Data& data, const bool needSwap)
{
	switch (data.ColorType)
	{
	case 0:
	{
		uint8_t grayAlpha1 = 0;
		uint8_t grayAlpha2 = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&grayAlpha1), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&grayAlpha2), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 6> CRCData{ 't', 'R', 'N', 'S', grayAlpha1, grayAlpha2 };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] tRNS CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 2:
	{
		uint8_t redAlpha1 = 0;
		uint8_t redAlpha2 = 0;
		uint8_t greenAlpha1 = 0;
		uint8_t greenAlpha2 = 0;
		uint8_t blueAlpha1 = 0;
		uint8_t blueAlpha2 = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&redAlpha1), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&redAlpha2), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&greenAlpha1), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&greenAlpha2), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&blueAlpha1), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&blueAlpha2), sizeof(uint8_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::array<uint8_t, 10> CRCData
		{
			't',
			'R',
			'N',
			'S',
			redAlpha1,
			redAlpha2,
			greenAlpha1,
			greenAlpha2,
			blueAlpha1,
			blueAlpha2
		};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] tRNS CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		return true;
	}

	case 3:
	{
		std::vector<uint8_t> paletteAlpha(length);
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(paletteAlpha.data()), paletteAlpha.size());
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
			Utils::Memory::SwapBytes(CRC);

		std::vector<uint8_t> CRCData(paletteAlpha.size() + 4);
		CRCData[0] = 't';
		CRCData[1] = 'R';
		CRCData[2] = 'N';
		CRCData[3] = 'S';
		for (uint32_t i = 0; i < paletteAlpha.size(); i++)
			CRCData[i + 4] = paletteAlpha[i];
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] tRNS CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
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

bool TRAP::INTERNAL::PNGImage::ProcesspHYs(std::ifstream& file, const bool needSwap)
{
	std::vector<uint8_t> unusedData(9);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unusedData.size() + 4);
	CRCData[0] = 'p';
	CRCData[1] = 'H';
	CRCData[2] = 'Y';
	CRCData[3] = 's';
	for (uint32_t i = 0; i < unusedData.size(); i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] pHYs CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesssPLT(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unusedData.size() + 4);
	CRCData[0] = 's';
	CRCData[1] = 'P';
	CRCData[2] = 'L';
	CRCData[3] = 'T';
	for (uint32_t i = 0; i < unusedData.size(); i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] sPLT CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesstIME(std::ifstream& file, const bool needSwap)
{
	tIMEChunk timeChunk{};
	uint32_t CRC = 0;

	file.read(reinterpret_cast<char*>(&timeChunk.Year), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&timeChunk.Month), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&timeChunk.Day), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&timeChunk.Hour), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&timeChunk.Minute), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&timeChunk.Second), sizeof(uint8_t));
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
	{
		Utils::Memory::SwapBytes(timeChunk.Year);
		Utils::Memory::SwapBytes(CRC);
	}

	std::array<uint8_t, 11> CRCData
	{
		't',
		'I',
		'M',
		'E',
		reinterpret_cast<uint8_t*>(&timeChunk.Year)[1],
		reinterpret_cast<uint8_t*>(&timeChunk.Year)[0],
		timeChunk.Month,
		timeChunk.Day,
		timeChunk.Hour,
		timeChunk.Minute,
		timeChunk.Second
	};
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] tIME CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	if (!tIMECheck(timeChunk))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessiTXt(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unusedData.size() + 4);
	CRCData[0] = 'i';
	CRCData[1] = 'T';
	CRCData[2] = 'X';
	CRCData[3] = 't';
	for (uint32_t i = 0; i < unusedData.size(); i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] iTXt CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesstEXt(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unusedData.size() + 4);
	CRCData[0] = 't';
	CRCData[1] = 'E';
	CRCData[2] = 'X';
	CRCData[3] = 't';
	for (uint32_t i = 0; i < unusedData.size(); i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] tEXt CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesszTXt(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unusedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unusedData.data()), unusedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unusedData.size() + 4);
	CRCData[0] = 'z';
	CRCData[1] = 'T';
	CRCData[2] = 'X';
	CRCData[3] = 't';
	for (uint32_t i = 0; i < unusedData.size(); i++)
		CRCData[i + 4] = unusedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] zTXt CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessPLTE(std::ifstream& file, Data& data, const uint32_t length, const bool needSwap)
{
	if (length % 3 != 0)
	{
		TP_ERROR("[Image][PNG] PLTE Length is not divisible by 3!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if (data.ColorType == 0 || data.ColorType == 4)
	{
		TP_ERROR("[Image][PNG] PLTE Invalid usage! This chunk should not appear with Color Type: ", static_cast<uint32_t>(data.ColorType), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	std::vector<RGBA> paletteData(length / 3);
	uint32_t CRC = 0;
	uint32_t paletteIndex = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		RGBA rgba{};
		file.read(reinterpret_cast<char*>(&rgba.Red), sizeof(uint8_t));
		i++;
		file.read(reinterpret_cast<char*>(&rgba.Green), sizeof(uint8_t));
		i++;
		file.read(reinterpret_cast<char*>(&rgba.Blue), sizeof(uint8_t));

		paletteData[paletteIndex++] = rgba;
	}
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

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
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] PLTE CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	data.Palette = paletteData;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessIDAT(std::ifstream& file, Data& data, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> compressedData(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(compressedData.size() + 4);
	CRCData[0] = 'I';
	CRCData[1] = 'D';
	CRCData[2] = 'A';
	CRCData[3] = 'T';
	for (uint32_t i = 0; i < compressedData.size(); i++)
		CRCData[i + 4] = compressedData[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] IDAT CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	data.CompressedData.insert(data.CompressedData.end(), compressedData.begin(), compressedData.end());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesseXIf(std::ifstream& file, const uint32_t length, const bool needSwap)
{
	std::vector<uint8_t> unused(length);
	uint32_t CRC = 0;
	file.read(reinterpret_cast<char*>(unused.data()), unused.size());
	file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

	//Convert to machines endian
	if (needSwap)
		Utils::Memory::SwapBytes(CRC);

	std::vector<uint8_t> CRCData(unused.size() + 4);
	CRCData[0] = 'e';
	CRCData[1] = 'X';
	CRCData[2] = 'I';
	CRCData[3] = 'f';
	for (uint32_t i = 0; i < unused.size(); i++)
		CRCData[i + 4] = unused[i];
	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] eXIf CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::IHDRCheck(const IHDRChunk& ihdrChunk)
{
	//Check if Width and or Height are (in)valid
	if (!ihdrChunk.Width || !ihdrChunk.Height)
	{
		TP_ERROR("[Image][PNG] Width and or Height are invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check if Bit Depth is a valid number
	if (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 && ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16)
	{
		TP_ERROR("[Image][PNG] Bit Depth is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check if Color Type is a valid number
	if (ihdrChunk.ColorType != 0 && ihdrChunk.ColorType != 2 && ihdrChunk.ColorType != 3 && ihdrChunk.ColorType != 4 && ihdrChunk.ColorType != 6)
	{
		TP_ERROR("[Image][PNG] Color Type is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check if Color Type matches Bit Depth
	if (ihdrChunk.ColorType == 2 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: TrueColor(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if (ihdrChunk.ColorType == 3 && (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 && ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8))
	{
		TP_ERROR("[Image][PNG] Color Type: Indexed-Color(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if (ihdrChunk.ColorType == 4 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: GrayScale Alpha(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if (ihdrChunk.ColorType == 6 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: TrueColor Alpha(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only Deflate/Inflate Compression Method is defined by the ISO
	if (ihdrChunk.CompressionMethod != 0)
	{
		TP_ERROR("[Image][PNG] Compression Method: ", static_cast<uint32_t>(ihdrChunk.CompressionMethod), " is unsupported!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only Adaptive filtering with 5 basic filter types is defined by the ISO
	if (ihdrChunk.FilterMethod != 0)
	{
		TP_ERROR("[Image][PNG] Filter Method: ", static_cast<uint32_t>(ihdrChunk.CompressionMethod), " is unsupported!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only No Interlace and Adam7 Interlace are defined by the ISO
	if (ihdrChunk.InterlaceMethod != 0 && ihdrChunk.InterlaceMethod != 1)
	{
		TP_ERROR("[Image][PNG] Interlace Method: ", static_cast<uint32_t>(ihdrChunk.InterlaceMethod), " is unsupported!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::tIMECheck(const tIMEChunk& timeChunk)
{
	//Check Month validity
	if (timeChunk.Month < 1 || timeChunk.Month > 12)
	{
		TP_ERROR("[Image][PNG] Time stamp Month: ", static_cast<uint32_t>(timeChunk.Month), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Day validity
	if (timeChunk.Day < 1 || timeChunk.Day > 31)
	{
		TP_ERROR("[Image][PNG] Time stamp Day: ", static_cast<uint32_t>(timeChunk.Day), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Hour validity
	if (timeChunk.Hour < 0 || timeChunk.Hour > 23)
	{
		TP_ERROR("[Image][PNG] Time stamp Hour: ", static_cast<uint32_t>(timeChunk.Hour), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Minute validity
	if (timeChunk.Minute < 0 || timeChunk.Minute > 59)
	{
		TP_ERROR("[Image][PNG] Time stamp Minute: ", static_cast<uint32_t>(timeChunk.Minute), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Second validity
	if (timeChunk.Second < 0 || timeChunk.Second > 60)
	{
		TP_ERROR("[Image][PNG] Time stamp Second: ", timeChunk.Second, " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	return true;
}

bool TRAP::INTERNAL::PNGImage::DecompressData(uint8_t* source, const int sourceLength, uint8_t* destination, const int destinationLength, bool needSwap)
{
	if (sourceLength < 2)
	{
		TP_ERROR("[Image][PNG] Compressed zlib data is too small!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false; //Error, size of zlib data too small
	}
	if ((source[0] * 256 + source[1]) % 31 != 0)
	{
		TP_ERROR("[Image][PNG] Decompression Failed! 256 * source[0](", source[0], ") + source[1](", source[1], ") must be a multiple of 31(", static_cast<uint32_t>(source[0] * 256 + source[1]), ")!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false; //Error: 256 * source[0] + source[1] must be a multiple of 31, the FCHECK value is supposed to be made this way
	}

	const uint32_t CM = source[0] & 15;
	const uint32_t CINFO = (source[0] >> 4) & 15;
	//FCHECK = source[1] & 31; //FCHECK is already tested above
	const uint32_t FDICT = (source[1] >> 5) & 1;
	//FLEVEL = (source[1] >> 6) & 3; //FLEVEL is not used here

	if (CM != 8 || CINFO > 7)
	{
		TP_ERROR("[Image][PNG] Decompression Failed! Only Compression Method 8(De/Inflate) with sliding window of 32K is supported by the PNG Specification!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false; //Error: Only compression method 8: inflate with sliding window of 32K is supported by the PNG specification
	}
	if(FDICT != 0)
	{
		TP_ERROR("[Image][PNG] Decompression Failed! Additional Flags should not specify a preset dictionary!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false; //Error: The PNG specification says that the zlib stream should not specify a preset dictionary in the additional flags!
	}

	if (!Utils::Decompress::Inflate(source + 2, sourceLength - 2, destination, destinationLength))
	{
		TP_ERROR("[Image][PNG] Decompression Failed! Inflate Error!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	uint8_t* buf = &source[sourceLength - 4];
	uint32_t adler32 = ((static_cast<uint32_t>(buf[0]) << 24u) | (static_cast<uint32_t>(buf[1]) << 16u) |
		               (static_cast<uint32_t>(buf[2]) << 8u) | static_cast<uint32_t>(buf[3]));
	uint32_t checksum = Utils::Hash::Adler32(destination, destinationLength);
	if(needSwap)
	{
		Utils::Memory::SwapBytes(adler32);
		Utils::Memory::SwapBytes(checksum);		
	}
	if (checksum != adler32)
	{
		TP_ERROR("[Image][PNG] Decompression Failed! Adler32 Checksum: ", adler32, " doesnt match Checksum: ", checksum, "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false; //Error: Adler checksum not correct, data must be corrupt
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::UnFilterScanline(uint8_t* recon,
	const uint8_t* scanline,
	const uint8_t* precon,
	const std::size_t byteWidth,
	const uint8_t filterType,
	const std::size_t length)
{
	//For PNG Filter Method 0
	//UnFilter a PNG Image Scanline by Scanline.
	//When the pixels are smaller than 1 Byte, the Filter works Byte per Byte (byteWidth = 1)
	//precon is the previous unFiltered Scanline, recon the result, scanline the current one the incoming Scanlines do NOT include the FilterType byte, that one is given in the parameter filterType instead
	//recon and scanline MAY be the same memory address!
	//precon must be disjoint.
	std::size_t i;
	switch (filterType)
	{
	case 0:
		for (i = 0; i != length; ++i)
			recon[i] = scanline[i];
		break;

	case 1:
		for (i = 0; i != byteWidth; ++i)
			recon[i] = scanline[i];
		for (i = byteWidth; i < length; ++i)
			recon[i] = scanline[i] + recon[i - byteWidth];
		break;

	case 2:
		if (precon)
			for (i = 0; i != length; ++i)
				recon[i] = scanline[i] + precon[i];
		else
			for (i = 0; i != length; ++i)
				recon[i] = scanline[i];
		break;

	case 3:
		if (precon)
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i] + (precon[i] >> 1u);
			for (i = byteWidth; i < length; ++i)
				recon[i] = scanline[i] + ((recon[i - byteWidth] + precon[i]) >> 1u);
		}
		else
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i];
			for (i = byteWidth; i < length; ++i)
				recon[i] = scanline[i] + (recon[i - byteWidth] >> 1u);
		}
		break;

	case 4:
		if (precon)
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = (scanline[i] + precon[i]);

			//Unroll independent paths of the Paeth predictor.
			//A 6x and 8x version would also be possible but that adds too much code.
			//Whether this actually speeds anything up at all depends on compiler and settings.
			if (byteWidth >= 4)
			{
				for (; i + 3 < length; i += 4)
				{
					const std::size_t j = i - byteWidth;
					const uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1], s2 = scanline[i + 2], s3 = scanline[i + 3];
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
				for (; i + 2 < length; i += 3)
				{
					const std::size_t j = i - byteWidth;
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
				for (; i + 1 < length; i += 2)
				{
					const std::size_t j = i - byteWidth;
					const uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1];
					const uint8_t r0 = recon[j + 0], r1 = recon[j + 1];
					const uint8_t p0 = precon[i + 0], p1 = precon[i + 1];
					const uint8_t q0 = precon[j + 0], q1 = precon[j + 1];
					recon[i + 0] = s0 + PaethPredictor(r0, p0, q0);
					recon[i + 1] = s1 + PaethPredictor(r1, p1, q1);
				}
			}

			for (; i != length; ++i)
				recon[i] = (scanline[i] + PaethPredictor(recon[i - byteWidth], precon[i], precon[i - byteWidth]));
		}
		else
		{
			for (i = 0; i != byteWidth; ++i)
				recon[i] = scanline[i];
			for (i = byteWidth; i < length; ++i)
				recon[i] = (scanline[i] + recon[i - byteWidth]);
		}
		break;

	default:
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::UnFilter(uint8_t* out, const uint8_t* in, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel)
{
	//For PNG Filter Method 0
	//This function unFilters a single image(e.g. without interlacing this is called once, with Adam7 seven times)
	//out must have enough bytes allocated already, in must have the scanlines + 1 filterType byte per scanline
	//width and height are image dimensions or dimensions of reduced image, bitsPerPixel is bits per pixel
	//in and out are allowed to be the same memory address(but are not the same size since in has the extra filter Bytes)
	uint8_t* prevLine = nullptr;

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

uint8_t TRAP::INTERNAL::PNGImage::PaethPredictor(uint16_t a, const uint16_t b, uint16_t c)
{
	uint16_t pa = Math::Abs(b - c);
	const uint16_t pb = Math::Abs(a - c);
	const uint16_t pc = Math::Abs(a + b - c - c);

	//Return input value associated with smallest of pa, pb, pc(with certain priority if equal)
	if (pb < pa)
	{
		a = b;
		pa = pb;
	}

	return static_cast<uint8_t>((pc < pa) ? c : a);
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::INTERNAL::PNGImage::GetRawSizeIDAT(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel)
{
	//In an IDAT chunk, each scanline is a multiple of 8 bits and in addition has one extra byte per line: the filter byte.
	//+ 1 for the filter byte, and possibly plus padding bits per line
	const std::size_t line = (static_cast<std::size_t>(width / 8u)* bitsPerPixel) + 1u + ((width & 7u) * bitsPerPixel + 7u) / 8u;

	return static_cast<std::size_t>(height)* line;
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::INTERNAL::PNGImage::GetRawSize(const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel)
{
	const std::size_t n = static_cast<std::size_t>(width)* static_cast<std::size_t>(height);
	return ((n / 8u) * bitsPerPixel) + ((n & 7u) * bitsPerPixel + 7u) / 8u;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::PostProcessScanlines(uint8_t* out, uint8_t* in, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel, const uint8_t interlaceMethod)
{
	//out must be a buffer big enough to contain full image, and in must contain the full decompressed data from the IDAT chunks(with filter bytes and possible padding bits)
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
			if (!UnFilter(&in[paddedPassStart[i]], &in[filterPassStart[i]], passW[i], passH[i], bitsPerPixel))
				return false;

		Adam7DeInterlace(out, in, width, height, bitsPerPixel);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

static const std::array<uint32_t, 7> ADAM7_IX = { 0, 4, 0, 2, 0, 1, 0 }; /*X start values*/
static const std::array<uint32_t, 7> ADAM7_IY = { 0, 0, 4, 0, 2, 0, 1 }; /*Y start values*/
static const std::array<uint32_t, 7> ADAM7_DX = { 8, 8, 4, 4, 2, 2, 1 }; /*X delta values*/
static const std::array<uint32_t, 7> ADAM7_DY = { 8, 8, 8, 4, 4, 2, 2 }; /*Y delta values*/

void TRAP::INTERNAL::PNGImage::Adam7GetPassValues(std::array<uint32_t, 7>& passW,
	std::array<uint32_t, 7>& passH,
	std::array<std::size_t, 8>& filterPassStart,
	std::array<std::size_t, 8>& paddedPassStart,
	std::array<std::size_t, 8>& passStart,
	const uint32_t width,
	const uint32_t height,
	const uint32_t bitsPerPixel)
{
	//Outputs various dimensions and positions in the image related to the Adam7 reduced images.
	//passW: Output containing the width of the 7 passes
	//passH: Output containing the height of the 7 passes
	//filterPassStart: Output containing the index of the start and end of each reduced image with filter bytes
	//paddedPassStat: Output containing the index of the start and end of each reduced image when without filter bytes but with padded scanlines
	//passStart: Output containing the index of the start and end of each reduced image without padding between scanlines, but still padding between the images
	//width, height: Width and Height of non-interlaced image
	//bitsPerPixel: bits per pixel
	//"padded" is only relevant if bitsPerPixel is less than 8 and a scanline or image does not end at a full byte

	//The passStart values have 8 values: The 8th one indicates the byte after the end of the 7th(= last) pass
	uint32_t i;

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

	filterPassStart[0] = paddedPassStart[0] = passStart[0] = 0;
	for (i = 0; i != 7; ++i)
	{
		//If passW[i] is 0, its 0 bytes, not 1(no filterType-byte)
		filterPassStart[i + 1] = filterPassStart[i] + ((passW[i] && passH[i]) ? passH[i] * (1u + (passW[i] * bitsPerPixel + 7u) / 8u) : 0);
		//Bits padded if needed to fill full byte at the end of each scanline
		paddedPassStart[i + 1] = paddedPassStart[i] + passH[i] * ((passW[i] * bitsPerPixel + 7u) / 8u);
		//Only padded at end of reduced image
		passStart[i + 1] = passStart[i] + (passH[i] * passW[i] * bitsPerPixel + 7u) / 8u;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PNGImage::Adam7DeInterlace(uint8_t* out, const uint8_t* in, const uint32_t width, const uint32_t height, const uint32_t bitsPerPixel)
{
	//in: Adam7 Interlaced Image, with no padding bits between scanlines, but between reduced images so that each reduced image starts at a byte.
	//out: The same pixels, but re-ordered so that they are now a non-interlaced image with size width * height
	//bitsPerPixel: bits per pixel
	//out has the following size in bits: width * height * bitsPerPixel.
	//in is possibly bigger due to padding bits between reduced images.
	//out must be big enough AND must be 0 everywhere if bitsPerPixel < 8 in the current implementation(because that is likely a little bit faster)
	//NOTE: Comments about padding bits are only relevant if bitsPerPixel < 8

	std::array<uint32_t, 7> passW{}, passH{};
	std::array<std::size_t, 8> filterPassStart{}, paddedPassStart{}, passStart{};

	Adam7GetPassValues(passW, passH, filterPassStart, paddedPassStart, passStart, width, height, bitsPerPixel);

	if (bitsPerPixel >= 8)
	{
		for (uint32_t i = 0; i != 7; ++i)
		{
			const std::size_t byteWidth = bitsPerPixel / 8u;
			for (uint32_t y = 0; y < passH[i]; ++y)
				for (uint32_t x = 0; x < passW[i]; ++x)
				{
					const std::size_t pixelInStart = passStart[i] + (y * passW[i] + x) * byteWidth;
					const std::size_t pixelOutStart = ((ADAM7_IY[i] + y * ADAM7_DY[i]) * width + ADAM7_IX[i] + x * ADAM7_DX[i]) * byteWidth;
					for (uint32_t b = 0; b < byteWidth; ++b)
						out[pixelOutStart + b] = in[pixelInStart + b];
				}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint16_t> TRAP::INTERNAL::PNGImage::ConvertTo2Byte(std::vector<uint8_t>& raw)
{
	std::vector<uint16_t> result(raw.size() / 2, 0);
	uint32_t resultIndex = 0;
	if (Application::GetEndian() == Application::Endian::Big)
	{
		for (uint32_t i = 0; i < raw.size(); i += 2)
		{
			const uint16_t val = (static_cast<uint16_t>(raw[i + 1]) << 8) | raw[i];
			result[resultIndex++] = val;
		}
	}
	else
	{
		for (uint32_t i = 0; i < raw.size(); i += 2)
		{
			const uint16_t val = (static_cast<uint16_t>(raw[i + 1]) << 8) | raw[i];
			result[resultIndex++] = val;
		}

		for (uint16_t& i : result)
			Utils::Memory::SwapBytes(i);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<uint8_t> TRAP::INTERNAL::PNGImage::ResolveIndexed(std::vector<uint8_t>& raw, const uint32_t width, const uint32_t height, const Data& data)
{
	std::vector<uint8_t> result(width * height * 4, 0);
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