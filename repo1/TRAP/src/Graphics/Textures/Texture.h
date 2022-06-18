#ifndef _TRAP_TEXTURE_H_
#define _TRAP_TEXTURE_H_

#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	enum class TextureWrap
	{
		NONE = 0,
		Repeat,
		Mirrored_Repeat,
		Clamp_To_Edge,
		Clamp_To_Border
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest
	};

	enum class TextureType
	{
		Texture2D,
		TextureCube
	};

	struct TextureParameters
	{
		TextureFilter Filter;
		TextureWrap Wrap;

		TextureParameters();
		explicit TextureParameters(TextureFilter filter);
		TextureParameters(TextureFilter filter, TextureWrap wrap);
	};

	class Texture
	{
	protected:
		Texture();
		
	public:
		virtual ~Texture() = default;
		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		Texture(Texture&&) = default;
		Texture& operator=(Texture&&) = default;

		virtual void Bind(uint32_t slot) const = 0;
		virtual void Unbind(uint32_t slot) const = 0;

		const std::string& GetName() const;
		TextureType GetType() const;
		TextureParameters GetParameters() const;

		virtual void SetWrap(TextureWrap wrap) = 0;
		virtual void SetFilter(TextureFilter filter) = 0;

		static uint8_t GetStrideFromFormat(Image::ColorFormat format);

		static uint32_t TRAPImageFormatToOpenGL(Image::ColorFormat format);
		static uint32_t TRAPImageFormatToOpenGLPrecise(Image::ColorFormat format, uint32_t bytesPerPixel);
		static uint32_t TRAPTextureWrapToOpenGL(TextureWrap wrap);

	protected:
		std::string m_name;
		TextureType m_textureType;
		TextureParameters m_textureParameters;
	};
}

#endif /*_TRAP_TEXTURE_H_*/