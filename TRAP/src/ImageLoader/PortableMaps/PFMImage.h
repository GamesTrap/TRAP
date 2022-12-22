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
		[[nodiscard]] const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] uint64_t GetPixelDataSize() const noexcept override;

	private:
		std::vector<float> m_data;

		struct Header
		{
			std::string MagicNumber = "";
			uint32_t Width = 0;
			uint32_t Height = 0;
			float ByteOrder = 0.0f;
		};
	};
}

#endif /*TRAP_PFMIMAGE_H*/