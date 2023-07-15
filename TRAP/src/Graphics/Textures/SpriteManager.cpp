#include "TRAPPCH.h"
#include "SpriteManager.h"

#include "SubTexture2D.h"
#include "Texture.h"
#include "FileSystem/FileSystem.h"

std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::SubTexture2D>> Sprites;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::CreateFromCoords(const std::string& name,
                                                                                        const Ref<TRAP::Graphics::Texture>& texture,
                                                                                        const TRAP::Math::Vec2& coords,
                                                                                        const TRAP::Math::Vec2& cellSize,
                                                                                        const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(texture != nullptr, "SpriteManager::CreateFromCoords(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SpriteManager::CreateFromCoords(): Texture is not a 2D texture!");
    TRAP_ASSERT(!name.empty(), "SpriteManager::CreateFromCoords(): Name is empty!");

	Ref<SubTexture2D> sprite = SubTexture2D::CreateFromCoords(name, texture, coords, cellSize, spriteSize);

	if(!sprite || name.empty() || texture->GetType() != TextureType::Texture2D)
    {
        TP_ERROR(Log::SpriteManagerPrefix, "Failed to create sprite from coords!");
        return nullptr;
    }

    Add(std::move(sprite));

    return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::CreateFromPixels(const std::string& name,
                                                                                        const Ref<TRAP::Graphics::Texture>& texture,
                                                                                        const TRAP::Math::Vec2& pixelPos,
                                                                                        const TRAP::Math::Vec2& pixelSize,
                                                                                        const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(texture != nullptr, "SpriteManager::CreateFromPixels(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SpriteManager::CreateFromPixels(): Texture is not a 2D texture!");
    TRAP_ASSERT(!name.empty(), "SpriteManager::CreateFromPixels(): Name is empty!");

	Ref<SubTexture2D> sprite = SubTexture2D::CreateFromPixels(name, texture, pixelPos, pixelSize, spriteSize);

	if(!sprite || name.empty() || texture->GetType() != TextureType::Texture2D)
    {
        TP_ERROR(Log::SpriteManagerPrefix, "Failed to create sprite from coords!");
        return nullptr;
    }

    Add(std::move(sprite));

    return Get(name);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteManager::Add(Ref<SubTexture2D> sprite)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(!sprite)
        return;

    if (!Exists(sprite->GetName()))
        Sprites[sprite->GetName()] = std::move(sprite);
    else
        TP_ERROR(Log::SpriteManagerPrefix, "Sprite with name: \"", sprite->GetName(),
                                           "\" already exists! Ignoring new texture");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(const Ref<SubTexture2D>& sprite)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    if(!sprite)
        return nullptr;

    if (Exists(sprite->GetName()))
    {
        Ref<SubTexture2D> spr = std::move(Sprites[sprite->GetName()]);
        Sprites.erase(sprite->GetName());
        return spr;
    }

	TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", sprite->GetName(), "\"!");

    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if (Exists(name))
	{
		Ref<SubTexture2D> spr = std::move(Sprites[name]);
		Sprites.erase(name);
		return spr;
	}

	TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Get(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(Exists(name))
	{
        return Sprites[name];
	}

    TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");
    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteManager::GetSprites() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return Sprites;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteManager::Clean() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	Sprites.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Reload(const std::string& nameOrPath)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!std::filesystem::exists(nameOrPath))
	{
		//Name
		if(Exists(nameOrPath))
		{
			auto sprite = Sprites[nameOrPath];
			if(sprite->GetTexture()->Reload())
				TP_INFO(Log::SpriteManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

			return sprite;
		}

		TP_WARN(Log::SpriteManagerPrefix, "Couldn't find sprite: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, sprite] : Sprites)
		{
            if (FileSystem::IsEquivalent(nameOrPath, sprite->GetTexture()->GetFilePath()))
            {
                if(sprite->GetTexture()->Reload())
                    TP_INFO(Log::SpriteManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

                return sprite;
            }
		}

		TP_WARN(Log::SpriteManagerPrefix, "Couldn't find sprite: ", std::filesystem::path(nameOrPath), " to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Reload(Ref<SubTexture2D> sprite)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	if(!Exists(sprite->GetName()))
	{
		TP_WARN(Log::SpriteManagerPrefix, "Couldn't find sprite: \"", sprite->GetName(), "\" to reload.");
		return nullptr;
	}

	if(sprite->GetTexture()->Reload())
		TP_INFO(Log::SpriteManagerPrefix, "Reloaded: \"", sprite->GetName(), "\"");

	return sprite;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteManager::ReloadAll()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TP_INFO(Log::SpriteManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, sprite] : Sprites)
		Reload(sprite);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::SpriteManager::Exists(const std::string& name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return Sprites.contains(name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::SpriteManager::ExistsPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	return std::ranges::any_of(Sprites, [&path](const auto& element)
	{
		return FileSystem::IsEquivalent(element.second->GetTexture()->GetFilePath(), path);
	});
}