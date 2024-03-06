#ifndef TRAP_TEXTUREMANAGER_H
#define TRAP_TEXTUREMANAGER_H

#include "Texture.h"

namespace TRAP::Graphics::TextureManager
{
	/// @brief Load a 2D texture from file.
	/// File name will be used as the texture name.
	/// @param filepath File path of texture to load.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::filesystem::path& filepath,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a 2D texture from file.
	/// @param name Name for the texture.
	/// @param filepath File path of texture to load.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, const std::filesystem::path& filepath,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a 2D texture from TRAP::Image.
	/// @param name Name for the texture.
	/// @param img TRAP::Image to use as texture.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, const Image* img,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a cube texture from file.
	/// @param name Name for the texture.
	/// @param filepath File path of texture to load.
	/// @param format Format of the cube texture.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, const std::filesystem::path& filepath,
				      TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a cube texture from file.
	/// File name will be used as the texture name.
	/// @param filepath File path of texture to load.
	/// @param format Format of the cube texture.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::filesystem::path& filepath, TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a cube texture from file.
	/// @param name Name for the texture.
	/// @param filepaths File paths of all 6 textures to load.
	/// Order: +X, -X, +Y, -Y, +Z, -Z.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, const std::array<std::filesystem::path, 6>& filepaths,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a cube texture from TRAP::Image.
	/// @param name Name for the texture.
	/// @param img TRAP::Image to use as texture.
	/// @param format Format of the cube texture.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, const Image* img,
				      TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);

	/// @brief Add a texture to the TextureManager.
	/// @param texture Texture to add.
	void Add(Ref<Texture> texture);
	/// @brief Remove a texture from the TextureManager.
	/// @param texture Texture to remove.
	/// @return Removed texture on success, nullptr otherwise.
	[[maybe_unused]] Ref<Texture>  Remove(const Ref<Texture>& texture);
	/// @brief Remove a texture from the TextureManager via its name.
	/// @param name Name of texture to remove.
	/// @return Removed texture on success, nullptr otherwise.
	[[maybe_unused]] Ref<Texture> Remove(const std::string& name);
	/// @brief Retrieve a texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @param textureType Type of texture.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> Get(const std::string& name, TextureType textureType);
	/// @brief Retrieve a 2D texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> Get2D(const std::string& name);
	/// @brief Retrieve a cube texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> GetCube(const std::string& name);
	/// @brief Retrieve all loaded textures from the TextureManager.
	/// @return Map of all loaded textures.
	[[nodiscard]] const std::unordered_map<std::string, Ref<Texture>, Utils::StringHasher, std::equal_to<>>& GetTextures() noexcept;
	/// @brief Clear all textures from the TextureManager.
	/// Except fallback textures.
	void Clean();

	/// @brief Reload a texture via its name or path.
	/// @param nameOrPath Name or path of a texture.
	/// @return Texture if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the texture was not found.
	Ref<Texture> Reload(const std::string& nameOrPath);
	/// @brief Reload a texture.
	/// @param texture Texture to reload.
	/// @return Texture if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the texture was not found.
	Ref<Texture> Reload(Ref<Texture> texture);
	/// @brief Reload all currently loaded textures.
	void ReloadAll();

	/// @brief Check whether a texture exists.
	/// @param name Name of texture to check.
	/// @return True if texture exists, false otherwise.
	[[nodiscard]] bool Exists(const std::string& name);
	/// @brief Check whether a texture exists by path.
	/// @param path Path of texture to check.
	/// @return True if texture exists, false otherwise.
	[[nodiscard]] bool ExistsPath(const std::filesystem::path& path);

	/// @brief Shutdown the TextureManager.
	void Shutdown() noexcept;
}

#endif /*TRAP_TEXTUREMANAGER_H*/