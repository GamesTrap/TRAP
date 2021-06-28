#ifndef _TRAP_TEXTUREMANAGER_H_
#define _TRAP_TEXTUREMANAGER_H_

#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"

namespace TRAP::Graphics
{
	class TextureManager
	{
	private:
		TextureManager() = default;
		~TextureManager() = default;

	public:
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		TextureManager(TextureManager&&) = delete;
		TextureManager& operator=(TextureManager&&) = delete;

		static const Scope<Texture2D>& Load(const std::string& filepath, TextureUsage usage);
		static const Scope<Texture2D>& Load(const std::string& name, const std::string& filepath, TextureUsage usage);
		static const Scope<Texture2D>& Load(const std::string& name, const Scope<Image>& img, TextureUsage usage);
		static const Scope<TextureCube>& Load(const std::string& name, const std::string& filepath, TextureCubeFormat format, TextureUsage usage);
		static const Scope<TextureCube>& Load(const std::string& filepath, TextureCubeFormat format, TextureUsage usage);
		static const Scope<TextureCube>& Load(const std::string& name, const std::array<std::string, 6>& filepaths, TextureUsage usage);
		static const Scope<TextureCube>& Load(const std::string& name, const Scope<Image>& img, TextureCubeFormat format, TextureUsage usage);

		static void Add(Scope<Texture> texture);
		static void Remove(const Scope<Texture>& texture);
		static void Remove(const std::string& name);
		static const Scope<Texture>& Get(const std::string& name, TextureType textureType);
		static const Scope<Texture2D>& Get2D(const std::string& name);
		static const Scope<TextureCube>& GetCube(const std::string& name);
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const Scope<Texture>& texture);
		static void ReloadAll();

		static void Shutdown();

		static bool Exists(const std::string& name);
		static bool ExistsVirtualPath(std::string_view virtualPath);

		static std::unordered_map<std::string, Scope<Texture>> s_Textures;
	};
}

#endif /*_TRAP_TEXTUREMANAGER_H_*/