#ifndef TRAP_TEXTURE2D_H
#define TRAP_TEXTURE2D_H

#include "Texture.h"
#include "ImageLoader/Image.h"

namespace TRAP::Graphics
{
	class Texture2D : public Texture
	{
	public:
		/// <summary>
		/// Create a 2D texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of the texture.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<Texture2D> CreateFromFile(const std::string& name, const std::filesystem::path& filepath);
		/// <summary>
		/// Create a 2D texture from file.
		/// File name will be used as the texture name.
		/// </summary>
		/// <param name="filepath">File path of the texture.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<Texture2D> CreateFromFile(const std::filesystem::path& filepath);
		/// <summary>
		/// Create a 2D texture from TRAP::Image.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="img">Image to create the texture from.</param>
		/// <returns>Loaded texture on success, Fallback texture otherwise.</returns>
		static Scope<Texture2D> CreateFromImage(const std::string& name, const TRAP::Image* const img);
		/// <summary>
		/// Create an empty 2D texture.
		/// </summary>
		/// <param name="width">Width for the texture.</param>
		/// <param name="height">Height for the texture.</param>
		/// <param name="bitsPerPixel">Bits per pixel for the texture.</param>
		/// <param name="format">Color format for the texture.</param>
		/// <returns>Empty texture on success, nullptr otherwise.</returns>
		static Scope<Texture2D> CreateEmpty(uint32_t width, uint32_t height, uint32_t bitsPerPixel,
		                                    Image::ColorFormat format);
		/// <summary>
		/// Create the fallback texture.
		/// </summary>
		/// <returns>Fallback texture.</returns>
		static Scope<Texture2D> Create(); //Fallback Texture

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
		/// Retrieve the file path of the texture.
		/// </summary>
		/// <returns>File path of the texture.</returns>
		const std::filesystem::path& GetFilePath() const;

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
		~Texture2D() override = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Texture2D(const Texture2D&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Texture2D& operator=(const Texture2D&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Texture2D(Texture2D&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Texture2D& operator=(Texture2D&&) = default;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Texture2D();
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of the texture.</param>
		Texture2D(std::string name, std::filesystem::path filepath);

		std::filesystem::path m_filepath;
	};
}

#endif /*TRAP_TEXTURE2D_H*/