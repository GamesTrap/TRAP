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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit QOIImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		QOIImage(const QOIImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		QOIImage& operator=(const QOIImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		QOIImage(QOIImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		QOIImage& operator=(QOIImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~QOIImage() override = default;

		/// <summary>
		/// Retrieve the raw pixel data of the image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		[[nodiscard]] constexpr const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] constexpr u64 GetPixelDataSize() const noexcept override;

	private:
        /// <summary>
        /// Decode the image QOI pixel data.
        /// </summary>
        /// <param name="file">Open file stream of the image.</param>
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

[[nodiscard]] constexpr const void* TRAP::INTERNAL::QOIImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::QOIImage::GetPixelDataSize() const noexcept
{
	return m_data.size();
}

#endif /*TRAP_QOIIMAGE_H*/