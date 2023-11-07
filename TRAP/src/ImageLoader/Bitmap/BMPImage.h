#ifndef TRAP_BMPIMAGE_H
#define TRAP_BMPIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class BMPImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit BMPImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		BMPImage(const BMPImage&) noexcept = default;
		/// @brief Copy assignment operator.
		BMPImage& operator=(const BMPImage&) noexcept = default;
		/// @brief Move constructor.
		BMPImage(BMPImage&&) noexcept = default;
		/// @brief Move assignment operator.
		BMPImage& operator=(BMPImage&&) noexcept = default;
		/// @brief Destructor.
		~BMPImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		struct BitField
		{
			u32 Start;
			u32 Span;
		};

		struct Header
		{
			u16 MagicNumber = 0; //Used to check format
			u32 Size = 0; //File size in bytes
			u32 DataOffset = 0; //Offset from file start - first pixel data
		};

		struct InfoHeader
		{
			u32 Size = 0; //Size of this header in bytes
			u32 Width = 0;
			i32 Height = 0;
			//u16 Planes = 0; //Always 1
			u16 BitsPerPixel = 0; //1, 4, 8, 16, 24, 32
			u32 Compression = 0; //0 = Uncompressed | 1 = RLE 8BPP | 2 = RLE 4BPP | 3 = BitFields
			u32 SizeImage = 0; //Size of the image in bytes
			//i32 XPixelsPerMeter = 0;
			//i32 YPixelsPerMeter = 0;
			u32 CLRUsed = 0; //Amount of colors in palette
			//u32 CLRImportant = 0; //Amount of important colors in palette
		};

		/// @brief Validate bit fields via the given masks.
		/// @param bitFields Bit fields to validate.
		/// @param masks Mask to validate the bit fields against.
		/// @return True if the bit fields are valid, false otherwise.
		[[nodiscard]] constexpr bool ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<u32, 4>& masks) const noexcept;
		/// @brief Parse a bit field.
		/// @param field Bit field to parse.
		/// @param mask Mask.
		/// @return True if the bit field was parsed successfully, false otherwise.
		[[nodiscard]] static constexpr bool ParseBitfield(BitField& field, u32 mask) noexcept;
		/// @brief Convert given value to 8 bits.
		/// @param value Value to convert.
		/// @param bitSpan Bit span.
		/// @return Value as 8 bits.
		[[nodiscard]] static constexpr u8 Make8Bits(u32 value, u32 bitSpan) noexcept;
		/// @brief Applies a bit field on the given value.
		/// @param x Value to apply bit field to.
		/// @param bitField Bit field to apply.
		/// @return Value with bit field applied.
		[[nodiscard]] static constexpr u32 ApplyBitField(u32 x, BitField& bitField) noexcept;
		/// @brief Applies a bit field on the given value.
		/// @param x Value to apply bit field to.
		/// @param bitField Bit field to apply.
		/// @return Value with bit field applied.
		[[nodiscard]] static constexpr u32 ApplyBitField(u16 x, BitField& bitField) noexcept;
		/// @brief Decode run length encoded 8-bit BMP data.
		/// @param compressedImageData Compressed image data.
		/// @param colorTable Color table.
		constexpr void DecodeRLE8(std::vector<u8>& compressedImageData, const std::vector<u8>* colorTable);

		std::vector<u8> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::BMPImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::BMPImage::GetPixelDataSize() const noexcept
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::INTERNAL::BMPImage::ValidateBitFields(std::array<BitField, 4>& bitFields,
	                                                                     std::array<u32, 4>& masks) const noexcept
{
	BitField* const bf = bitFields.data();

	u32 totalMask = 0;
	BitField totalField{};

	for(usize i = 0; i < bitFields.size(); i++)
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

[[nodiscard]] constexpr bool TRAP::INTERNAL::BMPImage::ParseBitfield(BitField& field, const u32 mask) noexcept
{
	u32 bit = 0;
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

[[nodiscard]] constexpr u8 TRAP::INTERNAL::BMPImage::Make8Bits(u32 value, const u32 bitSpan) noexcept
{
	u32 output = 0;

	if (bitSpan == 8)
		return NumericCast<u8>(value);
	if (bitSpan > 8)
		return NumericCast<u8>(value >> (bitSpan - 8));

	value <<= (8 - bitSpan); //Shift it up into the most significant bits.
	while(value != 0u)
	{
		output |= value;
		value >>= bitSpan;
	}

	return NumericCast<u8>(output);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::INTERNAL::BMPImage::ApplyBitField(const u16 x, BitField& bitField) noexcept
{
	return x >> bitField.Start & (BIT(bitField.Span) - 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::INTERNAL::BMPImage::ApplyBitField(const u32 x, BitField& bitField) noexcept
{
	return x >> bitField.Start & (BIT(bitField.Span) - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::BMPImage::DecodeRLE8(std::vector<u8>& compressedImageData,
	                                                const std::vector<u8>* const colorTable)
{
	u32 x = 0, y = 0;
	u8 t = 0, r = 0;

	u32 dataIndex = 0;

	if(colorTable != nullptr)
	{
		//Compressed RGBA
		while (true)
		{
			u8 color = compressedImageData[dataIndex++];

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
					x += compressedImageData[dataIndex++];
					y += compressedImageData[dataIndex++];
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
		u8 color = compressedImageData[dataIndex++];

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
				x += compressedImageData[dataIndex++];
				y += compressedImageData[dataIndex++];
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

#endif /*TRAP_BMPIMAGE_H*/