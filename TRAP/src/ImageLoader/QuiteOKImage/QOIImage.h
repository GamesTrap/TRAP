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
		[[nodiscard]] const void* GetPixelData() const noexcept override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		[[nodiscard]] uint64_t GetPixelDataSize() const noexcept override;

	private:
        /// <summary>
        /// Decode the image QOI pixel data.
        /// </summary>
        /// <param name="file">Open file stream of the image.</param>
        void DecodeImage(std::ifstream& file, const std::size_t& fileSize);

		std::vector<uint8_t> m_data;

		struct Header
		{
            std::string MagicNumber = std::string(4, '\0');
			uint32_t Width = 0;
			uint32_t Height = 0;
            uint8_t Channels = 0;
            uint8_t ColorSpace = 0;
		};
	};
}

#endif /*TRAP_QOIIMAGE_H*/