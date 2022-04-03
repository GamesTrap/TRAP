#ifndef TRAP_TEXTURECUBE_H
#define TRAP_TEXTURECUBE_H

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
		                                          const std::array<std::filesystem::path, 6>& filepaths);
		static Scope<TextureCube> CreateFromFile(const std::string& name, const std::filesystem::path& filepath,
		                                         TextureCubeFormat format);
		static Scope<TextureCube> CreateFromFile(const std::filesystem::path& filepath, TextureCubeFormat format);
		static Scope<TextureCube> CreateFromImage(const std::string& name, const Scope<Image>& img,
		                                          TextureCubeFormat format);
		//Order: Right, Left, Top, Bottom, Back, Front
		static Scope<TextureCube> CreateFromImages(const std::string& name, const std::array<Scope<Image>, 6>& imgs);
		static Scope<TextureCube> Create(); //Fallback Texture

		std::array<std::filesystem::path, 6> GetFilePaths() const;
		TextureCubeFormat GetTextureCubeFormat() const;
		uint32_t GetDepth() const override;
		uint32_t GetArraySize() const override;

		void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel, uint32_t arrayLayer) override;

		~TextureCube() override = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		TextureCube(const TextureCube&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TextureCube& operator=(const TextureCube&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TextureCube(TextureCube&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TextureCube& operator=(TextureCube&&) = default;

	protected:
		TextureCube(std::string name, std::array<std::filesystem::path, 6> filepaths, TextureCubeFormat format);
		TextureCube(std::string name, std::filesystem::path filepath, TextureCubeFormat format);

		std::array<std::filesystem::path, 6> m_filepaths;
		TextureCubeFormat m_textureFormat;
	};
}

#endif /*TRAP_TEXTURECUBE_H*/