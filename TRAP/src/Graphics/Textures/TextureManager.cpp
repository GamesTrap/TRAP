#include "TRAPPCH.h"
#include "TextureManager.h"

#include "Texture.h"
#include "FileSystem/FileSystem.h"

std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Texture>> Textures;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(texture)
	{
		if (Exists(texture->GetName()))
		{
			Ref<Texture> tex = std::move(Textures[texture->GetName()]);
			Textures.erase(texture->GetName());
			return tex;
		}

		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if (Exists(name))
	{
		Ref<Texture> tex = std::move(Textures[name]);
		Textures.erase(name);
		return tex;
	}

	TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get(const std::string& name,
                                                                                     const TextureType textureType)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get2D(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::GetCube(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::Texture>>& TRAP::Graphics::TextureManager::GetTextures() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Textures;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Clean()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Textures.clear();

	//Make sure that fallback textures are always available
	Graphics::TextureManager::Add(Graphics::Texture::CreateFallback2D());
	Graphics::TextureManager::Add(Graphics::Texture::CreateFallbackCube());
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Reload(const std::string& nameOrPath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(!TRAP::FileSystem::Exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			auto texture = Textures[nameOrPath];
			if(texture->Reload())
				TP_INFO(Log::TextureManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

			return texture;
		}

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
				for (u32 i = 0; i < texture->GetFilePaths().size(); i++)
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

		TP_WARN(Log::TextureManagerPrefix, "Couldn't find texture: ", std::filesystem::path(nameOrPath), " to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Reload(Ref<Texture> texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	TP_INFO(Log::TextureManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, texture] : Textures)
		Reload(texture);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::TextureManager::Exists(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Textures.contains(name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::TextureManager::ExistsPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	for (const auto& [name, texture] : Textures)
	{
		if (texture->GetType() == TextureType::Texture2D)
		{
			if (!texture->GetFilePath().empty() && FileSystem::IsEquivalent(texture->GetFilePath(), path))
				return true;
		}
		else if(texture->GetType() == TextureType::TextureCube)
		{
			const std::array<std::filesystem::path, 6> imageFilePaths = texture->GetFilePaths();
			for(const auto& filePath : imageFilePaths)
			{
				if (!filePath.empty() && FileSystem::IsEquivalent(filePath, path))
					return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Textures.clear();
}