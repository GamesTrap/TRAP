#ifndef TRAP_RADIANCEIMAGE_H
#define TRAP_RADIANCEIMAGE_H

#include "ImageLoader/Image.h"

#include <optional>

namespace TRAP::INTERNAL
{
	class RadianceImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit RadianceImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		RadianceImage(const RadianceImage&) noexcept = default;
		/// @brief Copy assignment operator.
		RadianceImage& operator=(const RadianceImage&) noexcept = default;
		/// @brief Move constructor.
		RadianceImage(RadianceImage&&) noexcept = default;
		/// @brief Move assignment operator.
		RadianceImage& operator=(RadianceImage&&) noexcept = default;
		/// @brief Destructor.
		~RadianceImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Constant pointer to the raw pixel data.
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// @brief Retrieve the size of the raw pixel data of the image.
		/// @return Size of the raw pixel data in bytes.
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
		using RGBE = std::array<u8, 4>;

		/// @brief Convert exponent and value to floating point.
		/// As specified in the Radiance HDR specification.
		/// @param exponent Exponent.
		/// @param value Value.
		/// @return Floating point value.
		[[nodiscard]] static f32 ConvertComponent(i8 exponent, i32 value);
		/// @brief Decode the given scanline.
		/// Used for RLE encoding and uncompressed data.
		/// @param scanline Storage for the decoded scanline.
		/// @param length Scanline length.
		/// @param file Open Radiance HDR file.
		/// @return True if successful, false otherwise.
		[[nodiscard]] static bool Decrunch(std::vector<RGBE>& scanline, u32 length, std::ifstream& file);
		/// @brief Decode the given scanline.
		/// Used for old RLE encoding.
		/// @param scanline Storage for the decoded scanline.
		/// @param scanlineIndex Start index for decoding.
		/// @param length Scanline length.
		/// @param file Open Radiance HDR file.
		/// @return True if successful, false otherwise.
		[[nodiscard]] static bool OldDecrunch(std::vector<RGBE>& scanline, u32 scanlineIndex,
		                                      u32 length, std::ifstream& file);
		/// @brief Extract color values from the scanlines.
		/// @param scanline Scanlines.
		/// @param data Output storage for color data.
		/// @param dataIndex Index in the output storage to store data at.
		void WorkOnRGBE(std::vector<RGBE>& scanline, std::vector<f32>& data, u64 dataIndex);

		/// @brief Check if file contains the magic number "#?".
		/// @param file File to check.
		/// @return True if magic number was found, false otherwise.
		[[nodiscard]] static bool ContainsMagicNumber(std::ifstream& file);

		/// @brief Check if file contains a valid/known format.
		/// Currently only "32-bit_rle_rgbe" is supported.
		/// @param file File to check.
		/// @return True if valid/known format was found, false otherwise.
		[[nodiscard]] static bool ContainsSupportedFormat(std::ifstream& file);

		/// @brief Skip lines which are not used for decoding.
		/// @param file File to skip lines.
		static void SkipUnusedLines(std::ifstream& file);

		/// @brief Retrieve the resolution data from file.
		/// @param file File to retrieve data from.
		/// @param outNeedXFlip True if image needs to be flipped on X axis.
		/// @param outNeedYFlip True if image needs to be flipped on Y axis.
		/// @param outNeedRotateClockwise True if image needs to be rotated 90 degrees clockwise.
		/// @param outNeedRotateCounterClockwise True if image needs to be rotated 90 degrees counter clockwise.
		/// @return Image resolution on success, empty optional otherwise.
		[[nodiscard]] static std::optional<TRAP::Math::Vec2ui> RetrieveImageResolution(std::ifstream& file, bool& outNeedXFlip, bool& outNeedYFlip,
		                                                                               bool& outNeedRotateClockwise, bool& outNeedRotateCounterClockwise);

		std::vector<f32> m_data;

		inline static constexpr u32 MinEncodingLength = 8; //Minimum scanline length for encoding
		inline static constexpr u32 MaxEncodingLength = 0x7FFF; //Maximum scanline length for encoding
		inline static constexpr u32 R = 0;
		inline static constexpr u32 G = 1;
		inline static constexpr u32 B = 2;
		inline static constexpr u32 E = 3;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::RadianceImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::RadianceImage::GetPixelDataSize() const noexcept
{
	return m_data.size() * sizeof(f32);
}

#endif /*TRAP_IMAGE_RADIANCE_H*/