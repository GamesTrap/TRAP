#include "TRAPPCH.h"
#include "SubTexture2D.h"

#include "Texture.h"

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromCoords(std::string name,
                                                                                       Ref<TRAP::Graphics::Texture> texture,
                                                                                       const TRAP::Math::Vec2& coords,
                                                                                       const TRAP::Math::Vec2& cellSize,
                                                                                       const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(texture != nullptr, "SubTexture2D::CreateFromCoords(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SubTexture2D::CreateFromCoords(): Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        (coords.x * cellSize.x) / static_cast<float>(texture->GetWidth()),
        (coords.y * cellSize.y) / static_cast<float>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        ((coords.x + spriteSize.x) * cellSize.x) / static_cast<float>(texture->GetWidth()),
        ((coords.y + spriteSize.y) * cellSize.y) / static_cast<float>(texture->GetHeight())
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromPixels(std::string name,
                                                                                       Ref<TRAP::Graphics::Texture> texture,
                                                                                       const TRAP::Math::Vec2& pixelPos,
                                                                                       const TRAP::Math::Vec2& pixelSize,
                                                                                       const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

    TRAP_ASSERT(texture != nullptr, "SubTexture2D::CreateFromPixels(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SubTexture2D::CreateFromPixels(): Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        pixelPos.x / static_cast<float>(texture->GetWidth()),
        pixelPos.y / static_cast<float>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        (pixelPos.x + spriteSize.x * pixelSize.x) / static_cast<float>(texture->GetWidth()),
        (pixelPos.y + spriteSize.y * pixelSize.y) / static_cast<float>(texture->GetHeight())
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SubTexture2D::SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture,
                                           const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max) noexcept
    : m_texture(std::move(texture)), m_name(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    m_texCoords[0] = { min.x, max.y };
    m_texCoords[1] = max;
    m_texCoords[2] = { max.x, min.y};
    m_texCoords[3] = min;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::SubTexture2D::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Math::Vec2, 4>& TRAP::Graphics::SubTexture2D::GetTexCoords() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_texCoords;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::SubTexture2D::GetName() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::SubTexture2D::GetName() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_name;
}
