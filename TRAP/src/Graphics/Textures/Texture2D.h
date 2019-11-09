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
		static Scope<Texture2D> CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
		                                         const std::vector<uint8_t>& pixelData, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
		                                         const std::vector<uint16_t>& pixelData, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format,
		                                         const std::vector<float>& pixelData, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel, ImageFormat format, TextureParameters parameters = TextureParameters());
		static Scope<Texture2D> Create(TextureParameters parameters = TextureParameters());

		TextureType GetType() const override;

	protected:
		static uint32_t s_maxTextureSize;
	};
}

#endif /*_TRAP_TEXTURE2D_H_*/