#ifndef TRAP_PFMIMAGE_H
#define TRAP_PFMIMAGE_H

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{
	class PFMImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the image to load.</param>
		explicit PFMImage(std::filesystem::path filepath);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		PFMImage(const PFMImage&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		PFMImage& operator=(const PFMImage&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		PFMImage(PFMImage&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		PFMImage& operator=(PFMImage&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
		~PFMImage() override = default;

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
		std::vector<f32> m_data;

		struct Header
		{
			std::string MagicNumber;
			u32 Width = 0;
			u32 Height = 0;
			f32 ByteOrder = 0.0f;
		};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const void* TRAP::INTERNAL::PFMImage::GetPixelData() const noexcept
{
	return m_data.data();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u64 TRAP::INTERNAL::PFMImage::GetPixelDataSize() const noexcept
{
	return m_data.size() * sizeof(f32);
}

#endif /*TRAP_PFMIMAGE_H*/