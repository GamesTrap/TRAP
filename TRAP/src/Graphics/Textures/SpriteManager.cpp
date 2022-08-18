#include "TRAPPCH.h"
#include "SpriteManager.h"

#include "SubTexture2D.h"
#include "FileSystem/FileSystem.h"

std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::SubTexture2D>> Sprites;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::CreateFromCoords(const std::string& name,
                                                                                        Ref<TRAP::Graphics::Texture> texture,
                                                                                        const TRAP::Math::Vec2& coords,
                                                                                        const TRAP::Math::Vec2& cellSize,
                                                                                        const TRAP::Math::Vec2& spriteSize)
{
	TP_PROFILE_FUNCTION();

    TRAP_ASSERT(texture != nullptr, "Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a 2D texture!");
    TRAP_ASSERT(!name.empty(), "Name is empty!");

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
                                                                                        Ref<TRAP::Graphics::Texture> texture,
                                                                                        const TRAP::Math::Vec2& pixelPos,
                                                                                        const TRAP::Math::Vec2& pixelSize,
                                                                                        const TRAP::Math::Vec2& spriteSize)
{
	TP_PROFILE_FUNCTION();

    TRAP_ASSERT(texture != nullptr, "Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a 2D texture!");
    TRAP_ASSERT(!name.empty(), "Name is empty!");

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
	TP_PROFILE_FUNCTION();

    if(!sprite)
        return;

    if (!Exists(sprite->GetName()))
        Sprites[sprite->GetName()] = std::move(sprite);
    else
        TP_ERROR(Log::SpriteManagerPrefix, "Sprite with name: \"", sprite->GetName(),
                                           "\" already exists! Ignoring new texture");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(Ref<SubTexture2D> sprite)
{
	TP_PROFILE_FUNCTION();

    if(!sprite)
        return nullptr;

    if (Exists(sprite->GetName()))
    {
        Ref<SubTexture2D> spr = std::move(Sprites[sprite->GetName()]);
        Sprites.erase(sprite->GetName());
        return spr;
    }
    else
        TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", sprite->GetName(), "\"!");

    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if (Exists(name))
	{
		Ref<SubTexture2D> spr = std::move(Sprites[name]);
		Sprites.erase(name);
		return spr;
	}
	else
		TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Get(const std::string& name)
{
	TP_PROFILE_FUNCTION();

	if(Exists(name))
	{
        return Sprites[name];
	}

    TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");
    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::unordered_map<std::string, TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteManager::GetSprites()
{
	return Sprites;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteManager::Clean()
{
	Sprites.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Reload(const std::string& nameOrPath)
{
	TP_PROFILE_FUNCTION();

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
		else
			TP_WARN(Log::SpriteManagerPrefix, "Couldn't find sprite: \"", nameOrPath, "\" to reload.");
	}
	else //Path
	{
		for (const auto& [name, sprite] : Sprites)
		{
            if (FileSystem::IsPathEquivalent(nameOrPath, sprite->GetTexture()->GetFilePath()))
            {
                if(sprite->GetTexture()->Reload())
                    TP_INFO(Log::SpriteManagerPrefix, "Reloaded: \"", nameOrPath, "\"");

                return sprite;
            }
		}

		TP_WARN(Log::SpriteManagerPrefix, "Couldn't find sprite: \"",
		        std::filesystem::path(nameOrPath).u8string(), "\" to reload.");
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Reload(Ref<SubTexture2D> sprite)
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

	TP_INFO(Log::SpriteManagerPrefix, "Reloading all may take a while...");
	for (auto& [name, sprite] : Sprites)
		Reload(sprite);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::SpriteManager::Exists(const std::string& name)
{
	return Sprites.find(name) != Sprites.end();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::SpriteManager::ExistsPath(const std::filesystem::path& path)
{
	for (const auto& [name, sprite] : Sprites)
	{
        if (FileSystem::IsPathEquivalent(sprite->GetTexture()->GetFilePath(), path))
            return true;
	}

	return false;
}