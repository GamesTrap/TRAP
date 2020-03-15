#ifndef _TRAP_BMPIMAGE_H_
#define _TRAP_BMPIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class BMPImage final : public Image
	{
	public:
		explicit BMPImage(std::string filepath);
		BMPImage(const BMPImage&) = default;
		BMPImage& operator=(const BMPImage&) = default;
		BMPImage(BMPImage&&) = default;
		BMPImage& operator=(BMPImage&&) = default;
		~BMPImage() = default;

		const void* GetPixelData() const override;
		uint32_t GetPixelDataSize() const override;

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
		
		std::vector<uint8_t> m_data;
	};
}

#endif /*_TRAP_BMPIMAGE_H_*/