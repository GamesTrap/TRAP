#ifndef TRAP_BMPIMAGE_H
#define TRAP_BMPIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class BMPImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit BMPImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		BMPImage(const BMPImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		BMPImage& operator=(const BMPImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		BMPImage(BMPImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		BMPImage& operator=(BMPImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~BMPImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] constexpr uint64_t GetPixelDataSize() const noexcept override;

	private:
		struct BitField
		{
			uint32_t Start;
			uint32_t Span;
		};

		struct Header
		{
			uint16_t MagicNumber = 0; //Used to check format
			uint32_t Size = 0; //File size in bytes
			uint32_t DataOffset = 0; //Offset from file start - first pixel data
		};

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
		};

		/// <summary>
		/// Validate bit fields via the given masks.
		/// </summary>
		/// <param name="bitFields">Bit fields to validate.</param>
		/// <param name="masks">Mask to validate the bit fields against.</param>
		/// <returns>True if the bit fields are valid, false otherwise.</returns>
		[[nodiscard]] constexpr bool ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<uint32_t, 4>& masks) const noexcept;
		/// <summary>
		/// Parse a bit field.
		/// </summary>
		/// <param name="field">Bit field to parse.</param>
		/// <param name="mask">Mask.</param>
		/// <returns>True if the bit field was parsed successfully, false otherwise.</returns>
		[[nodiscard]] static constexpr bool ParseBitfield(BitField& field, uint32_t mask) noexcept;
		/// <summary>
		/// Convert given value to 8 bits.
		/// </summary>
		/// <param name="value">Value to convert.</param>
		/// <param name="bitSpan">Bit span.</param>
		/// <returns>Value as 8 bits.</returns>
		[[nodiscard]] static constexpr uint8_t Make8Bits(uint32_t value, uint32_t bitSpan) noexcept;
		/// <summary>
		/// Applies a bit field on the given value.
		/// </summary>
		/// <param name="x">Value to apply bit field to.</param>
		/// <param name="bitField">Bit field to apply.</param>
		/// <returns>Value with bit field applied.</returns>
		[[nodiscard]] static constexpr uint32_t ApplyBitField(uint32_t x, BitField& bitField) noexcept;
		/// <summary>
		/// Applies a bit field on the given value.
		/// </summary>
		/// <param name="x">Value to apply bit field to.</param>
		/// <param name="bitField">Bit field to apply.</param>
		/// <returns>Value with bit field applied.</returns>
		[[nodiscard]] static constexpr uint32_t ApplyBitField(uint16_t x, BitField& bitField) noexcept;
		/// <summary>
		/// Decode run length encoded 8-bit BMP data.
		/// </summary>
		/// <param name="compressedImageData">Compressed image data.</param>
		/// <param name="colorTable">Color table.</param>
		constexpr void DecodeRLE8(std::vector<uint8_t>& compressedImageData, const std::vector<uint8_t>* colorTable);

		std::vector<uint8_t> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::BMPImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint64_t TRAP::INTERNAL::BMPImage::GetPixelDataSize() const noexcept
{
	return m_data.size();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::INTERNAL::BMPImage::ValidateBitFields(std::array<BitField, 4>& bitFields,
	                                                                     std::array<uint32_t, 4>& masks) const noexcept
{
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

[[nodiscard]] constexpr bool TRAP::INTERNAL::BMPImage::ParseBitfield(BitField& field, const uint32_t mask) noexcept
{
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

[[nodiscard]] constexpr uint8_t TRAP::INTERNAL::BMPImage::Make8Bits(uint32_t value, const uint32_t bitSpan) noexcept
{
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

[[nodiscard]] constexpr uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint16_t x, BitField& bitField) noexcept
{
	return x >> bitField.Start & (BIT(bitField.Span) - 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::INTERNAL::BMPImage::ApplyBitField(const uint32_t x, BitField& bitField) noexcept
{
	return x >> bitField.Start & (BIT(bitField.Span) - 1);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::BMPImage::DecodeRLE8(std::vector<uint8_t>& compressedImageData,
	                                                const std::vector<uint8_t>* const colorTable)
{
	uint32_t x = 0, y = 0;
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