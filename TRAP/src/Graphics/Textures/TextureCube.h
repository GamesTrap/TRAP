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
		/// <summary>
		/// Create a cube texture from 6 files.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepaths">
		/// File paths of the 6 texture files.
		/// Order: +X, -X, +Y, -Y, +Z, -Z
		/// </param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<TextureCube> CreateFromFiles(const std::string& name,
		                                          const std::array<std::filesystem::path, 6>& filepaths);
		/// <summary>
		/// Create a cube texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of texture.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<TextureCube> CreateFromFile(const std::string& name, const std::filesystem::path& filepath,
		                                         TextureCubeFormat format);
		/// <summary>
		/// Create a cube texture from file.
		/// File name will be used as the texture name.
		/// </summary>
		/// <param name="filepath">File path of texture.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<TextureCube> CreateFromFile(const std::filesystem::path& filepath, TextureCubeFormat format);
		/// <summary>
		/// Create a cube texture from TRAP::Image.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="img">Image to create the texture from.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<TextureCube> CreateFromImage(const std::string& name, const Scope<Image>& img,
		                                          TextureCubeFormat format);
		/// <summary>
		/// Create a cube texture from 6 TRAP::Images.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="imgs">
		/// Images to create the texture from.
		/// Order: +X, -X, +Y, -Y, +Z, -Z
		/// </param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<TextureCube> CreateFromImages(const std::string& name, const std::array<Scope<Image>, 6>& imgs);
		/// <summary>
		/// Create the fallback texture.
		/// </summary>
		/// <returns>Fallback texture.</returns>
		static Scope<TextureCube> Create(); //Fallback Texture

		/// <summary>
		/// Retrieve the file paths of the texture.
		/// </summary>
		/// <returns>File paths of the texture.</returns>
		std::array<std::filesystem::path, 6> GetFilePaths() const;
		/// <summary>
		/// Retrieve the format of the texture.
		/// </summary>
		/// <returns>Format of the texture.</returns>
		TextureCubeFormat GetTextureCubeFormat() const;
		/// <summary>
		/// Retrieve the depth of the texture.
		/// </summary>
		/// <returns>Depth of the texture.</returns>
		uint32_t GetDepth() const override;
		/// <summary>
		/// Retrieve the array size of the texture.
		/// </summary>
		/// <returns>Array size of the texture.</returns>
		uint32_t GetArraySize() const override;

		/// <summary>
		/// Update the texture via raw pixel data.
		/// </summary>
		/// <param name="data">Raw pixel data.</param>
		/// <param name="sizeInBytes">Size of the raw pixel data.</param>
		/// <param name="mipLevel">Mip level to update.</param>
		/// <param name="arrayLayer">Array layer to update.</param>
		void Update(const void* data, uint32_t sizeInBytes, uint32_t mipLevel, uint32_t arrayLayer) override;

		/// <summary>
		/// Destructor
		/// </summary>
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
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="name">Name of the texture.</param>
		/// <param name="filepaths">File paths of the texture.</param>
		/// <param name="format">Format of the texture.</param>
		TextureCube(std::string name, std::array<std::filesystem::path, 6> filepaths, TextureCubeFormat format);
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="name">Name of the texture.</param>
		/// <param name="filepath">File path of the texture.</param>
		/// <param name="format">Format of the texture.</param>
		TextureCube(std::string name, std::filesystem::path filepath, TextureCubeFormat format);

		std::array<std::filesystem::path, 6> m_filepaths;
		TextureCubeFormat m_textureFormat;
	};
}

#endif /*TRAP_TEXTURECUBE_H*/