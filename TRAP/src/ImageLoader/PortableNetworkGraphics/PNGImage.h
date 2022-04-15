#ifndef TRAP_PNGIMAGE_H
#define TRAP_PNGIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNGImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit PNGImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		PNGImage(const PNGImage&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PNGImage& operator=(const PNGImage&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PNGImage(PNGImage&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PNGImage& operator=(PNGImage&&) = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PNGImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

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
			bool eXIf = false;
		};

		struct NextChunk
		{
			uint32_t Length = 0;
			std::string MagicNumber{};
		};

		/// <summary>
		/// Process a PNG chunk.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <param name="alreadyLoaded">Flags to indicate which chunks have already been loaded.</param>
		/// <param name="needSwap">Flag to indicate if the data needs to be swapped.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data,
		                         AlreadyLoaded& alreadyLoaded, bool needSwap);

		struct IHDRChunk
		{
			uint32_t Width = 0;
			uint32_t Height = 0;
			uint8_t BitDepth = 0;
			uint8_t ColorType = 0;
			uint8_t CompressionMethod = 0;
			uint8_t FilterMethod = 0;
			uint8_t InterlaceMethod = 0;
			std::array<uint8_t, 4> CRC{};
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

		//Mandatory PNG chunks are IHDR, IDAT(s), IEND.

		/// <summary>
		/// Process the IHDR chunk.
		/// IHDR is the first chunk in the PNG file and contains the width, height,
		/// bit depth, color type, compression method, filter method and interlace method.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <param name="needSwap">Flag to indicate if the data needs to be swapped.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessIHDR(std::ifstream& file, Data& data, bool needSwap);
		/// <summary>
		/// Process the optional cHRM chunk.
		/// cHRM contains the chromaticities of the red, green and blue primaries and the referenced white point.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesscHRM(std::ifstream& file);
		/// <summary>
		/// Process the optional gAMA chunk.
		/// gAMA contains the images gamma value.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessgAMA(std::ifstream& file);
		/// <summary>
		/// Process the optional iCCP chunk.
		/// iCCP contains the ICC profile name and the ICC profile data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Length of the chunk.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessiCCP(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional sBIT chunk.
		/// sBIT contains the significant bits for each sample.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesssBIT(std::ifstream& file, const Data& data);
		/// <summary>
		/// Process the optional sRGB chunk.
		/// sRGB contains the rendering intent.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesssRGB(std::ifstream& file);
		/// <summary>
		/// Process the optional bKGD chunk.
		/// bKGD contains the image background color.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <param name="needSwap">Flag to indicate if the data needs to be swapped.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessbKGD(std::ifstream& file, const Data& data, bool needSwap);
		/// <summary>
		/// Process the optional hIST chunk.
		/// hIST contains the palette histogram.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesshIST(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional tRNS chunk.
		/// tRNS contains the transparency data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesstRNS(std::ifstream& file, uint32_t length, Data& data);
		/// <summary>
		/// Process the optional pHYs chunk.
		/// pHYs contains the physical pixel dimensions.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesspHYs(std::ifstream& file);
		/// <summary>
		/// Process the optional sPLT chunk.
		/// sPLT contains the suggested palette.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesssPLT(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional tIME chunk.
		/// tIME contains the last modification time.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="needSwap">Flag to indicate if the data needs to be swapped.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesstIME(std::ifstream& file, bool needSwap);
		/// <summary>
		/// Process the optional iTXt chunk.
		/// iTXt contains the international textual data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessiTXt(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional tEXt chunk.
		/// tEXt contains the textual data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesstEXt(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional zTXt chunk.
		/// zTXt contains the compressed textual data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesszTXt(std::ifstream& file, uint32_t length);
		/// <summary>
		/// Process the optional PLTE chunk.
		/// PLTE contains the palette.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessPLTE(std::ifstream& file, Data& data, uint32_t length);
		/// <summary>
		/// Process the IDAT chunk.
		/// IDAT contains the image data.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcessIDAT(std::ifstream& file, Data& data, uint32_t length);
		/// <summary>
		/// Process the optional eXIf chunk.
		/// eXIf contains the extended image information.
		/// </summary>
		/// <param name="file">Open PNG file.</param>
		/// <param name="length">Chunk length.</param>
		/// <returns>True if the chunk was processed successfully, false otherwise.</returns>
		static bool ProcesseXIf(std::ifstream& file, uint32_t length);

		/// <summary>
		/// Check IHDR validity.
		/// </summary>
		/// <param name="ihdrChunk">Loaded IHDR chunk data.</param>
		/// <returns>True if the IHDR chunk is valid, false otherwise.</returns>
		static bool IHDRCheck(const IHDRChunk& ihdrChunk);
		/// <summary>
		/// Check tIME timestamp validity.
		/// </summary>
		/// <param name="timeChunk">Loaded tIME chunk data.</param>
		/// <returns>True if the tIME timestampt is valid, false otherwise.</returns>
		static bool tIMECheck(const tIMEChunk& timeChunk);

		std::vector<uint8_t> m_data;
		std::vector<uint16_t> m_data2Byte;

		/// <summary>
		/// Decompress PNG data.
		/// </summary>
		/// <param name="source">Source data.</param>
		/// <param name="sourceLength">Source data length.</param>
		/// <param name="destination">Destination for uncompressed data.</param>
		/// <param name="destinationLength">Destination size.</param>
		/// <returns>True if the data was decompressed successfully, false otherwise.</returns>
		static bool DecompressData(uint8_t* source, int sourceLength, uint8_t* destination, int destinationLength);
		/// <summary>
		/// Unfilter scanline depending on given filter type.
		/// </summary>
		/// <param name="recon">
		/// <param name="scanline">Pointer to scanline.</param>
		/// <param name="precon">
		/// <param name="byteWidth">Width of the scanline in bytes.</param>
		/// <param name="filterType">Filter type.</param>
		/// <param name="length">Length of the scanline in bytes.</param>
		/// <returns>True if the scanline was unfiltered successfully, false otherwise.</returns>
		static bool UnFilterScanline(uint8_t* recon, const uint8_t* scanline, const uint8_t* precon,
		                             std::size_t byteWidth, uint8_t filterType, std::size_t length);
		/// <summary>
		/// Unfilter decompressed image data.
		/// </summary>
		/// <param name="out">Pointer to output storage.</param>
		/// <param name="in">Pointer to decompressed data.</param>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		/// <returns>True on successful unfiltering, false otherwise.</returns>
		static bool UnFilter(uint8_t* out, const uint8_t* in, uint32_t width, uint32_t height, uint32_t bitsPerPixel);
		/// <summary>
		/// Paeth predictor used by PNG filter type 4.
		/// </summary>
		/// <param name="a">Left pixel.</param>
		/// <param name="b">Above pixel.</param>
		/// <param name="c">Above-left pixel.</param>
		/// <returns>Predicted pixel.</returns>
		static uint8_t PaethPredictor(uint16_t a, uint16_t b, uint16_t c);
		/// <summary>
		/// Retrieve the raw/decompressed IDAT byte size.
		/// </summary>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		/// <returns>Raw IDAT size.</returns>
		static std::size_t GetRawSizeIDAT(uint32_t width, uint32_t height, uint32_t bitsPerPixel);
		/// <summary>
		/// Retrieve the raw/expected pixel data byte size.
		/// </summary>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		/// <returns>Raw pixel data byte size.</returns>
		static std::size_t GetRawSize(uint32_t width, uint32_t height, uint32_t bitsPerPixel);
		/// <summary>
		/// Post process scanlines
		/// This function converts filtered-padded-interlaced data into pure 2D image buffer data.
		/// Note: The in buffer will be overwritten with intermediate data!
		/// </summary>
		/// <param name="out">Pointer to output storage.</param>
		/// <param name="in">Pointer to decompressed data.</param>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		/// <param name="interlaceMethdo">Interlace method.</param>
		/// <returns>True on successful post processing, false otherwise.</returns>
		static bool PostProcessScanlines(uint8_t* out, uint8_t* in, uint32_t width, uint32_t height,
		                                 uint32_t bitsPerPixel, uint8_t interlaceMethod);
		/// <summary>
		/// Outputs various dimensions and positions in the image related to the Adam7 reduced images.
		/// </summary>
		/// <param name="passW">Output containing the width of the 7 passes.</param>
		/// <param name="passH">Output containing the height of the 7 passes.</param>
		/// <param name="filterPassStart">
		/// Output containing the index of the start and end of each reduced image with filter bytes.
		/// </param>
		/// <param name="paddedPassStart">
		/// Output containing the index of the start and end of each reduced image when without filter
		/// bytes but with padded scanlines.
		/// </param>
		/// <param name="passStart">
		/// Output containing the index of the start and end of each reduced image without padding
		/// between scanlines, but still padding between the images.
		/// </param>
		/// <param name="width">Image width (non-interlaced).</param>
		/// <param name="height">Image height (non-interlaced).</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		static void Adam7GetPassValues(std::array<uint32_t, 7>& passW,
		                               std::array<uint32_t, 7>& passH,
		                               std::array<std::size_t, 8>& filterPassStart,
		                               std::array<std::size_t, 8>& paddedPassStart,
		                               std::array<std::size_t, 8>& passStart,
		                               uint32_t width,
		                               uint32_t height,
		                               uint32_t bitsPerPixel);
		/// <summary>
		/// Deinterlace an Adam7 interlaced image.
		/// </summary>
		/// <param name="out">Same pixels as in, but re-ordered so that they are now a non-interlaced image with size width * height.</param>
		/// <param name="in">Adam7 interlaced image, with no padding bits between scanlines, but between reduced images so that each reduced image starts at a byte.</param>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="bitsPerPixel">Image bits per pixel.</param>
		static void Adam7DeInterlace(uint8_t* out, const uint8_t* in, uint32_t width, uint32_t height,
		                             uint32_t bitsPerPixel);
		/// <summary>
		/// Convert pixel data to 16 bits per channel with endianness correction.
		/// </summary>
		/// <param name="raw">Raw pixel data.</param>
		/// <returns>Converted 16 bpc pixel data.</returns>
		static std::vector<uint16_t> ConvertTo2Byte(std::vector<uint8_t>& raw);
		/// <summary>
		/// Resolves indexed pixel data to RGBA.
		/// </summary>
		/// <param name="raw">Raw pixel data.</param>
		/// <param name="width">Image width.</param>
		/// <param name="height">Image height.</param>
		/// <param name="data">Data containing information about the image.</param>
		/// <returns>Raw pixel data.</returns>
		static std::vector<uint8_t> ResolveIndexed(std::vector<uint8_t>& raw, uint32_t width, uint32_t height,
		                                           const Data& data);
	};
}

#endif /*TRAP_PNGIMAGE_H*/