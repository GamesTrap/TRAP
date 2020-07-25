#include "TRAPPCH.h"
#include "BMPImage.h"

#include "Utils/String/String.h"
#include "VFS/VFS.h"
#include "VFS/FileSystem.h"
#include "Maths/Math.h"
#include "Application.h"
#include "Utils/ByteSwap.h"

TRAP::INTERNAL::BMPImage::BMPImage(std::string filepath)
{
	TP_PROFILE_FUNCTION();
	
	m_filepath = std::move(filepath);
	
	TP_DEBUG(Log::ImageBMPPrefix, "Loading Image: \"", Utils::String::SplitString(m_filepath, '/').back(), "\"");

	std::filesystem::path physicalPath;
	if (!VFS::SilentResolveReadPhysicalPath(m_filepath, physicalPath))
	{
		TP_ERROR(Log::ImageBMPPrefix, "Couldn't resolve FilePath: ", m_filepath, "!");
		TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
		return;
	}

	if (FileSystem::PhysicalFileOrFolderExists(physicalPath))
	{
		std::ifstream file(physicalPath, std::ios::binary);
		if (!file.is_open())
		{
			TP_ERROR(Log::ImageBMPPrefix, "Couldn't open FilePath: ", m_filepath, "!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
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

		//File uses little-endian
		//Convert to machines endian
		bool needSwap = static_cast<bool>(Application::GetEndian() != Application::Endian::Little);
		if (needSwap)
		{
			Utils::Memory::SwapBytes(header.MagicNumber);
			Utils::Memory::SwapBytes(header.Size);
			Utils::Memory::SwapBytes(header.DataOffset);
		}

		if (header.MagicNumber != 0x4D42)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Magic number is invalid!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}

		struct InfoHeader
		{
			uint32_t Size = 0; //Size of this header in bytes
			uint32_t Width = 0;
			int32_t Height = 0;
			//uint16_t Planes = 0; //Always 1
			uint16_t BitsPerPixel = 0; //1, 4, 8, 16, 24, 32
			uint32_t Compression = 0; //0 = Uncompressed | 1 = RLE 8BPP | 2 = RLE 4BPP | 3 = BitFields
			uint32_t SizeImage = 0; //Size of the image in bytes
			//int32_t XPixelsPerMeter = 0;
			//int32_t YPixelsPerMeter = 0;
			uint32_t CLRUsed = 0; //Amount of colors in palette
			//uint32_t CLRImportant = 0; //Amount of important colors in palette
		} infoHeader;
		file.read(reinterpret_cast<char*>(&infoHeader.Size), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Width), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Height), sizeof(int32_t));
		file.ignore(2);
		file.read(reinterpret_cast<char*>(&infoHeader.BitsPerPixel), sizeof(uint16_t));
		file.read(reinterpret_cast<char*>(&infoHeader.Compression), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&infoHeader.SizeImage), sizeof(uint32_t));
		file.ignore(4);
		file.ignore(4);
		file.read(reinterpret_cast<char*>(&infoHeader.CLRUsed), sizeof(uint32_t));
		file.ignore(4);

		if (needSwap)
		{
			Utils::Memory::SwapBytes(infoHeader.Size);
			Utils::Memory::SwapBytes(infoHeader.Width);
			Utils::Memory::SwapBytes(infoHeader.Height);
			Utils::Memory::SwapBytes(infoHeader.BitsPerPixel);
			Utils::Memory::SwapBytes(infoHeader.Compression);
			Utils::Memory::SwapBytes(infoHeader.SizeImage);
			Utils::Memory::SwapBytes(infoHeader.CLRUsed);
		}

		if(infoHeader.Size == 12)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "OS/2 1.x BMPs are unsupported!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}
		
		std::array<uint32_t, 4> masks{};
		if(infoHeader.Compression == 3) //BitFields
		{
			if (infoHeader.Size == 40)
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Only BMPV5 Images with BitFields are supported!");
				TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
				return;
			}
			
			file.read(reinterpret_cast<char*>(&masks[0]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[1]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[2]), sizeof(uint32_t));
			file.read(reinterpret_cast<char*>(&masks[3]), sizeof(uint32_t));

			if (needSwap)
			{
				Utils::Memory::SwapBytes(masks[0]);
				Utils::Memory::SwapBytes(masks[1]);
				Utils::Memory::SwapBytes(masks[2]);
				Utils::Memory::SwapBytes(masks[3]);
			}
		}		

		if (infoHeader.Width < 1)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Width is invalid!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}

		bool needYFlip = false;
		if (infoHeader.Height < 0)
			m_height = Math::Abs(infoHeader.Height);
		else if (infoHeader.Height == 0)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Height is invalid!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}
		else
		{
			needYFlip = true;
			m_height = infoHeader.Height;
		}
		m_width = infoHeader.Width;
		m_bitsPerPixel = infoHeader.BitsPerPixel;

		if(m_bitsPerPixel <= 4)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "BitsPerPixel ", m_bitsPerPixel, " is unsupported!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}

		std::vector<uint8_t> colorTable{};
		if (m_bitsPerPixel <= 8 && infoHeader.CLRUsed)
		{
			colorTable.resize(4 * infoHeader.CLRUsed);
			if(!file.read(reinterpret_cast<char*>(colorTable.data()), 4 * infoHeader.CLRUsed))
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Couldn't load Color Map data!");
				TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
				return;
			}

			//Check if alpha is used
			bool alphaUsed = false;
			for (uint32_t i = 3; i < colorTable.size(); i += 4)
				if (colorTable[i] > 0)
				{
					alphaUsed = true;
					break;
				}

			//If alpha is unused set all alpha bytes to 255
			if (!alphaUsed)
				for (uint32_t i = 3; i < colorTable.size(); i += 4)
					colorTable[i] = 255;
		}

		//Load Pixel Data(BGRA) into vector
		file.seekg(header.DataOffset, std::ios::beg);
		std::vector<uint8_t> imageData{};		
		if ((m_bitsPerPixel != 32 && infoHeader.Compression == 0) && 4 - (((m_bitsPerPixel / 8) * m_width) % 4) != 4) //Padding
		{
			imageData.resize(m_width * m_height * (m_bitsPerPixel / 8));
			uint32_t padding = 4 - (((m_bitsPerPixel / 8) * m_width) % 4);
			uint32_t offset = 0;
			for (uint32_t j = 0; j < m_height; j++)
			{
				if(!file.read(reinterpret_cast<char*>(imageData.data()) + offset, m_width * (m_bitsPerPixel / 8)))
				{
					file.close();
					TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
					return;
				}

				for (uint32_t pad = 0; pad < padding; pad++)
					file.ignore();

				offset += m_width * (m_bitsPerPixel / 8);
			}
		}
		else //No Padding
		{
			if (infoHeader.Compression != 1)
			{
				imageData.resize(m_width* m_height* (m_bitsPerPixel / 8));
				if(!file.read(reinterpret_cast<char*>(imageData.data()), m_width * m_height * (m_bitsPerPixel / 8)))
				{
					file.close();
					TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
					return;
				}
			}
			else
			{
				imageData.resize(infoHeader.SizeImage);
				if (!file.read(reinterpret_cast<char*>(imageData.data()), infoHeader.SizeImage))
				{
					file.close();
					TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
					TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
					return;
				}
			}
		}

		if (infoHeader.Compression == 0) //Uncompressed
		{
			if (m_bitsPerPixel < 8)
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "BitsPerPixel ", m_bitsPerPixel, " is unsupported!");
				TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
				return;
			}

			if (m_bitsPerPixel == 8 && infoHeader.CLRUsed) //Color Table
			{
				m_colorFormat = ColorFormat::RGBA;
				m_bitsPerPixel = 32;
				
				m_data = DecodeBGRAMap(imageData, m_width, m_height, 4, colorTable);
			}
			else if(m_bitsPerPixel == 8 && !infoHeader.CLRUsed) //Grayscale
			{
				m_colorFormat = ColorFormat::GrayScale;
				m_bitsPerPixel = 8;

				m_data = imageData;
			}
			else if (m_bitsPerPixel == 16) //RGB
			{				
				m_colorFormat = ColorFormat::RGB;
				m_bitsPerPixel = 24;
				m_data = ConvertBGR16ToRGB24(imageData, m_width, m_height);
			}
			else if (m_bitsPerPixel == 24) //RGB
			{				
				m_colorFormat = ColorFormat::RGB;
				m_data = ConvertBGR24ToRGB24(imageData, m_width, m_height);
			}
			else if (m_bitsPerPixel == 32) //RGBA
			{
				m_colorFormat = ColorFormat::RGBA;

				//Check if alpha is used
				bool alphaUsed = false;
				for (uint32_t i = 3; i < imageData.size(); i += 4)
					if (imageData[i] > 0)
					{
						alphaUsed = true;
						break;
					}

				//If alpha is unused set all alpha bytes to 255
				if (!alphaUsed)
					for (uint32_t i = 3; i < imageData.size(); i += 4)
						imageData[i] = 255;

				m_data = ConvertBGRA32ToRGBA32(imageData, m_width, m_height);
			}
		}
		else if (infoHeader.Compression == 1) //Microsoft RLE 8
		{
			if(!infoHeader.CLRUsed)
			{
				//Compressed Grayscale
				m_colorFormat = ColorFormat::GrayScale;
				m_bitsPerPixel = 8;
				m_data.resize(m_width * m_height);
				
				//Decode Single Channel RLE 8
				DecodeRLE8(imageData, nullptr);
			}
			else
			{
				//Compressed Palette
				m_colorFormat = ColorFormat::RGBA;
				m_bitsPerPixel = 32;
				m_data.resize(m_width* m_height * 4);

				//Decode Multi Channel RLE 8
				DecodeRLE8(imageData, &colorTable);
			}
		}
		else if (infoHeader.Compression == 2) //Microsoft RLE 4
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "RLE 4 is unsupported!");
			TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
			return;
		}
		else if (infoHeader.Compression == 3) //BitFields
		{
			m_colorFormat = ColorFormat::RGBA;

			std::array<BitField, 4> bitFields{};

			std::vector<uint8_t> data{};
			data.resize(m_width * m_height * m_bitsPerPixel / 8);
			if(m_bitsPerPixel == 32)
			{
				if(!ValidateBitFields(bitFields, masks))
				{
					file.close();
					TP_ERROR(Log::ImageBMPPrefix, "Invalid BitFields!");
					TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
					return;
				}

				uint32_t index = 0;
				for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
				{
					uint32_t value = static_cast<uint32_t>(imageData[i]) +
						(static_cast<uint32_t>(imageData[i + 1]) << 8) +
						(static_cast<uint32_t>(imageData[i + 2]) << 16) +
						(static_cast<uint32_t>(imageData[i + 3]) << 24);

					data[index++] = Make8Bits(ApplyBitField(value, bitFields[0]), bitFields[0].Span);
					data[index++] = Make8Bits(ApplyBitField(value, bitFields[1]), bitFields[1].Span);
					data[index++] = Make8Bits(ApplyBitField(value, bitFields[2]), bitFields[2].Span);
					if (GetBytesPerPixel() == 4)
					{
						if (bitFields[3].Span)
							data[index++] = Make8Bits(ApplyBitField(value, bitFields[3]), bitFields[3].Span);
						else
							data[index++] = 255;
					}

					i += 4;
				}
			}
			else if(m_bitsPerPixel == 16)
			{
				if (!ValidateBitFields(bitFields, masks))
				{
					file.close();
					TP_ERROR(Log::ImageBMPPrefix, "Invalid BitFields!");
					TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
					return;
				}

				uint32_t index = 0;
				for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
				{
					uint16_t value = static_cast<uint16_t>(imageData[i]) +
							         static_cast<uint16_t>(imageData[i + 1] << 8);

					data[index++] = Make8Bits(ApplyBitField(value, bitFields[0]), bitFields[0].Span);
					data[index++] = Make8Bits(ApplyBitField(value, bitFields[1]), bitFields[1].Span);
					data[index++] = Make8Bits(ApplyBitField(value, bitFields[2]), bitFields[2].Span);
					if(GetBytesPerPixel() == 4)
					{
						if (bitFields[3].Span)
							data[index++] = Make8Bits(ApplyBitField(value, bitFields[3]), bitFields[3].Span);
						else
							data[index++] = 255;
					}

					i += 2;
				}
				
				m_bitsPerPixel = 32;
			}
			else
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Invalid BitsPerPixel for BitField Image!");
				TP_WARN(Log::ImageBMPPrefix, "Using Default Image!");
				return;
			}

			m_data = std::move(data);
		}

		if (needYFlip)
			m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());

		file.close();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

const void* TRAP::INTERNAL::BMPImage::GetPixelData() const
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::GetPixelDataSize() const
{
	return static_cast<uint32_t>(m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::BMPImage::ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<uint32_t, 4>& masks) const
{
	BitField* bf = bitFields.data();

	uint32_t totalMask = 0;
	BitField totalField{};

	for(int32_t i = 0; i < 4; i++)
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
	return static_cast<uint32_t>(x >> bitField.Start & ((uint32_t(1) << bitField.Span) - 1));
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint32_t x, BitField& bitField)
{
	return static_cast<uint32_t>(x >> bitField.Start & ((uint32_t(1) << bitField.Span) - 1));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::BMPImage::DecodeRLE8(std::vector<uint8_t>& compressedImageData, std::vector<uint8_t>* colorTable)
{
	int32_t x = 0, y = 0;
	uint8_t t, r;

	uint32_t dataIndex = 0;
	
	if(colorTable)
	{
		//Compressed RGBA
		while (true)
		{
			uint8_t color = compressedImageData[dataIndex++];

			if (color != 0)
			{
				r = compressedImageData[dataIndex++] * 4;

				for (t = 0; t < color; t++)
				{
					m_data[((y * m_width + x) * 4) + 0] = (*colorTable)[r + 2];
					m_data[((y * m_width + x) * 4) + 1] = (*colorTable)[r + 1];
					m_data[((y * m_width + x) * 4) + 2] = (*colorTable)[r + 0];
					m_data[((y * m_width + x) * 4) + 3] = (*colorTable)[r + 3];
					x++;
				}
			}
			else
			{
				r = compressedImageData[dataIndex++];

				if (r == 0)
				{
					x = 0;
					y++;
					continue;
				}
				if (r == 1)
					return;
				if (r == 2)
				{
					x = x + compressedImageData[dataIndex++];
					y = y + compressedImageData[dataIndex++];
					continue;
				}

				for (t = 0; t < r; t++)
				{
					color = compressedImageData[dataIndex++] * 4;
					m_data[((y * m_width + x) * 4) + 0] = (*colorTable)[color + 2];
					m_data[((y * m_width + x) * 4) + 1] = (*colorTable)[color + 1];
					m_data[((y * m_width + x) * 4) + 2] = (*colorTable)[color + 0];
					m_data[((y * m_width + x) * 4) + 3] = (*colorTable)[color + 3];
					x++;
				}

				color = r % 2;

				if (color != 0)
					dataIndex++;
			}
		}
	}
	else
	{
		//Compressed Grayscale
		while (true)
		{
			uint8_t color = compressedImageData[dataIndex++];

			if (color != 0)
			{
				r = compressedImageData[dataIndex++];

				for (t = 0; t < color; t++)
				{
					m_data[x + (y * m_width)] = r;
					x++;
				}
			}
			else
			{
				r = compressedImageData[dataIndex++];

				if (r == 0)
				{
					x = 0;
					y++;
					continue;
				}
				if (r == 1)
					return;
				if (r == 2)
				{
					x = x + compressedImageData[dataIndex++];
					y = y + compressedImageData[dataIndex++];
					continue;
				}

				for (t = 0; t < r; t++)
				{
					color = compressedImageData[dataIndex++];
					m_data[x + (y * m_width)] = color;
					x++;
				}

				color = r % 2;

				if (color != 0)
					dataIndex++;
			}
		}
	}
}