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
		const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const noexcept override;

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
		bool ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<uint32_t, 4>& masks) const noexcept;
		/// <summary>
		/// Parse a bit field.
		/// </summary>
		/// <param name="field">Bit field to parse.</param>
		/// <param name="mask">Mask.</param>
		/// <returns>True if the bit field was parsed successfully, false otherwise.</returns>
		static bool ParseBitfield(BitField& field, uint32_t mask) noexcept;
		/// <summary>
		/// Convert given value to 8 bits.
		/// </summary>
		/// <param name="value">Value to convert.</param>
		/// <param name="bitSpan">Bit span.</param>
		/// <returns>Value as 8 bits.</returns>
		static uint8_t Make8Bits(uint32_t value, uint32_t bitSpan) noexcept;
		/// <summary>
		/// Applies a bit field on the given value.
		/// </summary>
		/// <param name="x">Value to apply bit field to.</param>
		/// <param name="bitField">Bit field to apply.</param>
		/// <returns>Value with bit field applied.</returns>
		static uint32_t ApplyBitField(uint32_t x, BitField& bitField) noexcept;
		/// <summary>
		/// Applies a bit field on the given value.
		/// </summary>
		/// <param name="x">Value to apply bit field to.</param>
		/// <param name="bitField">Bit field to apply.</param>
		/// <returns>Value with bit field applied.</returns>
		static uint32_t ApplyBitField(uint16_t x, BitField& bitField) noexcept;
		/// <summary>
		/// Decode run length encoded 8-bit BMP data.
		/// </summary>
		/// <param name="compressedImageData">Compressed image data.</param>
		/// <param name="colorTable">Color table.</param>
		void DecodeRLE8(std::vector<uint8_t>& compressedImageData, const std::vector<uint8_t>* colorTable);

		std::vector<uint8_t> m_data;
	};
}

#endif /*TRAP_BMPIMAGE_H*/