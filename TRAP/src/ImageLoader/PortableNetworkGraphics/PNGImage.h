#ifndef _TRAP_PNGIMAGE_H_
#define _TRAP_PNGIMAGE_H_

#include "TRAPPCH.h"
#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNGImage final : public Image
	{
	public:
		explicit PNGImage(std::string filepath);

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
		std::string GetFilePath() const override;
		ImageFormat GetFormat() const override;

	private:
		struct RGBA
		{
			uint8_t Red = 0;
			uint8_t Green = 0;
			uint8_t Blue = 0;
			uint8_t Alpha = 255;
		};
		struct Data
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			uint8_t BitDepth = 0;
			uint8_t ColorType = 0;
			uint8_t CompressionMethod = 0; //Always 0 others are unsupported extensions!
			uint8_t FilterMethod = 0; //Always 0 others are unsupported extensions!
			uint8_t InterlaceMethod = 0; //Always 0 or 1 others are unsupported extensions!
			std::vector<RGBA> Palette{};
			std::vector<uint8_t> CompressedData{};
		};
		struct AlreadyLoaded
		{
			bool IHDR = false;
			bool cHRM = false;
			bool gAMA = false;
			bool iCCP = false;
			bool sBIT = false;
			bool sRGB = false;
			bool bKGD = false;
			bool hIST = false;
			bool tRNS = false;
			bool pHYs = false;
			bool tIME = false;
			bool PLTE = false;
			bool IDAT = false;
		};

		struct NextChunk
		{
			uint32_t Length = 0;
			std::array<uint8_t, 4> MagicNumber{};
		} nextChunk;

		bool ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data, AlreadyLoaded& alreadyLoaded, bool needSwap);

		struct IHDRChunk
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			uint8_t BitDepth = 0;
			uint8_t ColorType = 0;
			uint8_t CompressionMethod = 0;
			uint8_t FilterMethod = 0;
			uint8_t InterlaceMethod = 0;
			uint32_t CRC = 0;
		};
		struct tIMEChunk
		{
			uint16_t Year = 0;
			uint8_t Month = 0;
			uint8_t Day = 0;
			uint8_t Hour = 0;
			uint8_t Minute = 0;
			uint8_t Second = 0;
		};

		static bool ProcessIHDR(std::ifstream& file, Data& data, bool needSwap);
		static bool ProcesscHRM(std::ifstream& file, bool needSwap);
		static bool ProcessgAMA(std::ifstream& file, bool needSwap);
		static bool ProcessiCCP(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcesssBIT(std::ifstream& file, const Data& data, bool needSwap);
		static bool ProcesssRGB(std::ifstream& file, bool needSwap);
		static bool ProcessbKGD(std::ifstream& file, const Data& data, bool needSwap);
		static bool ProcesshIST(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcesstRNS(std::ifstream& file, Data& data, bool needSwap);
		static bool ProcesspHYs(std::ifstream& file, bool needSwap);
		static bool ProcesssPLT(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcesstIME(std::ifstream& file, bool needSwap);
		static bool ProcessiTXt(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcesstEXt(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcesszTXt(std::ifstream& file, uint32_t length, bool needSwap);
		static bool ProcessPLTE(std::ifstream& file, Data& data, uint32_t length, bool needSwap);
		static bool ProcessIDAT(std::ifstream& file, Data& data, uint32_t length, bool needSwap);
		static bool IHDRCheck(const IHDRChunk& ihdrChunk);
		static bool tIMECheck(const tIMEChunk& timeChunk);

		std::string m_filepath;
		uint32_t m_bitsPerPixel;
		uint32_t m_width;
		uint32_t m_height;
		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;
		ImageFormat m_format;
		bool m_isImageColored;
		bool m_isImageGrayScale;
		bool m_hasAlphaChannel;

		static int32_t DecompressData(uint8_t* source, int sourceLength, uint8_t* destination, int destinationLength);
	};
}

#endif /*_TRAP_PNGIMAGE_H_*/