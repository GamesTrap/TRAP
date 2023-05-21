#ifndef TRAP_RADIANCEIMAGE_H
#define TRAP_RADIANCEIMAGE_H

#include "ImageLoader/Image.h"

#include <optional>

namespace TRAP::INTERNAL
{
	class RadianceImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit RadianceImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		RadianceImage(const RadianceImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RadianceImage& operator=(const RadianceImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RadianceImage(RadianceImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RadianceImage& operator=(RadianceImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~RadianceImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] uint64_t GetPixelDataSize() const noexcept override;

	private:
		using RGBE = std::array<uint8_t, 4>;

		/// <summary>
		/// Convert exponent and value to floating point.
		/// As specified in the Radiance HDR specification.
		/// </summary>
		/// <param name="exponent">Exponent.</param>
		/// <param name="value">Value.</param>
		/// <returns>Floating point value.</returns>
		[[nodiscard]] static float ConvertComponent(int8_t exponent, int32_t value);
		/// <summary>
		/// Decode the given scanline.
		/// Used for RLE encoding and uncompressed data.
		/// </summary>
		/// <param name="scanline">Storage for the decoded scanline.</param>
		/// <param name="length">Scanline length.</param>
		/// <param name="file">Open Radiance HDR file.</param>
		/// <returns>True if successful, false otherwise.</returns>
		[[nodiscard]] static bool Decrunch(std::vector<RGBE>& scanline, uint32_t length, std::ifstream& file);
		/// <summary>
		/// Decode the given scanline.
		/// Used for old RLE encoding.
		/// </summary>
		/// <param name="scanline">Storage for the decoded scanline.</param>
		/// <param name="scanlineIndex">Start index for decoding.</param>
		/// <param name="length">Scanline length.</param>
		/// <param name="file">Open Radiance HDR file.</param>
		/// <returns>True if successful, false otherwise.</returns>
		[[nodiscard]] static bool OldDecrunch(std::vector<RGBE>& scanline, uint32_t scanlineIndex,
		                                      uint32_t length, std::ifstream& file);
		/// <summary>
		/// Extract color values from the scanlines.
		/// </summary>
		/// <param name="scanline">Scanlines.</param>
		/// <param name="data">Output storage for color data.</param>
		/// <param name="dataIndex">Index in the output storage to store data at.</param>
		void WorkOnRGBE(std::vector<RGBE>& scanline, std::vector<float>& data, uint64_t dataIndex);

		/// <summary>
		/// Check if file contains the magic number "#?".
		/// </summary>
		/// <param name="file">File to check.</param>
		/// <returns>True if magic number was found, false otherwise.</returns>
		[[nodiscard]] static bool ContainsMagicNumber(std::ifstream& file);

		/// <summary>
		/// Check if file contains a valid/known format.
		/// Currently only "32-bit_rle_rgbe" is supported.
		/// </summary>
		/// <param name="file">File to check.</param>
		/// <returns>True if valid/known format was found, false otherwise.</returns>
		[[nodiscard]] static bool ContainsSupportedFormat(std::ifstream& file);

		/// <summary>
		/// Skip lines which are not used for decoding.
		/// </summary>
		/// <param name="file">File to skip lines.</param>
		static void SkipUnusedLines(std::ifstream& file);

		/// <summary>
		/// Retrieve the resolution data from file.
		/// </summary>
		/// <param name="file">File to retrieve data from.</param>
		/// <param name="outNeedXFlip">True if image needs to be flipped on X axis.</param>
		/// <param name="outNeedYFlip">True if image needs to be flipped on Y axis.</param>
		/// <param name="outNeedRotateClockwise">True if image needs to be rotated 90 degrees clockwise.</param>
		/// <param name="outNeedRotateCounterClockwise">True if image needs to be rotated 90 degrees counter clockwise.</param>
		/// <returns>Image resolution on success, empty optional otherwise.</returns>
		[[nodiscard]] static std::optional<TRAP::Math::Vec2ui> RetrieveImageResolution(std::ifstream& file, bool& outNeedXFlip, bool& outNeedYFlip,
		                                                                               bool& outNeedRotateClockwise, bool& outNeedRotateCounterClockwise);

		std::vector<float> m_data;

		inline static constexpr uint32_t MinEncodingLength = 8; //Minimum scanline length for encoding
		inline static constexpr uint32_t MaxEncodingLength = 0x7FFF; //Maximum scanline length for encoding
		inline static constexpr uint32_t R = 0;
		inline static constexpr uint32_t G = 1;
		inline static constexpr uint32_t B = 2;
		inline static constexpr uint32_t E = 3;
	};
}

#endif /*TRAP_IMAGE_RADIANCE_H*/