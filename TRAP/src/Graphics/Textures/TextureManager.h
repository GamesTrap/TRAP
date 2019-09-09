#ifndef _TRAP_TEXTUREMANAGER_H_
#define _TRAP_TEXTUREMANAGER_H_

#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"

namespace TRAP::Graphics
{
	class TextureManager
	{
	public:
		static void Add(std::unique_ptr<Texture> texture);
		static const std::unique_ptr<Texture>& Get(const std::string& name, TextureType textureType);
		static const std::unique_ptr<Texture2D>& Get2D(const std::string& name);
		static const std::unique_ptr<TextureCube>& GetCube(const std::string& name);
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const std::unique_ptr<Texture>& texture);
		static void ReloadAll();

		static void Shutdown();

		static bool Exists(const std::string& name);

	private:
		TextureManager() = default;

		static std::unordered_map<std::string, std::unique_ptr<Texture>> s_Textures;
	};
}

#endif /*_TRAP_TEXTUREMANAGER_H_*/