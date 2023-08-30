#include "TRAPPCH.h"
#include "SubTexture2D.h"

#include "Texture.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromCoords(std::string name,
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
        (coords.x() * cellSize.x()) / NumericCast<float>(texture->GetWidth()),
        (coords.y() * cellSize.y()) / NumericCast<float>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        ((coords.x() + spriteSize.x()) * cellSize.x()) / NumericCast<float>(texture->GetWidth()),
        ((coords.y() + spriteSize.y()) * cellSize.y()) / NumericCast<float>(texture->GetHeight())
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromPixels(std::string name,
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
        pixelPos.x() / NumericCast<float>(texture->GetWidth()),
        pixelPos.y() / NumericCast<float>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        (pixelPos.x() + spriteSize.x() * pixelSize.x()) / NumericCast<float>(texture->GetWidth()),
        (pixelPos.y() + spriteSize.y() * pixelSize.y()) / NumericCast<float>(texture->GetHeight())
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SubTexture2D::SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture,
                                           const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max) noexcept
    : m_texture(std::move(texture)), m_texCoords(), m_name(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    std::get<0>(m_texCoords) = { min.x(), max.y() };
    std::get<1>(m_texCoords) = max;
    std::get<2>(m_texCoords) = { max.x(), min.y()};
    std::get<3>(m_texCoords) = min;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::SubTexture2D::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

    return m_texture;
}
