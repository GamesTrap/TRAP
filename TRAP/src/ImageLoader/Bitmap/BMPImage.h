#ifndef TRAP_BMPIMAGE_H
#define TRAP_BMPIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
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
		/// @brief Decode run length encoded 8-bit BMP data.
		/// @param compressedImageData Compressed image data.
		/// @param colorTable Color table.
		constexpr void DecodeRLE8(std::vector<u8>& compressedImageData, const std::vector<u8>* colorTable);

		std::vector<u8> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::BMPImage::GetPixelData() const noexcept
{
	return m_data;
}
//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::INTERNAL::BMPImage::DecodeRLE8(std::vector<u8>& compressedImageData,
	                                                const std::vector<u8>* const colorTable)
{
	u32 x = 0, y = 0;
	u8 t = 0, r = 0;

	u32 dataIndex = 0;

	if(colorTable != nullptr)
	{
		//Compressed RGBA
		while (true)
		{
			u8 color = compressedImageData[dataIndex++];

			if (color != 0)
			{
				r = compressedImageData[dataIndex++] * 4;

				for (t = 0; t < color; t++)
				{
					m_data[((y * m_width + x) * 4) + 0] = (*colorTable)[r + 2];
					m_data[((y * m_width + x) * 4) + 1] = (*colorTable)[r + 1];
					m_data[((y * m_width + x) * 4) + 2] = (*colorTable)[r + 0];
					m_data[((y * m_width + x) * 4) + 3] = (*colorTable)[r + 3];
					x++;
				}
			}
			else
			{
				r = compressedImageData[dataIndex++];

				if (r == 0)
				{
					x = 0;
					y++;
					continue;
				}
				if (r == 1)
					return;
				if (r == 2)
				{
					x += compressedImageData[dataIndex++];
					y += compressedImageData[dataIndex++];
					continue;
				}

				for (t = 0; t < r; t++)
				{
					color = compressedImageData[dataIndex++] * 4;
					m_data[((y * m_width + x) * 4) + 0] = (*colorTable)[color + 2];
					m_data[((y * m_width + x) * 4) + 1] = (*colorTable)[color + 1];
					m_data[((y * m_width + x) * 4) + 2] = (*colorTable)[color + 0];
					m_data[((y * m_width + x) * 4) + 3] = (*colorTable)[color + 3];
					x++;
				}

				color = r % 2;

				if (color != 0)
					dataIndex++;
			}
		}
	}

	//Compressed Grayscale
	while (true)
	{
		u8 color = compressedImageData[dataIndex++];

		if (color != 0)
		{
			r = compressedImageData[dataIndex++];

			for (t = 0; t < color; t++)
			{
				m_data[x + (y * m_width)] = r;
				x++;
			}
		}
		else
		{
			r = compressedImageData[dataIndex++];

			if (r == 0)
			{
				x = 0;
				y++;
				continue;
			}
			if (r == 1)
				return;
			if (r == 2)
			{
				x += compressedImageData[dataIndex++];
				y += compressedImageData[dataIndex++];
				continue;
			}

			for (t = 0; t < r; t++)
			{
				color = compressedImageData[dataIndex++];
				m_data[x + (y * m_width)] = color;
				x++;
			}

			color = r % 2;

			if (color != 0)
				dataIndex++;
		}
	}
}

#endif /*TRAP_BMPIMAGE_H*/
