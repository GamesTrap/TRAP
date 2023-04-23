#include "TRAPPCH.h"
#include "BMPImage.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"
#include "Maths/Math.h"
#include "Utils/Memory.h"
#include "Utils/Utils.h"

TRAP::INTERNAL::BMPImage::BMPImage(std::filesystem::path filepath)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	m_filepath = std::move(filepath);

	TP_DEBUG(Log::ImageBMPPrefix, "Loading image: \"", m_filepath.u8string(), "\"");

	if (!FileSystem::Exists(m_filepath))
		return;

	std::ifstream file(m_filepath, std::ios::binary);
	if (!file.is_open())
	{
		TP_ERROR(Log::ImageBMPPrefix, "Couldn't open file path: ", m_filepath.u8string(), "!");
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	Header header{};
	file.read(reinterpret_cast<char*>(&header.MagicNumber), sizeof(uint16_t));
	file.read(reinterpret_cast<char*>(&header.Size), sizeof(uint32_t));
	file.ignore(4);
	file.read(reinterpret_cast<char*>(&header.DataOffset), sizeof(uint32_t));

	//File uses little-endian
	//Convert to machines endian
	const bool needSwap = Utils::GetEndian() != Utils::Endian::Little;
	if (needSwap)
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
		TP_WARN(Log::ImageBMPPrefix, "Using default image!");
		return;
	}

	std::array<uint32_t, 4> masks{};
	if(infoHeader.Compression == 3) //BitFields
	{
		if (infoHeader.Size == 40)
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Only BMPV5 images with bit fields are supported!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		file.read(reinterpret_cast<char*>(&std::get<0>(masks)), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&std::get<1>(masks)), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&std::get<2>(masks)), sizeof(uint32_t));
		file.read(reinterpret_cast<char*>(&std::get<3>(masks)), sizeof(uint32_t));

		if (needSwap)
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
		m_height = Math::Abs(infoHeader.Height);
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
		m_height = infoHeader.Height;
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

	std::vector<uint8_t> colorTable{};
	if (m_bitsPerPixel <= 8 && (infoHeader.CLRUsed != 0u))
	{
		colorTable.resize(NumericCast<std::size_t>(4u) * infoHeader.CLRUsed);
		if(!file.read(reinterpret_cast<char*>(colorTable.data()), 4 * NumericCast<std::streamsize>(infoHeader.CLRUsed)))
		{
			file.close();
			TP_ERROR(Log::ImageBMPPrefix, "Couldn't load color map data!");
			TP_WARN(Log::ImageBMPPrefix, "Using default image!");
			return;
		}

		//Check if alpha is used
		bool alphaUsed = false;
		for (uint32_t i = 3; i < colorTable.size(); i += 4)
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
			for (uint32_t i = 3; i < colorTable.size(); i += 4)
				colorTable[i] = 255;
		}
	}

	//Load Pixel Data(BGRA) into vector
	file.seekg(header.DataOffset, std::ios::beg);
	std::vector<uint8_t> imageData{};
	if ((m_bitsPerPixel != 32 && infoHeader.Compression == 0) &&
	    4 - (((m_bitsPerPixel / 8) * m_width) % 4) != 4) //Padding
	{
		imageData.resize(NumericCast<std::size_t>(m_width) * m_height * (m_bitsPerPixel / 8));
		const uint32_t padding = 4 - (((m_bitsPerPixel / 8) * m_width) % 4);
		uint32_t offset = 0;
		for (uint32_t j = 0; j < m_height; j++)
		{
			if(!file.read(reinterpret_cast<char*>(imageData.data()) + offset,
			              NumericCast<std::streamsize>(m_width) * (m_bitsPerPixel / 8)))
			{
				file.close();
				TP_ERROR(Log::ImageBMPPrefix, "Couldn't load pixel data!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
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
			imageData.resize(NumericCast<std::size_t>(m_width) * m_height * (m_bitsPerPixel / 8));
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
			for (uint32_t i = 3; i < imageData.size(); i += 4)
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
				for (uint32_t i = 3; i < imageData.size(); i += 4)
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
			m_data.resize(NumericCast<std::size_t>(m_width) * m_height);

			//Decode Single Channel RLE 8
			DecodeRLE8(imageData, nullptr);
		}
		else
		{
			//Compressed Palette
			m_colorFormat = ColorFormat::RGBA;
			m_bitsPerPixel = 32;
			m_data.resize(NumericCast<std::size_t>(m_width) * m_height * 4);

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

		std::vector<uint8_t> data{};
		data.resize(m_width * m_height * m_bitsPerPixel / 8);
		if(m_bitsPerPixel == 32)
		{
			if(!ValidateBitFields(bitFields, masks))
			{
				TP_ERROR(Log::ImageBMPPrefix, "Invalid bit fields!");
				TP_WARN(Log::ImageBMPPrefix, "Using default image!");
				return;
			}

			uint32_t index = 0;
			for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
			{
				const uint32_t value = NumericCast<uint32_t>(imageData[i]) +
					                   (NumericCast<uint32_t>(imageData[i + 1]) << 8u) +
					                   (NumericCast<uint32_t>(imageData[i + 2]) << 16u) +
					                   (NumericCast<uint32_t>(imageData[i + 3]) << 24u);

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

			uint32_t index = 0;
			for (uint32_t i = 0; i < m_width * m_height * m_bitsPerPixel / 8 - 1;)
			{
				const uint16_t value = NumericCast<uint16_t>(imageData[i]) +
								       NumericCast<uint16_t>(imageData[i + 1] << 8u);

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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const void* TRAP::INTERNAL::BMPImage::GetPixelData() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint64_t TRAP::INTERNAL::BMPImage::GetPixelDataSize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::BMPImage::ValidateBitFields(std::array<BitField, 4>& bitFields,
	                                                           std::array<uint32_t, 4>& masks) const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	BitField* const bf = bitFields.data();

	uint32_t totalMask = 0;
	BitField totalField{};

	for(std::size_t i = 0; i < bitFields.size(); i++)
	{
		//No overlapping masks.
		if ((totalMask & masks[i]) != 0u)
			return false;

		totalMask |= masks[i];

		if (!ParseBitfield(bf[i], masks[i]))
			return false;

		//Make sure it fits in bit size
		if (bf[i].Start + bf[i].Span > m_bitsPerPixel)
			return false;
	}

	if (totalMask == 0u)
		return false;

	//Check for contiguous-ity between fields, too.
	if (!ParseBitfield(totalField, totalMask))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::BMPImage::ParseBitfield(BitField& field, const uint32_t mask) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	uint32_t bit = 0;
	for (; bit < 32 && ((mask & BIT(bit)) == 0u); bit++);

	if(bit >= 32)
	{
		//Absent BitMasks are valid.
		field.Start = field.Span = 0;
		return true;
	}

	field.Start = bit;
	for (; bit < 32 && ((mask & BIT(bit)) != 0u); bit++);
	field.Span = bit - field.Start;

	//If there are more set bits, there was a gap, which is invalid
	return bit >= 32 || ((mask & ~(BIT(bit) - 1u)) == 0u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint8_t TRAP::INTERNAL::BMPImage::Make8Bits(uint32_t value, const uint32_t bitSpan) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	uint32_t output = 0;

	if (bitSpan == 8)
		return NumericCast<uint8_t>(value);
	if (bitSpan > 8)
		return NumericCast<uint8_t>(value >> (bitSpan - 8));

	value <<= (8 - bitSpan); //Shift it up into the most significant bits.
	while(value != 0u)
	{
		output |= value;
		value >>= bitSpan;
	}

	return NumericCast<uint8_t>(output);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint16_t x, BitField& bitField) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return x >> bitField.Start & (BIT(bitField.Span) - 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint32_t x, BitField& bitField) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Green, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return x >> bitField.Start & (BIT(bitField.Span) - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::BMPImage::DecodeRLE8(std::vector<uint8_t>& compressedImageData,
	                                      const std::vector<uint8_t>* const colorTable)
{
	ZoneNamedC(__tracy, tracy::Color::Green, TRAP_PROFILE_SYSTEMS() & ProfileSystems::ImageLoader);

	int32_t x = 0, y = 0;
	uint8_t t = 0, r = 0;

	uint32_t dataIndex = 0;

	if(colorTable != nullptr)
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
