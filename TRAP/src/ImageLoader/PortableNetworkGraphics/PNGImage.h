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
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
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
