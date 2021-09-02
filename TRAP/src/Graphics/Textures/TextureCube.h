#ifndef _TRAP_TEXTURECUBE_H_
#define _TRAP_TEXTURECUBE_H_

#include "Texture.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	using TextureCubeFormat = RendererAPI::TextureCubeType;

	class TextureCube : public Texture
	{
	public:
		//Order: Right, Left, Top, Bottom, Back, Front
		static Scope<TextureCube> CreateFromFiles(const std::string& name,
		                                          const std::array<std::string, 6>& filepaths);
		static Scope<TextureCube> CreateFromFile(const std::string& name, const std::string& filepath,
		                                         TextureCubeFormat format);
		static Scope<TextureCube> CreateFromFile(const std::string& filepath, TextureCubeFormat format);
		static Scope<TextureCube> CreateFromImage(const std::string& name, const Scope<Image>& img,
		                                          TextureCubeFormat format);
		//Order: Right, Left, Top, Bottom, Back, Front
		static Scope<TextureCube> CreateFromImages(const std::string& name, const std::array<Scope<Image>, 6>& imgs);
		static Scope<TextureCube> Create(); //Fallback Texture

		std::array<std::string, 6> GetFilePaths() const;
		TextureCubeFormat GetTextureCubeFormat() const;
		uint32_t GetDepth() const override;
		uint32_t GetArraySize() const override;

		void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel = 0, uint32_t arrayLayer = 0) override;

		~TextureCube() = default;
	protected:
		TextureCube(const std::string& name, const std::array<std::string, 6>& filepaths, TextureCubeFormat format);
		TextureCube(const std::string& name, const std::string& filepath, TextureCubeFormat format);

		std::array<std::string, 6> m_filepaths;
		TextureCubeFormat m_textureFormat;
	};
}

#endif /*_TRAP_TEXTURECUBE_H_*/