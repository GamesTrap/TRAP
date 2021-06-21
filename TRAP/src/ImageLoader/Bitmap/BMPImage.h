#ifndef _TRAP_BMPIMAGE_H_
#define _TRAP_BMPIMAGE_H_

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class BMPImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit BMPImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		BMPImage(const BMPImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		BMPImage& operator=(const BMPImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		BMPImage(BMPImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		BMPImage& operator=(BMPImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~BMPImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the Image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the Image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

	private:
		typedef struct Bitfield
		{
			uint32_t Start;
			uint32_t Span;
		} BitField;
		
		bool ValidateBitFields(std::array<BitField, 4>& bitFields, std::array<uint32_t, 4>& masks) const;
		static bool ParseBitfield(BitField& field, uint32_t mask);
		static uint8_t Make8Bits(uint32_t value, uint32_t bitSpan);
		static uint32_t ApplyBitField(uint32_t x, BitField& bitField);
		static uint32_t ApplyBitField(uint16_t x, BitField& bitField);
		void DecodeRLE8(std::vector<uint8_t>& compressedImageData, std::vector<uint8_t>* colorTable);
		
		std::vector<uint8_t> m_data;
	};
}

#endif /*_TRAP_BMPIMAGE_H_*/