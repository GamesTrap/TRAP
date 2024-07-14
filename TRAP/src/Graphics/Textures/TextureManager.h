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
	Ref<Texture> Load(const std::string& name, const Image& img,
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
	/// @brief Load a cube texture from files.
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
	Ref<Texture> Load(const std::string& name, const Image& img,
				      TextureCubeFormat format,
				      TextureCreationFlags flags = TextureCreationFlags::None);
	/// @brief Load a cube texture from 6 TRAP::Images.
	/// @param name Name for the texture.
	/// @param imgs TRAP::Images to use as texture.
	/// Order: +X, -X, +Y, -Y, +Z, -Z.
	/// @param flags Additional flags. Default: None.
	/// @return Loaded texture.
	Ref<Texture> Load(const std::string& name, std::span<const Image*, 6> imgs,
	                  TextureCreationFlags flags = TextureCreationFlags::None);

	/// @brief Add a texture to the TextureManager.
	/// @param texture Texture to add.
	void Add(const Ref<Texture>& texture);
	/// @brief Remove a texture from the TextureManager.
	/// @param texture Texture to remove.
	/// @return Removed texture on success, nullptr otherwise.
	[[maybe_unused]] Ref<Texture>  Remove(const Ref<Texture>& texture);
	/// @brief Remove a texture from the TextureManager via its name.
	/// @param name Name of texture to remove.
	/// @return Removed texture on success, nullptr otherwise.
	[[maybe_unused]] Ref<Texture> Remove(std::string_view name);
	/// @brief Retrieve a texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @param textureType Type of texture.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> Get(std::string_view name, TextureType textureType);
	/// @brief Retrieve a 2D texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> Get2D(std::string_view name);
	/// @brief Retrieve a cube texture from the TextureManager.
	/// @param name Name of texture to retrieve.
	/// @return Texture, Fallback texture if not found.
	[[nodiscard]] Ref<Texture> GetCube(std::string_view name);
	/// @brief Retrieve all textures that use the given path from the TextureManager.
	/// @param path Path to retrieve.
	/// @return Textures, empty vector if none were found.
	[[nodiscard]] std::vector<TRAP::Ref<TRAP::Graphics::Texture>> GetByPath(const std::filesystem::path& path);
	/// @brief Retrieve all loaded textures from the TextureManager.
	/// @return Map of all loaded textures.
	[[nodiscard]] const TRAP::Utils::UnorderedStringMap<Ref<Texture>>& GetTextures() noexcept;
	/// @brief Clear all textures from the TextureManager.
	/// Except fallback textures.
	void Clean();

	/// @brief Check whether a texture is in the TextureManager.
	/// @param name Name of texture to check.
	/// @return True if texture is in TextureManager, false otherwise.
	[[nodiscard]] bool Contains(std::string_view name);
	/// @brief Check whether a texture is in the TextureManager with the given path.
	/// @param path Path to check.
	/// @return True if a texture using the given path was found, false otherwise.
	[[nodiscard]] bool ContainsByPath(const std::filesystem::path& path);

	/// @brief Shutdown the TextureManager.
	void Shutdown() noexcept;
}

#endif /*TRAP_TEXTUREMANAGER_H*/
