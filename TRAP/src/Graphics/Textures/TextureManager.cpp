#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture.h"
#include "FS/FS.h"

std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Texture>> Textures;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromFile(filepath, TextureType::Texture2D);
	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                          const std::filesystem::path& filepath)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromFile(name, filepath, TextureType::Texture2D);

	if(texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                          const Image* const img)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromImage(name, img, TextureType::Texture2D);

	if (texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                          const std::filesystem::path& filepath,
															  const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromFile(name, filepath, TextureType::TextureCube, format);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
	                                                          const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromFile(filepath, TextureType::TextureCube, format);

	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                          const std::array<std::filesystem::path, 6>& filepaths)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromFiles(name, filepaths);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                          const Image* const img,
															  const TextureCubeFormat format)
{
	TP_PROFILE_FUNCTION();

	Scope<Texture> texture = Texture::CreateFromImage(name, img, TextureType::TextureCube, format);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(Scope<Texture> texture)
{
	TP_PROFILE_FUNCTION();

	if(texture)
	{
		if (!Exists(texture->GetName()))
			Textures[texture->GetName()] = std::move(texture);
		else
			TP_ERROR(Log::TextureManagerPrefix, "Texture with name: \"", texture->GetName(),
			         "\" already exists! Ignoring new texture");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const Texture* const texture)
{
	TP_PROFILE_FUNCTION();

	if(texture)
	{
		if (Exists(texture->GetName()))
		{
			Scope<Texture> tex = std::move(Textures[texture->GetName()]);
			Textures.erase(texture->GetName());
			return tex;
		}
		else
			TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Scope<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
	{
		Scope<Texture> tex = std::move(Textures[name]);
		Textures.erase(name);
		return tex;
	}
	else
		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Get(const std::string& name,
                                                             const TextureType textureType)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
		if (Textures[name]->GetType() == textureType)
				return Textures[name].get();

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);

	return Get("FallbackCube", TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Scope<TRAP::Graphics::Texture>>& TRAP::Graphics::TextureManager::GetTextures()
{
	return Textures;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	Textures.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Reload(const std::string& nameOrPath)
{
	TP_PROFILE_FUNCTION();

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			const TextureType textureType = Textures[nameOrPath]->GetType();
			std::filesystem::path filePath = Textures[nameOrPath]->GetFilePath();

			if (filePath.empty())
			{
				TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
				return nullptr;
			}

			const std::string name = Textures[nameOrPath]->GetName();

			if (textureType == TextureType::Texture2D)
			{
				Textures[nameOrPath].reset();
				Textures[nameOrPath] = Texture::CreateFromFile(name, filePath, TextureType::Texture2D);
				Textures[nameOrPath]->AwaitLoading();
				TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", nameOrPath, "\"");
				return Textures[nameOrPath].get();
			}
			if (textureType == TextureType::TextureCube)
			{
				if (Textures[nameOrPath])
				{
					const TextureCubeFormat textureFormat = Textures[nameOrPath]->GetCubeFormat();

					std::array<std::filesystem::path, 6> filePaths{};
					for (uint32_t i = 0; i < Textures[nameOrPath]->GetFilePaths().size(); i++)
						filePaths[i] = Textures[nameOrPath]->GetFilePaths()[i];

					Textures[nameOrPath].reset();
					if (textureFormat == TextureCubeFormat::Cross /*||
					    textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
						Textures[nameOrPath] = Texture::CreateFromFile(name, filePath, TextureType::TextureCube, textureFormat);
					else
						Textures[nameOrPath] = Texture::CreateFromFiles(name, filePaths);
					Textures[nameOrPath]->AwaitLoading();

					TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", nameOrPath, "\"");
					return Textures[nameOrPath].get();
				}
			}
			else
				//Shouldn't be reached
				TP_WARN(Log::TextureManagerPrefix, "Unknown texture type: \"", nameOrPath, "\"");
		}
		else
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
	}
	else
	{
		//Path
		for (const auto& [name, texture] : Textures)
			if (texture->GetType() == TextureType::Texture2D)
			{
				if (FS::IsPathEquivalent(nameOrPath, texture->GetFilePath()))
					return Reload(texture.get());
			}
			else if (texture->GetType() == TextureType::TextureCube)
			{
				for (uint32_t i = 0; i < texture->GetFilePaths().size(); i++)
				{
					if (!texture->GetFilePaths()[i].empty())
					{
						if (FS::IsPathEquivalent(nameOrPath, texture->GetFilePaths()[i]))
							return Reload(texture.get());
					}
				}
			}

		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"",
		        std::filesystem::path(nameOrPath).generic_u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::TextureManager::Reload(const Texture* const texture)
{
	TP_PROFILE_FUNCTION();

	if(Exists(texture->GetName()))
	{
		const std::string name = texture->GetName();
		const TextureType textureType = Textures[name]->GetType();
		std::filesystem::path filePath = Textures[name]->GetFilePath();

		if (filePath.empty())
		{
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", name, "\" to reload.");
			return nullptr;
		}

		if (textureType == TextureType::Texture2D)
		{
			Textures[name].reset();
			Textures[name] = Texture::CreateFromFile(name, filePath, TextureType::Texture2D);
			Textures[name]->AwaitLoading();
			TP_INFO(Log::TextureManagerTexture2DPrefix, "Reloaded: \"", name, "\"");
			return Textures[name].get();
		}
		if (textureType == TextureType::TextureCube)
		{
			const TextureCubeFormat textureFormat = texture->GetCubeFormat();

			std::array<std::filesystem::path, 6> filePaths{};
			for (uint32_t i = 0; i < texture->GetFilePaths().size(); i++)
				filePaths[i] = texture->GetFilePaths()[i];

			Textures[name].reset();
			if (textureFormat == TextureCubeFormat::Cross /*||
				textureFormat == TextureCubeFormat::Equirectangular*/) //TODO Add when Equirecangular is implemented
				Textures[name] = Texture::CreateFromFile(name, filePath, TextureType::TextureCube, textureFormat);
			else
				Textures[name] = Texture::CreateFromFiles(name, filePaths);
			Textures[name]->AwaitLoading();

			TP_INFO(Log::TextureManagerTextureCubePrefix, "Reloaded: \"", name, "\"");
			return Textures[name].get();
		}

		TP_WARN(Log::TextureManagerPrefix, "Unknown texture type: \"", name, "\"");
	}
	else
		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", texture->GetName(), "\" to reload.");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::ReloadAll()
{
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::TextureManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, texture] : Textures)
		Reload(texture.get());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown()
{
	TP_PROFILE_FUNCTION();

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::TextureManagerPrefix, "Destroying textures");
#endif
	Clean();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::Exists(const std::string& name)
{
	return Textures.find(name) != Textures.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::ExistsPath(const std::filesystem::path& path)
{
	for (const auto& [name, texture] : Textures)
	{
		if (texture->GetType() == TextureType::Texture2D)
		{
			if (FS::IsPathEquivalent(texture->GetFilePath(), path))
				return true;
		}
		else if(texture->GetType() == TextureType::TextureCube)
		{
			const std::array<std::filesystem::path, 6> imageFilePaths = texture->GetFilePaths();
			for(const auto& filePath : imageFilePaths)
			{
				if (FS::IsPathEquivalent(filePath, path))
					return true;
			}
		}
	}

	return false;
}