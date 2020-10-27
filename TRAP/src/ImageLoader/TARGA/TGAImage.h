#ifndef _TRAP_TGAIMAGE_H_
#define _TRAP_TGAIMAGE_H_

#include "ImageLoader/Image.h"

namespace TRAP::INTERNAL
{	
	class TGAImage final : public Image
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="filepath">File path of the Image to load.</param>
		explicit TGAImage(std::string filepath);
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		TGAImage(const TGAImage&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		TGAImage& operator=(const TGAImage&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		TGAImage(TGAImage&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		TGAImage& operator=(TGAImage&&) = default;
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~TGAImage() = default;

		/// <summary>
		/// Retrieve the raw pixel data of the Image.
		/// </summary>
		/// <returns>Constant pointer to the raw pixel data.</returns>
		const void* GetPixelData() const override;
		/// <summary>
		/// Retrieve the size of the raw pixel data of the Image.
		/// </summary>
		/// <returns>Size of the raw pixel data in bytes.</returns>
		uint64_t GetPixelDataSize() const override;

	private:
		static std::vector<uint8_t> DecodeRLEBGRAMap(std::vector<uint8_t>& source, uint32_t width, uint32_t height, uint32_t channels, std::vector<uint8_t>& colorMap);
		static std::vector<uint8_t> DecodeRLEGrayScale(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR16ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGR24ToRGB24(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		static std::vector<uint8_t> ConvertRLEBGRA32ToRGBA(std::vector<uint8_t>& source, uint32_t width, uint32_t height);
		
		std::vector<uint8_t> m_data;
	};
}

#endif /*_TRAP_TGAIMAGE_H_*/