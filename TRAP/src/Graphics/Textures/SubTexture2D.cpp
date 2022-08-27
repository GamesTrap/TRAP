#include "TRAPPCH.h"
#include "SubTexture2D.h"

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromCoords(std::string name,
                                                                                       Ref<TRAP::Graphics::Texture> texture,
                                                                                       const TRAP::Math::Vec2& coords,
                                                                                       const TRAP::Math::Vec2& cellSize,
                                                                                       const TRAP::Math::Vec2& spriteSize)
{
    TRAP_ASSERT(texture != nullptr, "Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        (coords.x * cellSize.x) / texture->GetWidth(),
        (coords.y * cellSize.y) / texture->GetHeight()
    };
    const TRAP::Math::Vec2 max
    {
        ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(),
        ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight()
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
    TRAP_ASSERT(texture != nullptr, "Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        pixelPos.x / texture->GetWidth(),
        pixelPos.y / texture->GetHeight()
    };
    const TRAP::Math::Vec2 max
    {
        (pixelPos.x + spriteSize.x * pixelSize.x) / texture->GetWidth(),
        (pixelPos.y + spriteSize.y * pixelSize.y) / texture->GetHeight()
    };

    return MakeRef<SubTexture2D>(std::move(name), std::move(texture), min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SubTexture2D::SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture,
                                           const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max)
    : m_texture(std::move(texture)), m_name(std::move(name))
{
    m_texCoords[0] = { min.x, max.y };
    m_texCoords[1] = max;
    m_texCoords[2] = { max.x, min.y};
    m_texCoords[3] = min;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::SubTexture2D::GetTexture() const
{
    return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Math::Vec2, 4>& TRAP::Graphics::SubTexture2D::GetTexCoords() const
{
    return m_texCoords;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Graphics::SubTexture2D::GetName()
{
    return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::string& TRAP::Graphics::SubTexture2D::GetName() const
{
    return m_name;
}
