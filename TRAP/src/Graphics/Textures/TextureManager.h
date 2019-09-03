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
		static Texture* Get(const std::string& name, TextureType textureType);
		static Texture2D* Get2D(const std::string& name);
		static TextureCube* GetCube(const std::string& name);
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

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::Texture2D* TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	return dynamic_cast<Texture2D*>(Get(name, TextureType::Texture2D));
}

//-------------------------------------------------------------------------------------------------------------------//

inline TRAP::Graphics::TextureCube* TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	return dynamic_cast<TextureCube*>(Get(name, TextureType::TextureCube));
}

#endif /*_TRAP_TEXTUREMANAGER_H_*/