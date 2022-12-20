#include "TRAPPCH.h"
#include "TextureManager.h"
#include "Texture.h"
#include "FileSystem/FileSystem.h"

std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Texture>> Textures;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromFile(filepath, TextureType::Texture2D, TextureCubeFormat::NONE,
	                                               flags);
	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::filesystem::path& filepath,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromFile(name, filepath, TextureType::Texture2D,
												   TextureCubeFormat::NONE, flags);

	if(texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const Image* const img,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromImage(name, img, TextureType::Texture2D, TextureCubeFormat::NONE,
													flags);

	if (texture)
	{
		Add(std::move(texture));

		return Get2D(name);
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::filesystem::path& filepath,
															            const TextureCubeFormat format,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromFile(name, filepath, TextureType::TextureCube, format, flags);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
	                                                                    const TextureCubeFormat format,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromFile(filepath, TextureType::TextureCube, format, flags);

	if(texture)
	{
		const std::string name = texture->GetName();

		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::array<std::filesystem::path, 6>& filepaths,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromFiles(name, filepaths, flags);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const Image* const img,
															            const TextureCubeFormat format,
															            const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Ref<Texture> texture = Texture::CreateFromImage(name, img, TextureType::TextureCube, format, flags);

	if(texture)
	{
		Add(std::move(texture));

		return GetCube(name);
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(Ref<Texture> texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(Ref<Texture> texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(texture)
	{
		if (Exists(texture->GetName()))
		{
			Ref<Texture> tex = std::move(Textures[texture->GetName()]);
			Textures.erase(texture->GetName());
			return tex;
		}
		else
			TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (Exists(name))
	{
		Ref<Texture> tex = std::move(Textures[name]);
		Textures.erase(name);
		return tex;
	}
	else
		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get(const std::string& name,
                                                                       const TextureType textureType)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(Exists(name))
	{
		if (Textures[name]->GetType() == textureType)
				return Textures[name];
	}

	if (textureType == TextureType::Texture2D)
		return Get("Fallback2D", TextureType::Texture2D);

	return Get("FallbackCube", TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Texture>>& TRAP::Graphics::TextureManager::GetTextures() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Textures;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Textures.clear();

	//Make sure that fallback textures are always available
	Graphics::TextureManager::Add(Graphics::Texture::CreateFallback2D());
	Graphics::TextureManager::Add(Graphics::Texture::CreateFallbackCube());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Reload(const std::string& nameOrPath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			auto texture = Textures[nameOrPath];
			if(texture->Reload())
				TP_INFO(Log::TextureManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

			return texture;
		}
		else
			TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, texture] : Textures)
		{
			if (texture->GetType() == TextureType::Texture2D)
			{
				if (FileSystem::IsEquivalent(nameOrPath, texture->GetFilePath()))
				{
					if(texture->Reload())
						TP_INFO(Log::TextureManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

					return texture;
				}
			}
			else if (texture->GetType() == TextureType::TextureCube)
			{
				for (uint32_t i = 0; i < texture->GetFilePaths().size(); i++)
				{
					if (texture->GetFilePaths()[i].empty())
						continue;

					if (FileSystem::IsEquivalent(nameOrPath, texture->GetFilePaths()[i]))
					{
						if(texture->Reload())
							TP_INFO(Log::TextureManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

						return texture;
					}
				}
			}
		}

		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"",
		        std::filesystem::path(nameOrPath).u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Reload(Ref<Texture> texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!Exists(texture->GetName()))
	{
		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: \"", texture->GetName(), "\" to reload.");
		return nullptr;
	}

	if(texture->Reload())
		TP_INFO(Log::TextureManagerPrefix, "Reloaded: \"", texture->GetName(), "\"");

	return texture;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::ReloadAll()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TP_INFO(Log::TextureManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, texture] : Textures)
		Reload(texture);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::Exists(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Textures.find(name) != Textures.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::TextureManager::ExistsPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	for (const auto& [name, texture] : Textures)
	{
		if (texture->GetType() == TextureType::Texture2D)
		{
			if (FileSystem::IsEquivalent(texture->GetFilePath(), path))
				return true;
		}
		else if(texture->GetType() == TextureType::TextureCube)
		{
			const std::array<std::filesystem::path, 6> imageFilePaths = texture->GetFilePaths();
			for(const auto& filePath : imageFilePaths)
			{
				if (FileSystem::IsEquivalent(filePath, path))
					return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Textures.clear();
}