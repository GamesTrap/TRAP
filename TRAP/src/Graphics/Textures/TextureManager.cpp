#include "TRAPPCH.h"
#include "TextureManager.h"

#include "Texture.h"
#include "FileSystem/FileSystem.h"

namespace
{
	TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::Texture>> Textures;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::Create2D(filepath.filename().generic_string(), filepath, flags);
	if(texture)
	{
		Add(texture);

		return texture;
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::filesystem::path& filepath,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::Create2D(name, filepath, flags);

	if(texture)
	{
		Add(texture);

		return texture;
	}

	return Get2D("Fallback2D");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const Image& img,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::Create2D(name, img, flags);

	if (texture)
	{
		Add(texture);

		return texture;
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

	Ref<Texture> texture = Texture::CreateCube(name, filepath, format, flags);

	if(texture)
	{
		Add(texture);

		return texture;
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::filesystem::path& filepath,
	                                                                    const TextureCubeFormat format,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::CreateCube(filepath.filename().generic_string(), filepath, format, flags);

	if(texture)
	{
		Add(texture);

		return texture;
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::array<std::filesystem::path, 6>& filepaths,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::CreateCube(name, filepaths, flags);

	if(texture)
	{
		Add(texture);

		return texture;
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const Image& img,
															            const TextureCubeFormat format,
															            const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::CreateCube(name, img, format, flags);

	if(texture)
	{
		Add(texture);

		return texture;
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Load(const std::string& name,
	                                                                    const std::span<const Image*, 6> imgs,
                                                                        const TextureCreationFlags flags)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Ref<Texture> texture = Texture::CreateCube(name, imgs, flags);

	if (texture)
	{
		Add(texture);

		return texture;
	}

	return GetCube("FallbackCube");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Add(const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(texture == nullptr)
	{
		TRAP_ASSERT(false, "TextureManager::Add(): Provided texture is nullptr!");
		return;
	}

	if (!Contains(texture->GetName()))
		Textures[texture->GetName()] = texture;
	else
		TP_ERROR(Log::TextureManagerPrefix, "Texture with name: \"", texture->GetName(),
					"\" already exists! Ignoring new texture");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const Ref<Texture>& texture)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(texture == nullptr)
	{
		TRAP_ASSERT(false, "TextureManager::Remove(): Provided texture is nullptr!");
		return nullptr;
	}

	if(Textures.erase(texture->GetName()) > 0)
		return texture;

	TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", texture->GetName(), "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Remove(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Textures.find(name); it != Textures.end())
	{
		Ref<Texture> tex = it->second;
		Textures.erase(it);
		return tex;
	}

	TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get(const std::string_view name,
                                                                                     const TextureType textureType)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Textures.find(name); it != Textures.end())
		return it->second;

	TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with name: ", name, "!");
	TP_WARN(Log::TextureManagerPrefix, "Using fallback texture!");

	switch(textureType)
	{
	case TextureType::Texture2D:
		return Get("Fallback2D", TextureType::Texture2D);

	case TextureType::TextureCube:
		return Get("FallbackCube", TextureType::TextureCube);
	}

	TRAP_ASSERT(false, "TextureManager::Get(): Failed to retrieve Fallback Texture, unknown TextureType provided!");
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::Get2D(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Get(name, TextureType::Texture2D);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::TextureManager::GetCube(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Get(name, TextureType::TextureCube);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Ref<TRAP::Graphics::Texture>> TRAP::Graphics::TextureManager::GetByPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	std::vector<Ref<Texture>> foundTextures{};

	for(const auto& [name, texture] : Textures)
	{
		switch(texture->GetType())
		{
		case TextureType::Texture2D:
		{
			if(!texture->GetFilePaths().empty() && FileSystem::IsEquivalent(path, texture->GetFilePaths().front()))
				foundTextures.push_back(texture);
			break;
		}

		case TextureType::TextureCube:
		{
			for(const auto& texPath : texture->GetFilePaths())
			{
				if(texPath.empty())
					continue;

				if(FileSystem::IsEquivalent(path, texPath))
				{
					foundTextures.push_back(texture);
					break;
				}
			}
			break;
		}
		}
	}

	if(foundTextures.empty())
		TP_ERROR(Log::TextureManagerPrefix, "Couldn't find texture with path: ", path, "!");

	return foundTextures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::Texture>>& TRAP::Graphics::TextureManager::GetTextures() noexcept
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

[[nodiscard]] bool TRAP::Graphics::TextureManager::Contains(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Textures.contains(name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::TextureManager::ContainsByPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	const auto it = std::ranges::find_if(Textures, [&path](const auto& e)
	{
		const auto& [name, texture] = e;

		switch(texture->GetType())
		{
		case TextureType::Texture2D:
		{
			if(!texture->GetFilePaths().empty() && FileSystem::IsEquivalent(texture->GetFilePaths().front(), path))
				return true;
			break;
		}

		case TextureType::TextureCube:
		{
			for(const auto& filePath : texture->GetFilePaths())
			{
				if (!filePath.empty() && FileSystem::IsEquivalent(filePath, path))
					return true;
			}
		}
		}

		return false;
	});

	return it != Textures.end();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::TextureManager::Shutdown() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Textures.clear();
}
