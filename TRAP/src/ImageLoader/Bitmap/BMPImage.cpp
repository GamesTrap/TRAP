#include "TRAPPCH.h"
#include "BMPImage.h"

#include "Utils/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Maths/Maths.h"
#include "ImageLoader/TARGA/TGAImage.h"

TRAP::INTERNAL::BMPImage::BMPImage(std::string filepath)
	: m_filepath(std::move(filepath)),
	m_bitsPerPixel(0),
	m_width(0),
	m_height(0),
	m_format(ImageFormat::NONE),
	m_isImageColored(false),
	m_isImageGrayScale(false),
	m_isImageCompressed(false),
	m_hasAlphaChannel(false)
{
	TP_DEBUG("[Image][BMP] Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::Get()->SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR("[Image][BMP] Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN("[Image][BMP] Using Default Image!");
		return;
	}

	if (FileSystem::FileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR("[Image][BMP] Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}

		struct Header
		{
			uint16_t MagicNumber = 0; //Used to check format
			uint32_t Size = 0; //File size in bytes
			uint32_t DataOffset = 0; //Offset from file start - first pixel data
		} header;
		file.read(reinterpret_cast<char*>(&header.MagicNumber), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&header.Size), sizeof(uint32_t));
		file.ignore(4);
		file.read(reinterpret_cast<char*>(&header.DataOffset), sizeof(uint32_t));

		if (header.MagicNumber != 0x4D42)
		{
			TP_ERROR("[Image][BMP] Magic number is invalid!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}

		struct InfoHeader
		{
			uint32_t Size = 0; //Size of this header in bytes
			uint32_t Width = 0;
			int32_t Height = 0;
			uint16_t Planes = 0; //Always 1
			uint16_t BitsPerPixel = 0; //1, 4, 8, 16, 24, 32
			uint32_t Compression = 0; //0 = Uncompressed | 1 = RLE 8BPP | 2 = RLE 4BPP | 3 = BitFields
			uint32_t SizeImage = 0; //Size of the image in bytes
			int32_t XPixelsPerMeter = 0;
			int32_t YPixelsPerMeter = 0;
			uint32_t CLRUsed = 0; //Amount of colors in palette
			uint32_t CLRImportant = 0; //Amount of important colors in palette
		} infoHeader;
		file.read(reinterpret_cast<char*>(&infoHeader.Size), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Width), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Height), sizeof(int32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Planes), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&infoHeader.BitsPerPixel), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Compression), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.SizeImage), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.XPixelsPerMeter), sizeof(int32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.YPixelsPerMeter), sizeof(int32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.CLRUsed), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.CLRImportant), sizeof(uint32_t));

		std::array<uint32_t, 4> masks{};
		if(infoHeader.Compression == 3) //BitFields
		{
			if (infoHeader.Size == 40)
			{
				TP_ERROR("[Image][BMP] Only BMPV5 Images with BitFields are supported!");
				TP_WARN("[Image][BMP] Using Default Image!");
				return;
			}
			
			file.read(reinterpret_cast<char*>(&masks[0]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[1]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[2]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[3]), sizeof(uint32_t));
		}		

		if (infoHeader.Width < 1)
		{
			TP_ERROR("[Image][BMP] Width is invalid!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}

		bool needYFlip = false;
		if (infoHeader.Height < 0)
			m_height = Math::Abs(infoHeader.Height);
		else if (infoHeader.Height == 0)
		{
			TP_ERROR("[Image][BMP] Height is invalid!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}
		else
		{
			needYFlip = true;
			m_height = infoHeader.Height;
		}
		m_width = infoHeader.Width;
		m_bitsPerPixel = infoHeader.BitsPerPixel;

		if (m_bitsPerPixel <= 8 && !infoHeader.CLRUsed)
		{
			TP_ERROR("[Image][BMP] No color stored in color table!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}
		if (m_bitsPerPixel == 1)
		{
			TP_ERROR("[Image][BMP] BitsPerPixel is invalid/unsupported!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}

		std::vector<uint8_t> colorTable{};
		uint8_t temp;
		if (m_bitsPerPixel <= 8)
		{
			colorTable.reserve(4 * infoHeader.CLRUsed);

			for (unsigned int i = 0; i < 4 * infoHeader.CLRUsed; i++)
			{
				file.read(reinterpret_cast<char*>(&temp), sizeof(uint8_t));
				colorTable.emplace_back(temp);
			}
		}

		//Load Pixel Data(BGRA) into vector
		file.seekg(header.DataOffset, std::ios::beg);
		std::vector<uint8_t> imageData{};
		imageData.reserve(m_width * m_height * (m_bitsPerPixel / 8));
		if ((m_bitsPerPixel != 32 && infoHeader.Compression == 0) && 4 - (((m_bitsPerPixel / 8) * m_width) % 4) != 4) //Padding
		{			
			uint32_t padding = 4 - (((m_bitsPerPixel / 8) * m_width) % 4);
			for (unsigned int j = 0; j < m_height; j++)
			{
				for (unsigned int i = 0; i < m_width; i++)
				{
					for (unsigned int k = 0; k < m_bitsPerPixel / 8; k++)
					{
						file.read(reinterpret_cast<char*>(&temp), sizeof(uint8_t));
						imageData.emplace_back(temp);
					}
				}

				for (unsigned int pad = 0; pad < padding; pad++)
					file.ignore();
			}
		}
		else //No Padding
		{
			for (unsigned int i = 0; i < static_cast<unsigned int>(m_width * m_height * (m_bitsPerPixel / 8)); i++)
			{
				file.read(reinterpret_cast<char*>(&temp), sizeof(uint8_t));
				imageData.emplace_back(temp);
			}
		}

		if (infoHeader.Compression == 0) //Uncompressed
		{
			if (m_bitsPerPixel < 8)
			{
				TP_ERROR("[Image][BMP] BitsPerPixel ", m_bitsPerPixel, " is unsupported!");
				TP_WARN("[Image][BMP] Using Default Image!");
				return;
			}

			if (m_bitsPerPixel == 8) //Color Table
			{
				m_format = ImageFormat::RGBA;
				m_isImageColored = true;
				m_bitsPerPixel = 32;
				m_hasAlphaChannel = true;
				
				//Check if alpha is used
				bool alphaUsed = false;
				for (unsigned int i = 3; i < imageData.size() - 1; i += 4)
					if (imageData[i] > 0)
					{
						alphaUsed = true;
						break;
					}

				//If alpha is unused set all alpha bytes to 255
				if (!alphaUsed)
					for (unsigned int i = 3; i < imageData.size() - 1; i += 4)
						imageData[i] = 255;
				
				m_data = DecodeBGRAMap(imageData, m_width, m_height, 4, colorTable);
			}
			else if (m_bitsPerPixel == 16) //RGB
			{				
				m_format = ImageFormat::RGB;
				m_isImageColored = true;
				m_bitsPerPixel = 24;
				m_data = ConvertBGR16ToRGB24(imageData, m_width, m_height);
			}
			else if (m_bitsPerPixel == 24) //RGB
			{				
				m_format = ImageFormat::RGB;
				m_isImageColored = true;
				m_data = ConvertBGR24ToRGB24(imageData, m_width, m_height);
			}
			else if (m_bitsPerPixel == 32) //RGBA
			{
				m_format = ImageFormat::RGBA;
				m_isImageColored = true;
				m_hasAlphaChannel = true;

				//Check if alpha is used
				bool alphaUsed = false;
				for (unsigned int i = 3; i < imageData.size(); i += 4)
					if (imageData[i] > 0)
					{
						alphaUsed = true;
						break;
					}

				//If alpha is unused set all alpha bytes to 255
				if (!alphaUsed)
					for (unsigned int i = 3; i < imageData.size(); i += 4)
						imageData[i] = 255;

				m_data = ConvertBGRA32ToRGBA32(imageData, m_width, m_height);
			}
		}
		else if (infoHeader.Compression == 1) //RLE 8
		{
			m_isImageCompressed = true;
			m_hasAlphaChannel = true;
			m_isImageColored = true;
			m_bitsPerPixel = 32;
			m_format = ImageFormat::RGBA;

			//m_data = DecodeRLEBGRAMap(imageData, m_width, m_height, 4, colorTable); //TODO doesnt work with BMPs RLE
			TP_ERROR("[Image][BMP] RLE 8 is WIP!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}
		else if (infoHeader.Compression == 2) //RLE 4
		{
			TP_ERROR("[Image][BMP] RLE 4 is unsupported!");
			TP_WARN("[Image][BMP] Using Default Image!");
			return;
		}
		else if (infoHeader.Compression == 3) //BitFields
		{
			m_isImageCompressed = true;
			m_isImageColored = true;
			m_hasAlphaChannel = true;
			m_format = ImageFormat::RGBA;

			std::array<BitField, 4> bitFields{};

			std::vector<uint8_t> data{};
			data.reserve(m_width* m_height* m_bitsPerPixel / 8);
			if(m_bitsPerPixel == 32)
			{
				if(!ValidateBitFields(bitFields, masks))
				{
					TP_ERROR("[Image][BMP] Invalid BitFields!");
					TP_WARN("[Image][BMP] Using Default Image!");
					return;
				}
				
				for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
				{
					uint32_t value = static_cast<uint32_t>(imageData[i]) +
						(static_cast<uint32_t>(imageData[i + 1]) << 8) +
						(static_cast<uint32_t>(imageData[i + 2]) << 16) +
						(static_cast<uint32_t>(imageData[i + 3]) << 24);

					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[0]), bitFields[0].Span));
					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[1]), bitFields[1].Span));
					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[2]), bitFields[2].Span));
					if (GetBytesPerPixel() == 4)
					{
						if (bitFields[3].Span)
							data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[3]), bitFields[3].Span));
						else
							data.emplace_back(255);
					}

					i += 4;
				}
			}
			else if(m_bitsPerPixel == 16)
			{
				if (!ValidateBitFields(bitFields, masks))
				{
					TP_ERROR("[Image][BMP] Invalid BitFields!");
					TP_WARN("[Image][BMP] Using Default Image!");
					return;
				}

				for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
				{
					uint16_t value = static_cast<uint16_t>(imageData[i]) +
							         static_cast<uint16_t>(imageData[i + 1] << 8);

					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[0]), bitFields[0].Span));
					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[1]), bitFields[1].Span));
					data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[2]), bitFields[2].Span));
					if(GetBytesPerPixel() == 4)
					{
						if (bitFields[3].Span)
							data.emplace_back(Make8Bits(ApplyBitField(value, bitFields[3]), bitFields[3].Span));
						else
							data.emplace_back(255);
					}

					i += 2;
				}
				
				m_data = data;
				m_bitsPerPixel = 32;
			}
			else
			{
				TP_ERROR("[Image][BMP] Invalid BitsPerPixel for BitField Image!");
				TP_WARN("[Image][BMP] Using Default Image!");
				return;
			}

			m_data = data;
		}

		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_format, m_data.data());

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void* TRAP::INTERNAL::BMPImage::GetPixelData()
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetBitsPerPixel() const
{
	return m_bitsPerPixel;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetBytesPerPixel() const
{
	return m_bitsPerPixel / 8;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::HasAlphaChannel() const
{
	return m_hasAlphaChannel;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::IsImageCompressed() const
{
	return m_isImageCompressed;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::IsImageGrayScale() const
{
	return m_isImageGrayScale;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::IsImageColored() const
{
	return m_isImageColored;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::IsHDR() const
{
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::INTERNAL::BMPImage::GetFilePath() const
{
	return m_filepath;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::ImageFormat TRAP::INTERNAL::BMPImage::GetFormat() const
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<uint32_t, 4>& masks) const
{
	BitField* bf = bitFields.data();

	uint32_t totalMask = 0;
	BitField totalField{};

	for(int i = 0; i < 4; i++)
	{
		//No overlapping masks.
		if (totalMask & masks[i])
			return false;
		totalMask |= masks[i];

		if (!ParseBitfield(bf[i], masks[i]))
			return false;

		//Make sure it fits in bit size
		if (bf[i].Start + bf[i].Span > m_bitsPerPixel)
			return false;
	}

	if (!totalMask)
		return false;

	//Check for contiguous-ity between fields, too.
	if (!ParseBitfield(totalField, totalMask))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::ParseBitfield(BitField& field, const uint32_t mask)
{
	uint32_t bit;
	for (bit = 0; bit < 32 && !(mask & (uint32_t(1) << bit)); bit++)
		;

	if(bit >= 32)
	{
		//Absent BitMasks are valid.
		field.Start = field.Span = 0;
		return true;
	}

	field.Start = bit;
	for (; bit < 32 && (mask & (uint32_t(1) << bit)); bit++)
		;
	field.Span = bit - field.Start;

	//If there are more set bits, there was a gap, which is invalid
	if (bit < 32 && (mask & ~((uint32_t(1) << bit) - 1)))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

uint8_t TRAP::INTERNAL::BMPImage::Make8Bits(uint32_t value, const uint32_t bitSpan)
{
	uint32_t output = 0;

	if (bitSpan == 8)
		return static_cast<uint8_t>(value);
	if (bitSpan > 8)
		return static_cast<uint8_t>(value >> (bitSpan - 8));

	value <<= (8 - bitSpan); //Shift it up intro the most significant bits.
	while(value)
	{
		output |= value;
		value >>= bitSpan;
	}

	return static_cast<uint8_t>(output);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint16_t x, BitField& bitField)
{
	return static_cast<uint32_t>(x >> bitField.Start & (uint32_t(1) << bitField.Span) - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint32_t x, BitField& bitField)
{
	return static_cast<uint32_t>(x >> bitField.Start & (uint32_t(1) << bitField.Span) - 1);
}