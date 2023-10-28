#include "TRAPPCH.h"
#include "BMPImage.h"

#include "FileSystem/FileSystem.h"
#include "Maths/Math.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::BMPImage::BMPImage(std::filesystem::path filepath)
	: Image(std::move(filepath))
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	TP_DEBUG(Log::ImageBMPPrefix, "Loading image: ", m_filepath);

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageBMPPrefix, "Couldn't open file path: ", m_filepath, "!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	Header header{};
	file.read(reinterpret_cast<char*>(&header.MagicNumber), sizeof(u16));
	file.read(reinterpret_cast<char*>(&header.Size), sizeof(u32));
	file.ignore(4);
	file.read(reinterpret_cast<char*>(&header.DataOffset), sizeof(u32));

	//File uses little-endian
	//Convert to machines endian
	if constexpr (Utils::GetEndian() != Utils::Endian::Little)
	{
		Utils::Memory::SwapBytes(header.MagicNumber);
		Utils::Memory::SwapBytes(header.Size);
		Utils::Memory::SwapBytes(header.DataOffset);
	}

	if (header.MagicNumber != 0x4D42)
	{
		file.close();
		TP_ERROR(Log::ImageBMPPrefix, "Magic number ", header.MagicNumber, " is invalid!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	InfoHeader infoHeader{};
	file.read(reinterpret_cast<char*>(&infoHeader.Size), sizeof(u32));
	file.read(reinterpret_cast<char*>(&infoHeader.Width), sizeof(u32));
	file.read(reinterpret_cast<char*>(&infoHeader.Height), sizeof(i32));
	file.ignore(2);
	file.read(reinterpret_cast<char*>(&infoHeader.BitsPerPixel), sizeof(u16));
	file.read(reinterpret_cast<char*>(&infoHeader.Compression), sizeof(u32));
	file.read(reinterpret_cast<char*>(&infoHeader.SizeImage), sizeof(u32));
	file.ignore(4);
	file.ignore(4);
	file.read(reinterpret_cast<char*>(&infoHeader.CLRUsed), sizeof(u32));
	file.ignore(4);

	if constexpr (Utils::GetEndian() != Utils::Endian::Little)
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
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	std::array<u32, 4> masks{};
	if(infoHeader.Compression == 3) //BitFields
	{
		if (infoHeader.Size == 40)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Only BMPV5 images with bit fields are supported!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		file.read(reinterpret_cast<char*>(&std::get<0>(masks)), sizeof(u32));
		file.read(reinterpret_cast<char*>(&std::get<1>(masks)), sizeof(u32));
		file.read(reinterpret_cast<char*>(&std::get<2>(masks)), sizeof(u32));
		file.read(reinterpret_cast<char*>(&std::get<3>(masks)), sizeof(u32));

		if constexpr (Utils::GetEndian() != Utils::Endian::Little)
		{
			Utils::Memory::SwapBytes(std::get<0>(masks));
			Utils::Memory::SwapBytes(std::get<1>(masks));
			Utils::Memory::SwapBytes(std::get<2>(masks));
			Utils::Memory::SwapBytes(std::get<3>(masks));
		}
	}

	if (infoHeader.Width < 1)
	{
		file.close();
		TP_ERROR(Log::ImageBMPPrefix, "Width ", infoHeader.Width, " is invalid!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	bool needYFlip = false;
	if (infoHeader.Height < 0)
		m_height = NumericCast<u32>(Math::Abs(infoHeader.Height));
	else if (infoHeader.Height == 0)
	{
		file.close();
		TP_ERROR(Log::ImageBMPPrefix, "Height ", infoHeader.Height, " is invalid!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}
	else
	{
		needYFlip = true;
		m_height = NumericCast<u32>(infoHeader.Height);
	}
	m_width = infoHeader.Width;
	m_bitsPerPixel = infoHeader.BitsPerPixel;

	if(m_bitsPerPixel <= 4)
	{
		file.close();
		TP_ERROR(Log::ImageBMPPrefix, "Bits per pixel ", m_bitsPerPixel, " is unsupported!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	std::vector<u8> colorTable{};
	if (m_bitsPerPixel <= 8 && (infoHeader.CLRUsed != 0u))
	{
		colorTable.resize(NumericCast<usize>(4u) * infoHeader.CLRUsed);
		if(!file.read(reinterpret_cast<char*>(colorTable.data()), 4 * NumericCast<std::streamsize>(infoHeader.CLRUsed)))
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Couldn't load color map data!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		//Check if alpha is used
		bool alphaUsed = false;
		for (u32 i = 3; i < colorTable.size(); i += 4)
		{
			if (colorTable[i] > 0)
			{
				alphaUsed = true;
				break;
			}
		}

		//If alpha is unused set all alpha bytes to 255
		if (!alphaUsed)
		{
			for (u32 i = 3; i < colorTable.size(); i += 4)
				colorTable[i] = 255;
		}
	}

	//Load Pixel Data(BGRA) into vector
	file.seekg(header.DataOffset, std::ios::beg);
	std::vector<u8> imageData{};
	if ((m_bitsPerPixel != 32 && infoHeader.Compression == 0) &&
	    4 - (((m_bitsPerPixel / 8) * m_width) % 4) != 4) //Padding
	{
		imageData.resize(NumericCast<usize>(m_width) * m_height * (m_bitsPerPixel / 8));
		const u32 padding = 4 - (((m_bitsPerPixel / 8) * m_width) % 4);
		u32 offset = 0;
		for (u32 j = 0; j < m_height; j++)
		{
			if(!file.read(reinterpret_cast<char*>(imageData.data()) + offset,
			              NumericCast<std::streamsize>(m_width) * (m_bitsPerPixel / 8)))
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
				return;
			}

			for (u32 pad = 0; pad < padding; pad++)
				file.ignore();

			offset += m_width * (m_bitsPerPixel / 8);
		}
	}
	else //No Padding
	{
		if (infoHeader.Compression != 1)
		{
			imageData.resize(NumericCast<usize>(m_width) * m_height * (m_bitsPerPixel / 8));
			if(!file.read(reinterpret_cast<char*>(imageData.data()),
						  NumericCast<std::streamsize>(m_width) * m_height * (m_bitsPerPixel / 8)))
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
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
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
				return;
			}
		}
	}

	file.close();

	if (infoHeader.Compression == 0) //Uncompressed
	{
		if (m_bitsPerPixel < 8)
		{
			TP_ERROR(Log::ImageBMPPrefix, "Bits per pixel ", m_bitsPerPixel, " is unsupported!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		if (m_bitsPerPixel == 8 && (infoHeader.CLRUsed != 0u)) //Color Table
		{
			m_colorFormat = ColorFormat::RGBA;
			m_bitsPerPixel = 32;

			m_data = DecodeBGRAMap(imageData, m_width, m_height, 4, colorTable);
		}
		else if(m_bitsPerPixel == 8 && (infoHeader.CLRUsed == 0u)) //Grayscale
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
			for (u32 i = 3; i < imageData.size(); i += 4)
			{
				if (imageData[i] > 0)
				{
					alphaUsed = true;
					break;
				}
			}

			//If alpha is unused set all alpha bytes to 255
			if (!alphaUsed)
			{
				for (u32 i = 3; i < imageData.size(); i += 4)
					imageData[i] = 255;
			}

			m_data = ConvertBGRA32ToRGBA32(imageData, m_width, m_height);
		}
	}
	else if (infoHeader.Compression == 1) //Microsoft RLE 8
	{
		if(infoHeader.CLRUsed == 0u)
		{
			//Compressed Grayscale
			m_colorFormat = ColorFormat::GrayScale;
			m_bitsPerPixel = 8;
			m_data.resize(NumericCast<usize>(m_width) * m_height);

			//Decode Single Channel RLE 8
			DecodeRLE8(imageData, nullptr);
		}
		else
		{
			//Compressed Palette
			m_colorFormat = ColorFormat::RGBA;
			m_bitsPerPixel = 32;
			m_data.resize(NumericCast<usize>(m_width) * m_height * 4);

			//Decode Multi Channel RLE 8
			DecodeRLE8(imageData, &colorTable);
		}
	}
	else if (infoHeader.Compression == 2) //Microsoft RLE 4
	{
		TP_ERROR(Log::ImageBMPPrefix, "RLE 4 is unsupported!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}
	else if (infoHeader.Compression == 3) //BitFields
	{
		m_colorFormat = ColorFormat::RGBA;

		std::array<BitField, 4> bitFields{};

		std::vector<u8> data{};
		data.resize(m_width * m_height * m_bitsPerPixel / 8);
		if(m_bitsPerPixel == 32)
		{
			if(!ValidateBitFields(bitFields, masks))
			{
				TP_ERROR(Log::ImageBMPPrefix, "Invalid bit fields!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
				return;
			}

			u32 index = 0;
			for (u32 i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
			{
				const u32 value = NumericCast<u32>(imageData[i]) +
					                   (NumericCast<u32>(imageData[i + 1]) << 8u) +
					                   (NumericCast<u32>(imageData[i + 2]) << 16u) +
					                   (NumericCast<u32>(imageData[i + 3]) << 24u);

				data[index++] = Make8Bits(ApplyBitField(value, std::get<0>(bitFields)), std::get<0>(bitFields).Span);
				data[index++] = Make8Bits(ApplyBitField(value, std::get<1>(bitFields)), std::get<1>(bitFields).Span);
				data[index++] = Make8Bits(ApplyBitField(value, std::get<2>(bitFields)), std::get<2>(bitFields).Span);
				if (GetBytesPerPixel() == 4)
				{
					if (std::get<3>(bitFields).Span != 0u)
						data[index++] = Make8Bits(ApplyBitField(value, std::get<3>(bitFields)), std::get<3>(bitFields).Span);
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
				TP_ERROR(Log::ImageBMPPrefix, "Invalid bit fields!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
				return;
			}

			u32 index = 0;
			for (u32 i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
			{
				const u16 value = NumericCast<u16>(imageData[i]) +
								       NumericCast<u16>(imageData[i + 1] << 8u);

				data[index++] = Make8Bits(ApplyBitField(value, std::get<0>(bitFields)), std::get<0>(bitFields).Span);
				data[index++] = Make8Bits(ApplyBitField(value, std::get<1>(bitFields)), std::get<1>(bitFields).Span);
				data[index++] = Make8Bits(ApplyBitField(value, std::get<2>(bitFields)), std::get<2>(bitFields).Span);
				if(GetBytesPerPixel() == 4)
				{
					if (std::get<3>(bitFields).Span != 0u)
						data[index++] = Make8Bits(ApplyBitField(value, std::get<3>(bitFields)), std::get<3>(bitFields).Span);
					else
						data[index++] = 255;
				}

				i += 2;
			}

			m_bitsPerPixel = 32;
		}
		else
		{
			TP_ERROR(Log::ImageBMPPrefix, "Invalid bits per pixel ", m_bitsPerPixel, " for bit field image!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		m_data = std::move(data);
	}

	if (needYFlip)
		m_data = FlipY(m_width, m_height, m_colorFormat, m_data.data());
}
