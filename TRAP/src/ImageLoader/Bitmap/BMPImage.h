#ifndef TRAP_BMPIMAGE_H
#define TRAP_BMPIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	/// @brief Bitmap image.
	///
	/// Supported subset of specification:
	/// - Bitmaps with >= 8 bits per pixel
	/// - Bitmaps with the following compression types:
	///     - Uncompressed
	///     - Run length encoded (RLE8)
	///     - Bit fields
	class BMPImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit BMPImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		BMPImage(const BMPImage&) noexcept = default;
		/// @brief Copy assignment operator.
		BMPImage& operator=(const BMPImage&) noexcept = default;
		/// @brief Move constructor.
		BMPImage(BMPImage&&) noexcept = default;
		/// @brief Move assignment operator.
		BMPImage& operator=(BMPImage&&) noexcept = default;
		/// @brief Destructor.
		~BMPImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<u8> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::BMPImage::GetPixelData() const noexcept
{
	return m_data;
}

#endif /*TRAP_BMPIMAGE_H*/
