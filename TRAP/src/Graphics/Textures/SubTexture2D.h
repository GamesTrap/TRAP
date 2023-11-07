#ifndef TRAP_SUBTEXTURE2D_H
#define TRAP_SUBTEXTURE2D_H

#include <array>

#include "Maths/Types.h"
#include "Maths/Vec2.h"

namespace TRAP::Graphics
{
    class Texture;

    class SubTexture2D
    {
    public:
        /// @brief Create a 2D sub texture from a texture and the given coordinates and sprite size.
        /// @param name Name for the sprite.
        /// @param texture Sprite sheet texture.
        /// @param coords The coordinates for the sub texture.
        /// @param cellSize Size the sprite in pixels (16x16 for example).
        /// @param spriteSize Optional: Multiplier for the cell size (allows to use multi cell sprites as a single sprite).
        /// @return Created 2D sub texture.
        [[nodiscard]] static TRAP::Ref<SubTexture2D> CreateFromCoords(std::string name,
                                                                      Ref<TRAP::Graphics::Texture> texture,
                                                                      const TRAP::Math::Vec2& coords,
                                                                      const TRAP::Math::Vec2& cellSize,
                                                                      const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

        /// @brief Create a 2D sub texture from a texture and the given pixel coordinates and size.
        /// @param name Name for the sprite.
        /// @param texture Sprite sheet texture.
        /// @param pixelPos Position of the top-left corner of the sprite.
        /// @param pixelSize Size of the sprite in pixels
        /// @param spriteSize Optional: Multiplied for the pixel size (allows to use multi cell sprites as a single sprite).
        /// @return Created 2D sub texture.
        [[nodiscard]] static TRAP::Ref<SubTexture2D> CreateFromPixels(std::string name,
                                                                      Ref<TRAP::Graphics::Texture> texture,
                                                                      const TRAP::Math::Vec2& pixelPos,
                                                                      const TRAP::Math::Vec2& pixelSize,
                                                                      const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

        /// @brief Constructor
        /// @param name Name for the sprite.
        /// @param texture Sprite sheet to use.
        /// @param min Min texture coordinates to use.
        /// @param max Max texture coordinates to use.
        SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture, const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max) noexcept;

		/// @brief Copy constructor.
        SubTexture2D(const SubTexture2D&) noexcept = default;
		/// @brief Move constructor.
        SubTexture2D(SubTexture2D&&) noexcept = default;
		/// @brief Copy assignment operator.
        SubTexture2D& operator=(const SubTexture2D&) noexcept = default;
		/// @brief Move assignment operator.
        SubTexture2D& operator=(SubTexture2D&&) noexcept = default;
		/// @brief Destructor.
        ~SubTexture2D() = default;

        /// @brief Retrieve the used sprite sheet texture.
        /// @return Sprite sheet texture.
        [[nodiscard]] Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

        /// @brief Retrieve the texture coordinates for this sub texture.
        /// @return Texture coordinates.
        [[nodiscard]] constexpr const std::array<TRAP::Math::Vec2, 4>& GetTexCoords() const noexcept;

        /// @brief Retrieve the name of the sprite.
        /// @return Name of the sprite.
        [[nodiscard]] constexpr std::string GetName() noexcept;
        /// @brief Retrieve the name of the sprite.
        /// @return Name of the sprite.
        [[nodiscard]] constexpr const std::string& GetName() const noexcept;

    private:
        Ref<TRAP::Graphics::Texture> m_texture;
        std::array<TRAP::Math::Vec2, 4> m_texCoords{};
        std::string m_name;
    };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<TRAP::Math::Vec2, 4>& TRAP::Graphics::SubTexture2D::GetTexCoords() const noexcept
{
    return m_texCoords;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::SubTexture2D::GetName() noexcept
{
    return m_name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::string& TRAP::Graphics::SubTexture2D::GetName() const noexcept
{
    return m_name;
}

#endif /*TRAP_SUBTEXTURE2D_H*/