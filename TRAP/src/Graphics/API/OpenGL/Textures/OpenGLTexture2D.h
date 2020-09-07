#ifndef _TRAP_OPENGLTEXTURE2D_H_
#define _TRAP_OPENGLTEXTURE2D_H_

#include "Graphics/Textures/Texture2D.h"

namespace TRAP::Graphics::API
{
	class OpenGLTexture2D final : public Texture2D
	{
	public:
		explicit OpenGLTexture2D(TextureParameters parameters);
		OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t bitsPerPixel, Image::ColorFormat format, TextureParameters parameters);
		OpenGLTexture2D(std::string name, const Scope<Image>& img, TextureParameters parameters);
		OpenGLTexture2D(std::string name, std::string_view filepath, TextureParameters parameters);
		~OpenGLTexture2D();
		OpenGLTexture2D(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D& operator=(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D(OpenGLTexture2D&&) = default;
		OpenGLTexture2D& operator=(OpenGLTexture2D&&) = default;

		void Bind(uint32_t slot) const override;
		void Unbind(uint32_t slot) const override;

		uint32_t GetHandle() const;

		void SetWrap(TextureWrap wrap) override;
		void SetFilter(TextureFilter filter) override;

		void UploadImage(const TRAP::Scope<TRAP::Image>& image) override;

	private:
		enum class PixelDataType
		{
			Float,
			Unsigned_Short,
			Unsigned_Byte
		};
		static GLenum PixelDataTypeToOpenGL(PixelDataType type);

		void InitializeTexture();
		void UploadTexture(const Scope<Image>& image, bool exists) const;
		void UploadData(const Scope<Image>& image, PixelDataType type) const;

		void UploadFallback();
		bool CheckLimits(uint32_t width, uint32_t height);

		uint32_t m_handle;
		uint32_t m_width, m_height, m_bitsPerPixel;
		Image::ColorFormat m_colorFormat;
	};
}

#endif /*_TRAP_OPENGLTEXTURE2D_H_*/