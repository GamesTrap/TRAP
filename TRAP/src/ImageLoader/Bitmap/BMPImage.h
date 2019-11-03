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

		void* GetPixelData() override;
		uint32_t GetPixelDataSize() const override;
		uint32_t GetBitsPerPixel() const override;
		uint32_t GetBytesPerPixel() const override;
		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;
		bool HasAlphaChannel() const override;
		bool IsImageGrayScale() const override;
		bool IsImageColored() const override;
		bool IsHDR() const override;
		std::string_view GetFilePath() const override;
		ImageFormat GetFormat() const override;

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
		
		std::string m_filepath;
		uint32_t m_bitsPerPixel;
		uint32_t m_width;
		uint32_t m_height;
		std::vector<uint8_t> m_data;
		ImageFormat m_format;
		bool m_isImageColored;
		bool m_isImageGrayScale;
		bool m_hasAlphaChannel;
	};
}

#endif /*_TRAP_BMPIMAGE_H_*/