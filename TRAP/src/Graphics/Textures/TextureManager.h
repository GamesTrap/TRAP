#ifndef TRAP_TEXTUREMANAGER_H
#define TRAP_TEXTUREMANAGER_H

#include "Texture.h"

namespace TRAP::Graphics::TextureManager
{
	/// <summary>
	/// Load a 2D texture from file.
	/// File name will be used as the texture name.
	/// </summary>
	/// <param name="filepath">File path of texture to load.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::filesystem::path& filepath,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a 2D texture from file.
	/// </summary>
	/// <param name="name">Name for the texture.</param>
	/// <param name="filepath">File path of texture to load.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::string& name, const std::filesystem::path& filepath,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a 2D texture from TRAP::Image.
	/// </summary>
	/// <param name="name">Name for the texture.</param>
	/// <param name="img">TRAP::Image to use as texture.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::string& name, const Image* const img,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a cube texture from file.
	/// </summary>
	/// <param name="name">Name for the texture.</param>
	/// <param name="filepath">File path of texture to load.</param>
	/// <param name="format">Format of the cube texture.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::string& name, const std::filesystem::path& filepath,
				      TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a cube texture from file.
	/// File name will be used as the texture name.
	/// </summary>
	/// <param name="filepath">File path of texture to load.</param>
	/// <param name="format">Format of the cube texture.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::filesystem::path& filepath, TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a cube texture from file.
	/// </summary>
	/// <param name="name">Name for the texture.</param>
	/// <param name="filepaths">
	/// File paths of all 6 textures to load.
	/// Order: +X, -X, +Y, -Y, +Z, -Z.
	/// </param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::string& name, const std::array<std::filesystem::path, 6>& filepaths,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// <summary>
	/// Load a cube texture from TRAP::Image.
	/// </summary>
	/// <param name="name">Name for the texture.</param>
	/// <param name="img">TRAP::Image to use as texture.</param>
	/// <param name="format">Format of the cube texture.</param>
	/// <param name="flags">Additional flags. Default: None.</param>
	/// <returns>Loaded texture.</returns>
	Ref<Texture> Load(const std::string& name, const Image* const img,
				      TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);

	/// <summary>
	/// Add a texture to the TextureManager.
	/// </summary>
	/// <param name="texture">Texture to add.</param>
	void Add(Ref<Texture> texture);
	/// <summary>
	/// Remove a texture from the TextureManager.
	/// </summary>
	/// <param name="texture">Texture to remove.</param>
	/// <returns>Removed texture on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<Texture>  Remove(Ref<Texture> texture);
	/// <summary>
	/// Remove a texture from the TextureManager via its name.
	/// </summary>
	/// <param name="name">Name of texture to remove.</param>
	/// <returns>Removed texture on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<Texture> Remove(const std::string& name);
	/// <summary>
	/// Retrieve a texture from the TextureManager.
	/// </summary>
	/// <param name="name">Name of texture to retrieve.</param>
	/// <param name="textureType">Type of texture.</param>
	/// <returns>Texture, Fallback texture if not found.</returns>
	[[nodiscard]] Ref<Texture> Get(const std::string& name, TextureType textureType);
	/// <summary>
	/// Retrieve a 2D texture from the TextureManager.
	/// </summary>
	/// <param name="name">Name of texture to retrieve.</param>
	/// <returns>Texture, Fallback texture if not found.</returns>
	[[nodiscard]] Ref<Texture> Get2D(const std::string& name);
	/// <summary>
	/// Retrieve a cube texture from the TextureManager.
	/// </summary>
	/// <param name="name">Name of texture to retrieve.</param>
	/// <returns>Texture, Fallback texture if not found.</returns>
	[[nodiscard]] Ref<Texture> GetCube(const std::string& name);
	/// <summary>
	/// Retrieve all loaded textures from the TextureManager.
	/// </summary>
	/// <returns>Map of all loaded textures.</returns>
	[[nodiscard]] const std::unordered_map<std::string, Ref<Texture>>& GetTextures() noexcept;
	/// <summary>
	/// Clear all textures from the TextureManager.
	/// Except fallback textures.
	/// </summary>
	void Clean();

	/// <summary>
	/// Reload a texture via its name or path.
	/// </summary>
	/// <param name="nameOrPath">Name or path of a texture.</param>
	/// <returns>
	/// Texture if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the texture was not found.
	/// </returns>
	Ref<Texture> Reload(const std::string& nameOrPath);
	/// <summary>
	/// Reload a texture.
	/// </summary>
	/// <param name="texture">Texture to reload.</param>
	/// <returns>
	/// Texture if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the texture was not found.
	/// </returns>
	Ref<Texture> Reload(Ref<Texture> texture);
	/// <summary>
	/// Reload all currently loaded textures.
	/// </summary>
	void ReloadAll();

	/// <summary>
	/// Check whether a texture exists.
	/// </summary>
	/// <param name="name">Name of texture to check.</param>
	/// <returns>True if texture exists, false otherwise.</returns>
	[[nodiscard]] bool Exists(const std::string& name);
	/// <summary>
	/// Check whether a texture exists by path.
	/// </summary>
	/// <param name="path">Path of texture to check.</param>
	/// <returns>True if texture exists, false otherwise.</returns>
	[[nodiscard]] bool ExistsPath(const std::filesystem::path& path);

	/// <summary>
	/// Shutdown the TextureManager.
	/// </summary>
	void Shutdown() noexcept;
}

#endif /*TRAP_TEXTUREMANAGER_H*/