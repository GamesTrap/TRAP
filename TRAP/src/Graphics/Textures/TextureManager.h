#ifndef _TRAP_TEXTUREMANAGER_H_
#define _TRAP_TEXTUREMANAGER_H_

#include "Texture.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "Utils/Singleton.h"

namespace TRAP::Graphics
{
	class TextureManager : public Singleton
	{
	public:
		static const Scope<Texture>& Load(const std::string& filepath, TextureParameters parameters = TextureParameters());
		static const Scope<Texture>& Load(const std::string& name, const std::string& filepath, TextureParameters parameters = TextureParameters());
		static const Scope<Texture>& Load(const std::string& name, const std::string& filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static const Scope<Texture>& Load(const std::string& filepath, InputFormat format, TextureParameters parameters = TextureParameters());
		static const Scope<Texture>& Load(const std::string& name, const std::array<std::string, 6> & filepaths, TextureParameters parameters = TextureParameters());
		
		static void Add(Scope<Texture> texture);
		static void Remove(const Scope<Texture>& texture);
		static void Remove(std::string_view name);
		static const Scope<Texture>& Get(const std::string& name, TextureType textureType);
		static const Scope<Texture2D>& Get2D(const std::string& name);
		static const Scope<TextureCube>& GetCube(const std::string& name);
		static void Clean();

		static void Reload(const std::string& nameOrVirtualPath);
		static void Reload(const Scope<Texture>& texture);
		static void ReloadAll();

		static void Shutdown();

		static bool Exists(const std::string& name);
		static bool ExistsVirtualPath(const std::string& virtualPath);
		
	private:
		TextureManager() = default;

		static std::unordered_map<std::string, Scope<Texture>> s_Textures;
	};
}

#endif /*_TRAP_TEXTUREMANAGER_H_*/