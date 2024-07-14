#include "TRAPPCH.h"
#include "SpriteManager.h"

#include "SubTexture2D.h"
#include "Texture.h"
#include "FileSystem/FileSystem.h"

TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::SubTexture2D>> Sprites;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::CreateFromCoords(const std::string& name,
                                                                                        const Ref<TRAP::Graphics::Texture>& texture,
                                                                                        const TRAP::Math::Vec2& coords,
                                                                                        const TRAP::Math::Vec2& cellSize,
                                                                                        const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(!sprite)
        return;

    if (!Contains(sprite->GetName()))
        Sprites[sprite->GetName()] = std::move(sprite);
    else
        TP_ERROR(Log::SpriteManagerPrefix, "Sprite with name: \"", sprite->GetName(),
                                           "\" already exists! Ignoring new texture");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(const Ref<SubTexture2D>& sprite)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    if(!sprite)
        return nullptr;

    if (Contains(sprite->GetName()))
    {
        Ref<SubTexture2D> spr = std::move(Sprites[sprite->GetName()]);
        Sprites.erase(sprite->GetName());
        return spr;
    }

	TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", sprite->GetName(), "\"!");

    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Remove(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Sprites.find(name); it != Sprites.end())
	{
		Ref<SubTexture2D> spr = it->second;
		Sprites.erase(it);
		return spr;
	}

	TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteManager::Get(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	if(const auto it = Sprites.find(name); it != Sprites.end())
		return it->second;

    TP_ERROR(Log::SpriteManagerPrefix, "Couldn't find sprite with name: \"", name, "\"!");
    return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::UnorderedStringMap<TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteManager::GetSprites() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return Sprites;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteManager::Clean() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	Sprites.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::SpriteManager::Contains(const std::string_view name)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return Sprites.contains(name);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::SpriteManager::ContainsByPath(const std::filesystem::path& path)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	return std::ranges::any_of(Sprites, [&path](const auto& element)
	{
		return !element.second->GetTexture()->GetFilePaths().empty() && FileSystem::IsEquivalent(element.second->GetTexture()->GetFilePaths().front(), path);
	});
}
