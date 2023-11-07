/*
LodePNG version 20230410

Copyright (c) 2005-2023 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.

Modified by Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_PNGIMAGE_H
#define TRAP_PNGIMAGE_H

#include <array>

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PNGImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit PNGImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		PNGImage(const PNGImage&) noexcept = default;
		/// @brief Copy assignment operator.
		PNGImage& operator=(const PNGImage&) noexcept = default;
		/// @brief Move constructor.
		PNGImage(PNGImage&&) noexcept = default;
		/// @brief Move assignment operator.
		PNGImage& operator=(PNGImage&&) noexcept = default;
		/// @brief Destructor.
		~PNGImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		struct RGBA
		{
			u8 Red = 0;
			u8 Green = 0;
			u8 Blue = 0;
			u8 Alpha = 255;
		};
		struct Data
		{
			u32 Width = 0;
			u32 Height = 0;
			u8 BitDepth = 0;
			u8 ColorType = 0;
			u8 CompressionMethod = 0; //Always 0 others are unsupported extensions!
			u8 FilterMethod = 0; //Always 0 others are unsupported extensions!
			u8 InterlaceMethod = 0; //Always 0 or 1 others are unsupported extensions!
			std::vector<RGBA> Palette{};
			std::vector<u8> CompressedData{};
		};
		struct AlreadyLoaded
		{
			bool IHDR = false;
			bool sRGB = false;
			bool tRNS = false;
			bool PLTE = false;
			bool IDAT = false;
		};

		struct NextChunk
		{
			u32 Length = 0;
			std::string MagicNumber{};
		};

		/// @brief Process a PNG chunk.
		/// @param nextChunk Length and magic number of the next chunk to process.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @param alreadyLoaded Flags to indicate which chunks have already been loaded.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcessChunk(NextChunk& nextChunk, std::ifstream& file, Data& data,
		                                       AlreadyLoaded& alreadyLoaded);

		struct IHDRChunk
		{
			u32 Width = 0;
			u32 Height = 0;
			u8 BitDepth = 0;
			u8 ColorType = 0;
			u8 CompressionMethod = 0;
			u8 FilterMethod = 0;
			u8 InterlaceMethod = 0;
			std::array<u8, 4> CRC{};
		};
		struct tIMEChunk
		{
			u16 Year = 0;
			u8 Month = 0;
			u8 Day = 0;
			u8 Hour = 0;
			u8 Minute = 0;
			u8 Second = 0;
		};

		//Mandatory PNG chunks are IHDR, IDAT(s), IEND.

		/// @brief Process the IHDR chunk.
		/// IHDR is the first chunk in the PNG file and contains the width, height,
		/// bit depth, color type, compression method, filter method and interlace method.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcessIHDR(std::ifstream& file, Data& data);
		/// @brief Process the optional sBIT chunk.
		/// sBIT contains the significant bits for each sample.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcesssBIT(std::ifstream& file, const Data& data);
		/// @brief Process the optional sRGB chunk.
		/// sRGB contains the rendering intent.
		/// @param file Open PNG file.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcesssRGB(std::ifstream& file);
		/// @brief Process the optional bKGD chunk.
		/// bKGD contains the image background color.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcessbKGD(std::ifstream& file, const Data& data);
		/// @brief Process the optional tRNS chunk.
		/// tRNS contains the transparency data.
		/// @param file Open PNG file.
		/// @param length Chunk length.
		/// @param data Data containing information about the image.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcesstRNS(std::ifstream& file, u32 length, Data& data);
		/// @brief Process the optional PLTE chunk.
		/// PLTE contains the palette.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @param length Chunk length.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcessPLTE(std::ifstream& file, Data& data, u32 length);
		/// @brief Process the IDAT chunk.
		/// IDAT contains the image data.
		/// @param file Open PNG file.
		/// @param data Data containing information about the image.
		/// @param length Chunk length.
		/// @return True if the chunk was processed successfully, false otherwise.
		[[nodiscard]] static bool ProcessIDAT(std::ifstream& file, Data& data, u32 length);

		/// @brief Check IHDR validity.
		/// @param ihdrChunk Loaded IHDR chunk data.
		/// @return True if the IHDR chunk is valid, false otherwise.
		[[nodiscard]] static bool IHDRCheck(const IHDRChunk& ihdrChunk);

		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;

		/// @brief Decompress PNG data.
		/// @param source Source data.
		/// @param destination Destination for uncompressed data.
		/// @return True if the data was decompressed successfully, false otherwise.
		[[nodiscard]] static bool DecompressData(std::span<const u8> source, std::span<u8> destination);
		/// @brief Unfilter scanline depending on given filter type.
		/// @param recon
		/// @param scanline Pointer to scanline.
		/// @param precon
		/// @param byteWidth Width of the scanline in bytes.
		/// @param filterType Filter type.
		/// @param length Length of the scanline in bytes.
		/// @return True if the scanline was unfiltered successfully, false otherwise.
		[[nodiscard]] static bool UnFilterScanline(u8* recon, const u8* scanline, const u8* precon,
		                                           usize byteWidth, u8 filterType, usize length);
		/// @brief Unfilter decompressed image data.
		/// @param out Pointer to output storage.
		/// @param in Pointer to decompressed data.
		/// @param width Image width.
		/// @param height Image height.
		/// @param bitsPerPixel Image bits per pixel.
		/// @return True on successful unfiltering, false otherwise.
		[[nodiscard]] static bool UnFilter(u8* out, const u8* in, u32 width, u32 height, u32 bitsPerPixel);
		/// @brief Paeth predictor used by PNG filter type 4.
		/// @param a Left pixel.
		/// @param b Above pixel.
		/// @param c Above-left pixel.
		/// @return Predicted pixel.
		[[nodiscard]] static u8 PaethPredictor(u16 a, u16 b, u16 c);
		/// @brief Retrieve the raw/decompressed IDAT byte size.
		/// @param width Image width.
		/// @param height Image height.
		/// @param bitsPerPixel Image bits per pixel.
		/// @return Raw IDAT size.
		[[nodiscard]] static usize GetRawSizeIDAT(u32 width, u32 height, u32 bitsPerPixel);
		/// @brief Retrieve the raw/expected pixel data byte size.
		/// @param width Image width.
		/// @param height Image height.
		/// @param bitsPerPixel Image bits per pixel.
		/// @return Raw pixel data byte size.
		[[nodiscard]] static usize GetRawSize(u32 width, u32 height, u32 bitsPerPixel);
		/// @brief Post process scanlines
		/// This function converts filtered-padded-interlaced data into pure 2D image buffer data.
		/// @param out Pointer to output storage.
		/// @param in Pointer to decompressed data.
		/// @param width Image width.
		/// @param height Image height.
		/// @param bitsPerPixel Image bits per pixel.
		/// @param interlaceMethod Interlace method.
		/// @return True on successful post processing, false otherwise.
		/// @note The in buffer will be overwritten with intermediate data!
		[[nodiscard]] static bool PostProcessScanlines(u8* out, u8* in, u32 width, u32 height,
		                                               u32 bitsPerPixel, u8 interlaceMethod);
		/// @brief Outputs various dimensions and positions in the image related to the Adam7 reduced images.
		/// @param passW Output containing the width of the 7 passes.
		/// @param passH Output containing the height of the 7 passes.
		/// @param filterPassStart Output containing the index of the start and end of each reduced image with filter bytes.
		/// @param paddedPassStart Output containing the index of the start and end of each reduced image when without filter
		/// bytes but with padded scanlines.
		/// @param passStart Output containing the index of the start and end of each reduced image without padding
		/// between scanlines, but still padding between the images.
		/// @param width Image width (non-interlaced).
		/// @param height Image height (non-interlaced).
		/// @param bitsPerPixel Image bits per pixel.
		static void Adam7GetPassValues(std::array<u32, 7>& passW,
		                               std::array<u32, 7>& passH,
		                               std::array<usize, 8>& filterPassStart,
		                               std::array<usize, 8>& paddedPassStart,
		                               std::array<usize, 8>& passStart,
		                               u32 width,
		                               u32 height,
		                               u32 bitsPerPixel);
		/// @brief Deinterlace an Adam7 interlaced image.
		/// @param out Same pixels as in, but re-ordered so that they are now a non-interlaced image with size width * height.
		/// @param in Adam7 interlaced image, with no padding bits between scanlines, but between reduced images so that each reduced image starts at a byte.
		/// @param width Image width.
		/// @param height Image height.
		/// @param bitsPerPixel Image bits per pixel.
		static void Adam7DeInterlace(u8* out, const u8* in, u32 width, u32 height,
		                             u32 bitsPerPixel);
		/// @brief Convert pixel data to 16 bits per channel with endianness correction.
		/// @param raw Raw pixel data.
		/// @return Converted 16 bpc pixel data.
		[[nodiscard]] static std::vector<u16> ConvertTo2Byte(std::vector<u8>& raw);
		/// @brief Resolves indexed pixel data to RGBA.
		/// @param raw Raw pixel data.
		/// @param width Image width.
		/// @param height Image height.
		/// @param data Data containing information about the image.
		/// @return Raw pixel data.
		[[nodiscard]] static std::vector<u8> ResolveIndexed(std::vector<u8>& raw, u32 width, u32 height,
		                                                         const Data& data);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::PNGImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.data();

	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::PNGImage::GetPixelDataSize() const noexcept
{
	if (!m_data2Byte.empty())
		return m_data2Byte.size() * sizeof(u16);

	return m_data.size();
}

#endif /*TRAP_PNGIMAGE_H*/