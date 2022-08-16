#include "TRAPPCH.h"
#include "SubTexture2D.h"

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SubTexture2D::CreateFromCoords(TRAP::Graphics::Texture* texture,
                                                                                       const TRAP::Math::Vec2& coords,
                                                                                       const TRAP::Math::Vec2& spriteSize)
{
    TRAP_ASSERT(texture != nullptr, "Texture is nullptr!");
    TRAP_ASSERT(texture->GetType() == TextureType::Texture2D, "Texture is not a 2D texture!");

    const TRAP::Math::Vec2 min
    {
        (coords.x * spriteSize.x) / texture->GetWidth(),
        (coords.y * spriteSize.y) / texture->GetHeight()
    };
    const TRAP::Math::Vec2 max
    {
        ((coords.x + 1) * spriteSize.x) / texture->GetWidth(),
        ((coords.y + 1) * spriteSize.y) / texture->GetHeight()
    };

    return MakeRef<SubTexture2D>(texture, min, max);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SubTexture2D::SubTexture2D(TRAP::Graphics::Texture* texture, const TRAP::Math::Vec2& min,
                                           const TRAP::Math::Vec2& max)
    : m_texture(texture)
{
    m_texCoords[0] = { min.x, max.y };
    m_texCoords[1] = max;
    m_texCoords[2] = { max.x, min.y};
    m_texCoords[3] = min;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Texture* TRAP::Graphics::SubTexture2D::GetTexture() const
{
    return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Math::Vec2, 4>& TRAP::Graphics::SubTexture2D::GetTexCoords() const
{
    return m_texCoords;
}