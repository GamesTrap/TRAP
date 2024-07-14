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
		/// @return Raw pixel data.
		[[nodiscard]] constexpr std::span<const u8> GetPixelData() const noexcept override;

	private:
		std::vector<u8> m_data;
		std::vector<u16> m_data2Byte;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::span<const u8> TRAP::INTERNAL::PNGImage::GetPixelData() const noexcept
{
	if (!m_data2Byte.empty())
	{
		const std::span data(m_data2Byte);
		return TRAP::Utils::AsBytes(data);
	}

	return m_data;
}

#endif /*TRAP_PNGIMAGE_H*/
