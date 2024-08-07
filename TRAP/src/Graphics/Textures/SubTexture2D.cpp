#include "TRAPPCH.h"
#include "SubTexture2D.h"

#include "Texture.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromCoords(std::string name,
                                                                                                     Ref<TRAP::Graphics::Texture> texture,
                                                                                                     const TRAP::Math::Vec2& coords,
                                                                                                     const TRAP::Math::Vec2& cellSize,
                                                                                                     const TRAP::Math::Vec2& spriteSize)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    TRAP_ASSERT(texture != nullptr, "SubTexture2D::CreateFromCoords(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SubTexture2D::CreateFromCoords(): Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        (coords.x() * cellSize.x()) / NumericCast<f32>(texture->GetWidth()),
        (coords.y() * cellSize.y()) / NumericCast<f32>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        ((coords.x() + spriteSize.x()) * cellSize.x()) / NumericCast<f32>(texture->GetWidth()),
        ((coords.y() + spriteSize.y()) * cellSize.y()) / NumericCast<f32>(texture->GetHeight())
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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

    TRAP_ASSERT(texture != nullptr, "SubTexture2D::CreateFromPixels(): Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "SubTexture2D::CreateFromPixels(): Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        pixelPos.x() / NumericCast<f32>(texture->GetWidth()),
        pixelPos.y() / NumericCast<f32>(texture->GetHeight())
    };
    const TRAP::Math::Vec2 max
    {
        (pixelPos.x() + spriteSize.x() * pixelSize.x()) / NumericCast<f32>(texture->GetWidth()),
        (pixelPos.y() + spriteSize.y() * pixelSize.y()) / NumericCast<f32>(texture->GetHeight())
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SubTexture2D::SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture,
                                           const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max) noexcept
    : m_texture(std::move(texture)), m_texCoords({{min.x(), max.y()}, max, {max.x(), min.y()}, min}),
      m_name(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::SubTexture2D::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
                                           (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

    return m_texture;
}
