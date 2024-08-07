/*
Copyright (c) 2021, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT


QOI - The "Quite OK Image" format for fast, lossless image compression

Modified by Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_QOIIMAGE_H
#define TRAP_QOIIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class QOIImage final : public Image
	{
	public:
		/// @brief Constructor.
		/// @param filepath File path of the image to load.
		explicit QOIImage(std::filesystem::path filepath);
		/// @brief Copy constructor.
		QOIImage(const QOIImage&) noexcept = default;
		/// @brief Copy assignment operator.
		QOIImage& operator=(const QOIImage&) noexcept = default;
		/// @brief Move constructor.
		QOIImage(QOIImage&&) noexcept = default;
		/// @brief Move assignment operator.
		QOIImage& operator=(QOIImage&&) noexcept = default;
		/// @brief Destructor.
		~QOIImage() override = default;

		/// @brief Retrieve the raw pixel data of the image.
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
        /// @brief Decode the image QOI pixel data.
        /// @param file Open file stream of the image.
		/// @param fileSize Size of the file to decode.
        void DecodeImage(std::ifstream& file, const usize& fileSize);

		std::vector<u8> m_data;

		struct Header
		{
            std::string MagicNumber = std::string(4, '\0');
			u32 Width = 0;
			u32 Height = 0;
            u8 Channels = 0;
            u8 ColorSpace = 0;
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::QOIImage::GetPixelData() const noexcept
{
	return m_data;
}

#endif /*TRAP_QOIIMAGE_H*/