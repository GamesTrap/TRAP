#include "TRAPPCH.h"
#include "PNGImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Application.h"
#include "Utils/ByteSwap.h"
#include "Utils/Hash/CRC32.h"

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
		TP_DEBUG("[Image][PNG] Reading File Magic Number"); //TODO Remove
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
		//TODO Remove
		TP_DEBUG("[Image][PNG] File Magic Number is valid");
		////////////////////

		//File uses big-endian
		bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Big);
		//TODO Remove
		TP_DEBUG("[Image][PNG] Bytes will ", (needSwap ? "" : "not "), "be swapped!");

		//Load Chunk
		//TODO Remove
		TP_DEBUG("[Image][PNG] Reading Chunks...");
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

				if(nextChunk.Length > 2147483647)
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
		//TODO Remove
		TP_DEBUG("[Image][PMG] Width: ", data.Width);
		TP_DEBUG("[Image][PNG] Height: ", data.Height);
		TP_DEBUG("[Image][PNG] Bit Depth: ", static_cast<uint32_t>(data.BitDepth));
		if (!data.Palette.empty())
			TP_DEBUG("[Image][PNG] Color Palette Exists");
		TP_DEBUG("[Image][PNG] Color Type: ", static_cast<uint32_t>(data.ColorType));
		TP_DEBUG("[Image][PNG] Compression Method: ", static_cast<uint32_t>(data.CompressionMethod));
		TP_DEBUG("[Image][PNG] Filter Method: ", static_cast<uint32_t>(data.FilterMethod));
		TP_DEBUG("[Image][PNG] Interlace Method: ", static_cast<uint32_t>(data.InterlaceMethod));

		if(data.CompressionMethod != 0)
		{
			TP_ERROR("[Image][PNG] Compression Method is invalid!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}
		
		//TODO
		//DeInterlace(No, Adam7)
		//DeFilter(0)		

		//WIP////////////////////////////////////////////////////////////////////////
		//WIP////////////////////////////////////////////////////////////////////////
		//WIP////////////////////////////////////////////////////////////////////////
		bool convertByteto2Bytes = false;
		if(data.BitDepth == 8)
		{
			if (data.ColorType == 0)
				m_data.resize(m_width * m_height * 2);
			else if (data.ColorType == 2)
				m_data.resize(m_width * m_height * 4);
			else if (data.ColorType == 3)
				m_data.resize(m_width * m_height * 2); //TODO Correct?
			else if (data.ColorType == 4)
				m_data.resize(m_width * m_height * 3);
			else if (data.ColorType == 6)
				m_data.resize(m_width * m_height * 5);
		}
		else if(data.BitDepth == 16)
		{
			convertByteto2Bytes = true;
			if (data.ColorType == 0)
				m_data.resize((m_width * m_height) * 2 * 2);
			else if (data.ColorType == 2)
				m_data.resize((m_width * m_height) * 2 * 4);
			else if (data.ColorType == 3)
				m_data.resize((m_width * m_height) * 2 * 2); //TODO Correct?
			else if (data.ColorType == 4)
				m_data.resize((m_width * m_height) * 2 * 3);
			else if (data.ColorType == 6)
				m_data.resize((m_width * m_height) * 2 * 5);

		}
		//TODO Remove
		TP_DEBUG("[Image][PNG] Decompressing(Inflate) Data...");
		if (DecompressData(data.CompressedData.data(), data.CompressedData.size(), m_data.data(), m_data.size()) < 1)
		{
			m_data.clear();
			TP_ERROR("[Image][PNG] Decompression Failed!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return;
		}
		TP_DEBUG("[Image][PNG] Decompression Successful!");
		//WIP////////////////////////////////////////////////////////////////////////
		//WIP////////////////////////////////////////////////////////////////////////
		//WIP////////////////////////////////////////////////////////////////////////
		
		//TODO Remove
		TP_WARN("[Image][PNG] Interlacing is not implemented!");
		TP_WARN("[Image][PNG] Filtering is not implemented!");

		switch(data.ColorType)
		{
		case 0: //GrayScale
			m_format = ImageFormat::Gray_Scale;
			m_isImageGrayScale = true;
			switch(data.BitDepth)
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
			m_format = ImageFormat::RGB;
			m_isImageColored = true;
			if(data.BitDepth == 8)
			{
				m_bitsPerPixel = 24;
			}
			else if(data.BitDepth == 16)
			{
				m_bitsPerPixel = 48;
			}
			break;

		case 3: //Indexed Color
			m_format = ImageFormat::RGBA;
			m_isImageColored = true;
			m_hasAlphaChannel = true;
			switch(data.BitDepth)
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
				m_bitsPerPixel = 32;
				break;
				
			default:
				break;
			}
			break;

		case 4: //GrayScale Alpha
			m_format = ImageFormat::Gray_Scale_Alpha;
			m_isImageGrayScale = true;
			m_hasAlphaChannel = true;
			if(data.BitDepth == 8)
			{
				m_bitsPerPixel = 16;
			}
			else if(data.BitDepth == 16)
			{
				m_bitsPerPixel = 32;
			}
			break;

		case 6: //TrueColor Alpha
			m_format = ImageFormat::RGBA;
			m_isImageColored = true;
			m_hasAlphaChannel = true;
			if(data.BitDepth == 8)
			{
				m_bitsPerPixel = 32;
			}
			else if(data.BitDepth == 16)
			{
				m_bitsPerPixel = 64;
			}		
			break;
			
		default:
			break;
		}
		
		bool needYFlip = false;
		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_format, m_data.data());

		file.close();
	}

	//TODO Remove if loading works
	TP_ERROR("[Image][PNG] PNG support is WIP");
	TP_WARN("[Image][PNG] Using Default Image!");
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
	//TODO Remove
	TP_DEBUG("[Image][PNG] Found ",
	         static_cast<unsigned char>(nextChunk.MagicNumber[0]),
	         static_cast<unsigned char>(nextChunk.MagicNumber[1]),
	         static_cast<unsigned char>(nextChunk.MagicNumber[2]),
	         static_cast<unsigned char>(nextChunk.MagicNumber[3]),
	         " Header");
	if(nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'H' && nextChunk.MagicNumber[2] == 'D' && nextChunk.MagicNumber[3] == 'R' && !alreadyLoaded.IHDR)
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
		if (nextChunk.MagicNumber[0] == 'b' && nextChunk.MagicNumber[1] == 'K' && nextChunk.MagicNumber[2] == 'G' && nextChunk.MagicNumber[3] == 'D' && !alreadyLoaded.bKGD && alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.bKGD = true;
			if (ProcessbKGD(file, data, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'h' && nextChunk.MagicNumber[1] == 'I' && nextChunk.MagicNumber[2] == 'S' && nextChunk.MagicNumber[3] == 'T' && !alreadyLoaded.hIST && alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.hIST = true;
			if (ProcesshIST(file, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 't' && nextChunk.MagicNumber[1] == 'R' && nextChunk.MagicNumber[2] == 'N' && nextChunk.MagicNumber[3] == 'S' && !alreadyLoaded.tRNS && alreadyLoaded.PLTE && !alreadyLoaded.IDAT)
		{
			alreadyLoaded.tRNS = true;
			if (ProcesstRNS(file, data, needSwap))
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
		if(nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'D' && nextChunk.MagicNumber[2] == 'A' && nextChunk.MagicNumber[3] == 'T')
		{
			if (ProcessIDAT(file, data, nextChunk.Length, needSwap))
				return true;
		}
		if (nextChunk.MagicNumber[0] == 'I' && nextChunk.MagicNumber[1] == 'E' && nextChunk.MagicNumber[2] == 'N' && nextChunk.MagicNumber[3] == 'D')
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] IHDR Chunk CRC: ", ihdrChunk.CRC, " is valid");

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

	//TODO Remove
	TP_DEBUG("[Image][PNG] cHRM Chunk CRC: ", CRC, " is valid");
	
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] gAMA Chunk CRC: ", CRC, " is valid");

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
	for(uint32_t i = 0; i < length; i++)
		CRCData[i + 4] = unusedData[i];	

	if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
	{
		TP_ERROR("[Image][PNG] iCCP CRC: ", CRC, " is wrong!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//TODO Remove
	TP_DEBUG("[Image][PNG] iCCP Chunk CRC: ", CRC, " is valid");

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesssBIT(std::ifstream& file, const Data& data, const bool needSwap)
{
	//TODO Use this for decoding? Spec states that it should simplify decoding... 11.3.3.4
	switch(data.ColorType)
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
			
		std::array<uint8_t, 5> CRCData{ 's', 'B', 'I', 'T', significantGrayScaleBits};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] sBIT CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		//TODO Remove
		TP_DEBUG("[Image][PNG] sBIT Chunk CRC: ", CRC, " is valid");
			
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] sBIT Chunk CRC: ", CRC, " is valid");
			
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] sBIT Chunk CRC: ", CRC, " is valid");
			
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] sBIT Chunk CRC: ", CRC, " is valid");
			
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] sRGB Chunk CRC: ", CRC, " is valid");

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessbKGD(std::ifstream& file, const Data& data, const bool needSwap)
{
	//TODO Use this color as init for the image array?
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] bKGD Chunk CRC: ", CRC, " is valid");

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

		//TODO Remove
		TP_DEBUG("[Image][PNG] bKGD Chunk CRC: ", CRC, " is valid");

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

		std::array<uint8_t, 10> CRCData
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] bKGD Chunk CRC: ", CRC, " is valid");

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
	
	//TODO Remove
	TP_DEBUG("[Image][PNG] hIST Chunk CRC: ", CRC, " is valid");

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcesstRNS(std::ifstream& file, Data& data, const bool needSwap)
{
	//TODO Use this alpha values
	switch (data.ColorType)
	{
	case 0:
	{
		uint16_t grayAlpha = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&grayAlpha), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));
			
		//Convert to machines endian
		if(needSwap)
		{
			Utils::Memory::SwapBytes(grayAlpha);
			Utils::Memory::SwapBytes(CRC);
		}

		std::array<uint8_t, 6> CRCData{'t', 'R', 'N', 'S', reinterpret_cast<uint8_t*>(&grayAlpha)[1], reinterpret_cast<uint8_t*>(&grayAlpha)[0] };
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] tRNS CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		//TODO Remove
		TP_DEBUG("[Image][PNG] tRNS Chunk CRC: ", CRC, " is valid");

		return true;
	}

	case 2:
	{
		uint16_t redAlpha = 0;
		uint16_t greenAlpha = 0;
		uint16_t blueAlpha = 0;
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(&redAlpha), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&greenAlpha), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&blueAlpha), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&CRC), sizeof(uint32_t));

		//Convert to machines endian
		if (needSwap)
		{
			Utils::Memory::SwapBytes(redAlpha);
			Utils::Memory::SwapBytes(greenAlpha);
			Utils::Memory::SwapBytes(blueAlpha);
			Utils::Memory::SwapBytes(CRC);
		}

		std::array<uint8_t, 10> CRCData
		{
			't',
			'R',
			'N',
			'S',
			reinterpret_cast<uint8_t*>(&redAlpha)[1],
			reinterpret_cast<uint8_t*>(&redAlpha)[0],
			reinterpret_cast<uint8_t*>(&greenAlpha)[1],
			reinterpret_cast<uint8_t*>(&greenAlpha)[0],
			reinterpret_cast<uint8_t*>(&blueAlpha)[1],
			reinterpret_cast<uint8_t*>(&blueAlpha)[0]
		};
		if (Utils::Hash::CRC32(CRCData.data(), CRCData.size()) != CRC)
		{
			TP_ERROR("[Image][PNG] tRNS CRC: ", CRC, " is wrong!");
			TP_WARN("[Image][PNG] Using Default Image!");
			return false;
		}

		//TODO Remove
		TP_DEBUG("[Image][PNG] tRNS Chunk CRC: ", CRC, " is valid");
			
		return true;
	}

	case 3:
	{
		std::vector<uint8_t> paletteAlpha(data.Palette.size());
		uint32_t CRC = 0;
		file.read(reinterpret_cast<char*>(paletteAlpha.data()), data.Palette.size());
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] tRNS Chunk CRC: ", CRC, " is valid");
			
		for(uint32_t i = 0; i < paletteAlpha.size(); i++)
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] pHYs Chunk CRC: ", CRC, " is valid");

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
	
	//TODO Remove
	TP_DEBUG("[Image][PNG] sPLT Chunk CRC: ", CRC, " is valid");

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
	if(needSwap)
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] tIME Chunk CRC: ", CRC, " is valid");
	
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] iTXt Chunk CRC: ", CRC, " is valid");
	
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

	//TODO Remove
	TP_DEBUG("[Image][PNG] tEXt Chunk CRC: ", CRC, " is valid");

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

	//TODO Remove
	TP_DEBUG("[Image][PNG] zTXt Chunk CRC: ", CRC, " is valid");

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::ProcessPLTE(std::ifstream& file, Data& data, const uint32_t length, const bool needSwap)
{
	if (data.ColorType == 3) //Speed up for loading ColorTypes 2 and 6 because PLTE is optional there
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

		std::vector<RGBA> paletteData(length);
		uint32_t CRC = 0;
		for (uint32_t i = 0; i < length; i++)
		{
			RGBA rgba{};
			file.read(reinterpret_cast<char*>(&rgba.Red), sizeof(uint8_t));
			file.read(reinterpret_cast<char*>(&rgba.Green), sizeof(uint8_t));
			file.read(reinterpret_cast<char*>(&rgba.Blue), sizeof(uint8_t));

			paletteData[i] = rgba;
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

		//TODO Remove
		TP_DEBUG("[Image][PNG] PLTE Chunk CRC: ", CRC, " is valid");

		data.Palette = paletteData;
	}

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

	//TODO Remove
	TP_DEBUG("[Image][PNG] IDAT Chunk CRC: ", CRC, " is valid");

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
	if(ihdrChunk.ColorType == 2 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: TrueColor(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if(ihdrChunk.ColorType == 3 && (ihdrChunk.BitDepth != 1 && ihdrChunk.BitDepth != 2 && ihdrChunk.BitDepth != 4 && ihdrChunk.BitDepth != 8))
	{
		TP_ERROR("[Image][PNG] Color Type: Indexed-Color(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if(ihdrChunk.ColorType == 4 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: GrayScale Alpha(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	if(ihdrChunk.ColorType == 6 && (ihdrChunk.BitDepth != 8 && ihdrChunk.BitDepth != 16))
	{
		TP_ERROR("[Image][PNG] Color Type: TrueColor Alpha(", static_cast<uint32_t>(ihdrChunk.ColorType), ") doesnt allow a bit depth of ", static_cast<uint32_t>(ihdrChunk.BitDepth), "!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only Deflate/Inflate Compression Method is defined by the ISO
	if(ihdrChunk.CompressionMethod != 0)
	{
		TP_ERROR("[Image][PNG] Compression Method: ", static_cast<uint32_t>(ihdrChunk.CompressionMethod), " is unsupported!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only Adaptive filtering with 5 basic filter types is defined by the ISO
	if(ihdrChunk.FilterMethod != 0)
	{
		TP_ERROR("[Image][PNG] Filter Method: ", static_cast<uint32_t>(ihdrChunk.CompressionMethod), " is unsupported!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Only No Interlace and Adam7 Interlace are defined by the ISO
	if(ihdrChunk.InterlaceMethod != 0 && ihdrChunk.InterlaceMethod != 1)
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
	if(timeChunk.Month < 1 || timeChunk.Month > 12)
	{
		TP_ERROR("[Image][PNG] Time stamp Month: ", static_cast<uint32_t>(timeChunk.Month), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Day validity
	if(timeChunk.Day < 1 || timeChunk.Day > 31)
	{
		TP_ERROR("[Image][PNG] Time stamp Day: ", static_cast<uint32_t>(timeChunk.Day), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Hour validity
	if(timeChunk.Hour < 0 || timeChunk.Hour > 23)
	{
		TP_ERROR("[Image][PNG] Time stamp Hour: ", static_cast<uint32_t>(timeChunk.Hour), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Minute validity
	if(timeChunk.Minute < 0 || timeChunk.Minute > 59)
	{
		TP_ERROR("[Image][PNG] Time stamp Minute: ", static_cast<uint32_t>(timeChunk.Minute), " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}

	//Check Second validity
	if(timeChunk.Second < 0 || timeChunk.Second > 60)
	{
		TP_ERROR("[Image][PNG] Time stamp Second: ", timeChunk.Second, " is invalid!");
		TP_WARN("[Image][PNG] Using Default Image!");
		return false;
	}
	
	return true;
}

/*//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::PNGImage::MakeCRCTable()
{
	for(int n = 0; n < 256; n++)
	{
		uint64_t c = static_cast<uint64_t>(n);
		for(int k = 0; k < 8; k++)
		{
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
		}
		m_CRCTable[n] = c;
	}
	
	m_CRCTableComputed = true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::PNGImage::CheckCRC(std::vector<uint8_t>& CRCData, const uint32_t CRC)
{
	return ((UpdateCRC(0xffffffffL, CRCData) ^ 0xffffffffL) == CRC);
}

//-------------------------------------------------------------------------------------------------------------------//

uint64_t TRAP::INTERNAL::PNGImage::UpdateCRC(const uint64_t CRC, std::vector<uint8_t>& CRCData)
{
	uint64_t c = CRC;

	if (!m_CRCTableComputed)
		MakeCRCTable();
	for (uint32_t n = 0; n < CRCData.size(); n++)
		c = m_CRCTable[(c ^ CRCData[n]) & 0xFF] ^ (c >> 8);

	return c;
}

//-------------------------------------------------------------------------------------------------------------------//*/

int32_t TRAP::INTERNAL::PNGImage::DecompressData(uint8_t* source, const int sourceLength, uint8_t* destination, const int destinationLength)
{
	//TODO Replace with on Inflate Algorithm implementation
	zng_stream stream{};
	stream.total_in = stream.avail_in = sourceLength;
	stream.total_out = stream.avail_out = destinationLength;
	stream.next_in = source;
	stream.next_out = destination;

	int result = -1;
	int error = zng_inflateInit(&stream);
	if(error == Z_OK)
	{
		error = zng_inflate(&stream, Z_FINISH);
		if (error == Z_STREAM_END)
			result = static_cast<int>(stream.total_out);
	}
	zng_inflateEnd(&stream);

	return result;
}