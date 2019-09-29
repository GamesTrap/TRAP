#include "TRAPPCH.h"
#include "TGAImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"

TRAP::INTERNAL::TGAImage::TGAImage(std::string filepath)
	: m_filepath(std::move(filepath)),
	m_bitsPerPixel(0),
	m_width(0),
	m_height(0),
	m_isImageGrayScale(false),
	m_isImageColored(false),
	m_isImageCompressed(false),
	m_hasAlphaChannel(false),
	m_imageFormat(ImageFormat::NONE)
{
	TRAP_DEBUG("[Image][TGA] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TRAP_ERROR("[Image][TGA] Couldn't resolve FilePath: ", m_filepath, "!");
		TRAP_WARN("[Image][TGA] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TRAP_ERROR("[Image][TGA] Couldn't open FilePath: ", m_filepath, "!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;
		}

		//Start TGA Loading here
		struct Header
		{
			uint8_t IDLength = 0; //0 = No ImageID
			uint8_t ColorMapType = 0; //0 = No Color Map | 1 = Has Color Map
			uint8_t ImageType = 0; //0 = No Data | 1 = Uncompressed ColorMapped | 2 = Uncompressed TrueColor | 3 = Uncompressed BlackWhite | 9 = RunLengthEncoded ColorMapped | 10 = RunLengthEncoded TrueColor | 11 = RunLengthEncoded BlackWhite
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
		} header;

		file.read(reinterpret_cast<char*>(&header.IDLength), 1);
		file.read(reinterpret_cast<char*>(&header.ColorMapType), 1);
		file.read(reinterpret_cast<char*>(&header.ImageType), 1);
		file.read(reinterpret_cast<char*>(&header.ColorMapOffset), 2);
		file.read(reinterpret_cast<char*>(&header.NumOfColorMaps), 2);
		file.read(reinterpret_cast<char*>(&header.ColorMapDepth), 1);
		file.read(reinterpret_cast<char*>(&header.XOffset), 2);
		file.read(reinterpret_cast<char*>(&header.YOffset), 2);
		file.read(reinterpret_cast<char*>(&header.Width), 2);
		file.read(reinterpret_cast<char*>(&header.Height), 2);
		file.read(reinterpret_cast<char*>(&header.BitsPerPixel), 1);
		file.read(reinterpret_cast<char*>(&header.ImageDescriptor), 1);

		struct ColorMapData
		{
			std::string ImageID = "";
			std::vector<uint8_t> ColorMap{};
			std::vector<uint8_t> ImageData{};
		} colorMapData;

		if (header.ImageType == 0)
		{
			file.close();
			TRAP_ERROR("[Image][TGA] Image doesn't contain pixel data!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		if (header.IDLength != 0)
		{
			colorMapData.ImageID.resize(header.IDLength);
			file.read(static_cast<char*>(colorMapData.ImageID.data()), header.IDLength);
		}
		if (header.ColorMapType == 1)
		{
			colorMapData.ColorMap.resize((header.ColorMapDepth / 8) * header.NumOfColorMaps);
			if(!file.read(reinterpret_cast<char*>(colorMapData.ColorMap.data()), (header.ColorMapDepth / 8) * header.NumOfColorMaps))
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Couldn't load Color Map!");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
		}
		if(header.BitsPerPixel == 15)
		{
			file.close();
			TRAP_ERROR("[Image][TGA] BitsPerPixel 15 is unsupported!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		bool needXFlip = false;
		bool needYFlip = false;
		if ((header.ImageDescriptor & 0x30) == 0x30 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Top/Right | 2. If Image is stored Bottom/Right
			needXFlip = true;
		if ((header.ImageDescriptor & 0x30) == 0x00 || (header.ImageDescriptor & 0x30) == 0x10) //1. If Image is stored Bottom/Left | 2. If Image is stored Bottom/Right
			needYFlip = true;
		if (header.Width < 2 || header.Height < 2)
		{
			file.close();
			TRAP_ERROR("[Image][TGA] Image Width/Height is invalid/unsupported!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;
		}
		if(header.ImageType == 9 || header.ImageType == 11 || header.ImageType == 10) //All RLE formats
		{
			uint32_t currentPosition = static_cast<uint32_t>(file.tellg()); //Store current position in file
			file.seekg(0, std::ios::end); //Go to the end of file
			uint32_t pixelDataSize = static_cast<uint32_t>(file.tellg()) - currentPosition;
			file.seekg(-18, std::ios::end); //Check if there is a footer
			std::string offsetCheck{16, '\0'};
			file.read(offsetCheck.data(), 16);
			if (offsetCheck == "TRUEVISION-XFILE")
				pixelDataSize -= 26; //If a footer was found subtract the 26 bytes from pixelDataSize
			file.seekg(currentPosition); //Go back to starting position
			
			colorMapData.ImageData.resize(pixelDataSize);
			if (!file.read(reinterpret_cast<char*>(colorMapData.ImageData.data()), pixelDataSize))
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Couldn't read pixel data!");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
		}
		else
		{
			colorMapData.ImageData.resize(header.Width* header.Height* (header.BitsPerPixel / 8));
			if (!file.read(reinterpret_cast<char*>(colorMapData.ImageData.data()), header.Width * header.Height * (header.BitsPerPixel / 8)))
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Couldn't read pixel data!");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
		}

		m_width = header.Width;
		m_height = header.Height;
		m_bitsPerPixel = header.BitsPerPixel;
		switch (header.ImageType)
		{
		case 1:
		{
			m_isImageColored = true;
			if (header.BitsPerPixel > 8)
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Bad ColorMapped index size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeBGRAMap(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if(m_bitsPerPixel == 16)
				{
					m_imageFormat = ImageFormat::RGB;
					m_bitsPerPixel = 24;
				}
				else if (m_bitsPerPixel == 24)
					m_imageFormat = ImageFormat::RGB;					
				else if (m_bitsPerPixel == 32)
				{
					m_imageFormat = ImageFormat::RGBA;
					m_hasAlphaChannel = true;
				}
				break;
			}
			break;
		}

		case 9:
		{
			m_isImageColored = true;
			m_isImageCompressed = true;
			if (header.BitsPerPixel > 8)
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Bad ColorMapped RLE index size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			if (header.BitsPerPixel == 8)
			{
				m_data = DecodeRLEBGRAMap(colorMapData.ImageData, m_width, m_height, header.ColorMapDepth / 8, colorMapData.ColorMap);
				m_bitsPerPixel = header.ColorMapDepth;
				if(m_bitsPerPixel == 16)
				{
					m_imageFormat = ImageFormat::RGB;
					m_bitsPerPixel = 24;
				}
				if (m_bitsPerPixel == 24)
					m_imageFormat = ImageFormat::RGB;
				else if (m_bitsPerPixel == 32)
				{
					m_imageFormat = ImageFormat::RGBA;
					m_hasAlphaChannel = true;
				}
			}
			break;
		}

		case 3:
		{
			m_isImageGrayScale = true;
			m_imageFormat = ImageFormat::Gray_Scale;
			if (header.BitsPerPixel == 8)
				m_data = colorMapData.ImageData;
			if (header.BitsPerPixel > 8)
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Bad GrayScale pixel size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 11:
		{
			m_isImageGrayScale = true;
			m_imageFormat = ImageFormat::Gray_Scale;
			m_isImageCompressed = true;
			if (header.BitsPerPixel == 8)
				m_data = DecodeRLEGrayScale(colorMapData.ImageData, header.Width, header.Height);
			if (header.BitsPerPixel > 8)
			{
				file.close();
				TRAP_ERROR("[Image][TGA] Bad GrayScale RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 2:
		{
			m_isImageColored = true;
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_imageFormat = ImageFormat::RGB;
				m_bitsPerPixel = 24;
				m_data = ConvertBGR16ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = ConvertBGR24ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_imageFormat = ImageFormat::RGBA;
				m_hasAlphaChannel = true;
				m_data = ConvertBGRA32ToRGBA32(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				file.close();
				TRAP_ERROR("[Image][TGA] Bad TrueColor pixel size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 10:
		{
			m_isImageColored = true;
			switch (header.BitsPerPixel)
			{
			case 16:
			{
				m_imageFormat = ImageFormat::RGB;
				m_bitsPerPixel = 24;
				m_data = ConvertRLEBGR16ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 24:
			{
				m_imageFormat = ImageFormat::RGB;
				m_data = ConvertRLEBGR24ToRGB24(colorMapData.ImageData, m_width, m_height);
				break;
			}

			case 32:
			{
				m_imageFormat = ImageFormat::RGBA;
				m_hasAlphaChannel = true;
				m_data = ConvertRLEBGRA32ToRGBA(colorMapData.ImageData, m_width, m_height);
				break;
			}

			default:
				file.close();
				TRAP_ERROR("[Image][TGA] Bad TrueColor RLE pixel size: ", header.BitsPerPixel, " bits/pixel");
				TRAP_WARN("[Image][TGA] Using Default Image!");
				return;
			}
			break;
		}

		case 0: //Shouldn't be reached because of the above check!
			file.close();
			TRAP_ERROR("[Image][TGA] Image doesn't contain pixel data!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;

		default:
			file.close();
			TRAP_ERROR("[Image][TGA] Unknown or invalid Image Type!");
			TRAP_WARN("[Image][TGA] Using Default Image!");
			return;
		}

		if (needXFlip)
			m_data = FlipX(m_width, m_height, m_imageFormat, m_data.data());
		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_imageFormat, m_data.data());

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::TGAImage::GetPixelData()
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::TGAImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageCompressed() const
{
	return m_isImageCompressed;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::TGAImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::TGAImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::TGAImage::GetFormat() const
{
	return m_imageFormat;
}