#ifndef _TRAP_TEXTURECUBE_H_
#define _TRAP_TEXTURECUBE_H_

#include "Texture.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	enum class TextureCubeFormat
	{
		Cross,
		MultiFile,
		//Equiretangular //TODO
	};

	class TextureCube : public Texture
	{
	public:
		static Scope<TextureCube> CreateFromFiles(const std::string& name,
		                                          const std::array<std::string, 6>& filepaths, TextureUsage usage);
		static Scope<TextureCube> CreateFromFile(const std::string& name, const std::string& filepath,
		                                         TextureCubeFormat format, TextureUsage usage);
		static Scope<TextureCube> CreateFromFile(const std::string& filepath, TextureCubeFormat format,
		                                         TextureUsage usage);
		static Scope<TextureCube> CreateFromImage(const std::string& name, const Scope<Image>& img,
		                                          TextureCubeFormat format, TextureUsage usage);
		static Scope<TextureCube> CreateFromImages(const std::string& name, const std::array<Scope<Image>, 6>& imgs,
		                                           TextureUsage usage);
		static Scope<TextureCube> Create(TextureUsage usage); //Fallback Texture

		std::array<std::string, 6> GetFilePaths() const;
		TextureCubeFormat GetTextureCubeFormat() const;
		uint32_t GetDepth() const override;
		uint32_t GetArraySize() const override;

		~TextureCube() = default;
	protected:
		TextureCube();

		std::array<std::string, 6> m_filepaths;
		TextureCubeFormat m_textureFormat;
	};
}

#endif /*_TRAP_TEXTURECUBE_H_*/