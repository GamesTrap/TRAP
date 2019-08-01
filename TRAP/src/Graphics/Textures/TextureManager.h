#ifndef _TRAP_TEXTUREMANAGER_H_
#define _TRAP_TEXTUREMANAGER_H_

#include "Texture.h"

namespace TRAP::Graphics
{
	class TextureManager
	{
	public:
		static void Add(std::unique_ptr<Texture> texture);
		static Texture* Get(const std::string& name);
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const std::unique_ptr<Texture>& texture);
		static void ReloadAll();

		static void Shutdown();

	private:
		TextureManager() = default;

		static std::vector<std::unique_ptr<Texture>> s_textures;
	};
}

#endif /*_TRAP_TEXTUREMANAGER_H_*/