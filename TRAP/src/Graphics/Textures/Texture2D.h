#ifndef _TRAP_TEXTURE2D_H_
#define _TRAP_TEXTURE2D_H_

#include "Texture.h"

namespace TRAP::Graphics
{
	class Texture2D : public Texture
	{
	public:
		static Scope<Texture2D> CreateFromFile(const std::string& name, const std::string& filepath, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateFromFile(const std::string& filepath, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateFromImage(const std::string& name, const TRAP::Scope<TRAP::Image>& img, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel, Image::ColorFormat format, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> Create(TextureParameters parameters = TextureParameters());

		std::string_view GetFilePath() const;
		
	protected:
		Texture2D();
		
		static uint32_t s_maxTextureSize;

		std::string m_filepath;
	};
}

#endif /*_TRAP_TEXTURE2D_H_*/