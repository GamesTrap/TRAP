#ifndef TRAP_TEXTUREMANAGER_H
#define TRAP_TEXTUREMANAGER_H

#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"

namespace TRAP::Graphics
{
	class TextureManager
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		TextureManager() = delete;
		/// <summary>
		/// Destructor.
		/// </summary>
		~TextureManager() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		TextureManager(const TextureManager&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		TextureManager& operator=(const TextureManager&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		TextureManager(TextureManager&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		TextureManager& operator=(TextureManager&&) = delete;

		/// <summary>
		/// Load a 2D texture from file.
		/// File name will be used as the texture name.
		/// </summary>
		/// <param name="filepath">File path of texture to load.</param>
		/// <returns>Loaded texture.</returns>
		static Texture2D* Load(const std::filesystem::path& filepath);
		/// <summary>
		/// Load a 2D texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of texture to load.</param>
		/// <returns>Loaded texture.</returns>
		static Texture2D* Load(const std::string& name, const std::filesystem::path& filepath);
		/// <summary>
		/// Load a 2D texture from TRAP::Image.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="img">TRAP::Image to use as texture.</param>
		/// <returns>Loaded texture.</returns>
		static Texture2D* Load(const std::string& name, const Image* const img);
		/// <summary>
		/// Load a cube texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepath">File path of texture to load.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture.</returns>
		static TextureCube* Load(const std::string& name, const std::filesystem::path& filepath,
		                         TextureCubeFormat format);
		/// <summary>
		/// Load a cube texture from file.
		/// File name will be used as the texture name.
		/// </summary>
		/// <param name="filepath">File path of texture to load.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture.</returns>
		static TextureCube* Load(const std::filesystem::path& filepath, TextureCubeFormat format);
		/// <summary>
		/// Load a cube texture from file.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="filepaths">
		/// File paths of all 6 textures to load.
		/// Order: +X, -X, +Y, -Y, +Z, -Z.
		/// </param>
		/// <returns>Loaded texture.</returns>
		static TextureCube* Load(const std::string& name, const std::array<std::filesystem::path, 6>& filepaths);
		/// <summary>
		/// Load a cube texture from TRAP::Image.
		/// </summary>
		/// <param name="name">Name for the texture.</param>
		/// <param name="img">TRAP::Image to use as texture.</param>
		/// <param name="format">Format of the cube texture.</param>
		/// <returns>Loaded texture.</returns>
		static TextureCube* Load(const std::string& name, const Image* const img,
		                         TextureCubeFormat format);

		/// <summary>
		/// Add a texture to the TextureManager.
		/// </summary>
		/// <param name="texture">Texture to add.</param>
		static void Add(Scope<Texture> texture);
		/// <summary>
		/// Remove a texture from the TextureManager.
		/// </summary>
		/// <param name="texture">Texture to remove.</param>
		static void Remove(const Texture* const texture);
		/// <summary>
		/// Remove a texture from the TextureManager via its name.
		/// </summary>
		/// <param name="name">Name of texture to remove.</param>
		static void Remove(const std::string& name);
		/// <summary>
		/// Retrieve a texture from the TextureManager.
		/// </summary>
		/// <param name="name">Name of texture to retrieve.</param>
		/// <param name="textureType">Type of texture.</param>
		/// <returns>Texture, Fallback texture if not found.</returns>
		static Texture* Get(const std::string& name, TextureType textureType);
		/// <summary>
		/// Retrieve a 2D texture from the TextureManager.
		/// </summary>
		/// <param name="name">Name of texture to retrieve.</param>
		/// <returns>Texture, Fallback texture if not found.</returns>
		static Texture2D* Get2D(const std::string& name);
		/// <summary>
		/// Retrieve a cube texture from the TextureManager.
		/// </summary>
		/// <param name="name">Name of texture to retrieve.</param>
		/// <returns>Texture, Fallback texture if not found.</returns>
		static TextureCube* GetCube(const std::string& name);
		/// <summary>
		/// Retrieve all loaded textures from the TextureManager.
		/// </summary>
		/// <returns>Map of all loaded textures.</returns>
		static const std::unordered_map<std::string, Scope<Texture>>& GetTextures();
		/// <summary>
		/// Clear all textures from the TextureManager.
		/// </summary>
		static void Clean();

		/// <summary>
		/// Reload a texture via its name or path.
		/// </summary>
		/// <param name="nameOrPath">Name or path of a texture.</param>
		/// <returns>Pointer to reloaded texture on success, nullptr otherwise.</returns>
		static Texture* Reload(const std::string& nameOrPath);
		/// <summary>
		/// Reload a texture.
		/// </summary>
		/// <param name="texture">Texture to reload.</param>
		/// <returns>Pointer to reloaded texture on success, nullptr otherwise.</returns>
		static Texture* Reload(const Texture* const texture);
		/// <summary>
		/// Reload all currently loaded textures.
		/// </summary>
		static void ReloadAll();

		/// <summary>
		/// Shutdown TextureManager
		/// </summary>
		static void Shutdown();

		/// <summary>
		/// Check whether a texture exists.
		/// </summary>
		/// <param name="name">Name of texture to check.</param>
		/// <returns>True if texture exists, false otherwise.</returns>
		static bool Exists(const std::string& name);
		/// <summary>
		/// Check whether a texture exists by path.
		/// </summary>
		/// <param name="path">Path of texture to check.</param>
		/// <returns>True if texture exists, false otherwise.</returns>
		static bool ExistsPath(const std::filesystem::path& path);

		static std::unordered_map<std::string, Scope<Texture>> s_Textures;
	};
}

#endif /*TRAP_TEXTUREMANAGER_H*/