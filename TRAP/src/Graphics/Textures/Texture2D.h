#ifndef _TRAP_TEXTURE2D_H_
#define _TRAP_TEXTURE2D_H_

#include "Texture.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	class Texture2D : public Texture
	{
	public:
		static Scope<Texture2D> CreateFromFile(const std::string& name, std::string_view filepath,
		                                       TextureUsage usage);
		static Scope<Texture2D> CreateFromFile(std::string_view filepath, TextureUsage usage);
		static Scope<Texture2D> CreateFromImage(const std::string& name, const TRAP::Scope<TRAP::Image>& img,
		                                        TextureUsage usage);
		static Scope<Texture2D> CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel,
		                                    Image::ColorFormat format, TextureUsage usage);
		static Scope<Texture2D> Create(TextureUsage usage); //Fallback Texture

		const std::string& GetFilePath() const;

		void UploadImage(const TRAP::Scope<TRAP::Image>& image) override;

		static void UpdateLoadingTextures();

		~Texture2D() = default;
	protected:
		Texture2D();

		std::string m_filepath;

		static std::vector<std::pair<Texture2D*, std::future<Scope<Image>>>> m_loadingTextures;
	};
}

#endif /*_TRAP_TEXTURE2D_H_*/