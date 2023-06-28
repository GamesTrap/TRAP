#ifndef TRAP_SUBTEXTURE2D_H
#define TRAP_SUBTEXTURE2D_H

#include <array>

#include "Maths/Math.h"

namespace TRAP::Graphics
{
    class Texture;

    class SubTexture2D
    {
    public:
        /// <summary>
        /// Create a 2D sub texture from a texture and the given coordinates and sprite size.
        /// </summary>
        /// <param name="name">Name for the sprite.</param>
        /// <param name="texture">Sprite sheet texture.</param>
        /// <param name="coords">The coordinates for the sub texture.</param>
        /// <param name="cellSize">Size the sprite in pixels (16x16 for example).</param>
        /// <param name="spriteSize">Optional: Multiplier for the cell size (allows to use multi cell sprites as a single sprite).</param>
        /// <returns>Created 2D sub texture.</returns>
        [[nodiscard]] static TRAP::Ref<SubTexture2D> CreateFromCoords(std::string name,
                                                                      Ref<TRAP::Graphics::Texture> texture,
                                                                      const TRAP::Math::Vec2& coords,
                                                                      const TRAP::Math::Vec2& cellSize,
                                                                      const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

        /// <summary>
        /// Create a 2D sub texture from a texture and the given pixel coordinates and size.
        /// </summary>
        /// <param name="name">Name for the sprite.</param>
        /// <param name="texture">Sprite sheet texture.</param>
        /// <param name="pixelPos">Position of the top-left corner of the sprite.</param>
        /// <param name="pixelSize">Size of the sprite in pixels</param>
        /// <param name="spriteSize">Optional: Multiplied for the pixel size (allows to use multi cell sprites as a single sprite).</param>
        /// <returns>Created 2D sub texture.</returns>
        [[nodiscard]] static TRAP::Ref<SubTexture2D> CreateFromPixels(std::string name,
                                                                      Ref<TRAP::Graphics::Texture> texture,
                                                                      const TRAP::Math::Vec2& pixelPos,
                                                                      const TRAP::Math::Vec2& pixelSize,
                                                                      const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name">Name for the sprite.</param>
        /// <param name="texture">Sprite sheet to use.</param>
        /// <param name="min">Min texture coordinates to use.</param>
        /// <param name="max">Max texture coordinates to use.</param>
        SubTexture2D(std::string name, Ref<TRAP::Graphics::Texture> texture, const TRAP::Math::Vec2& min, const TRAP::Math::Vec2& max) noexcept;

		/// <summary>
		/// Copy constructor.
		/// </summary>
        SubTexture2D(const SubTexture2D&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
        SubTexture2D(SubTexture2D&&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
        SubTexture2D& operator=(const SubTexture2D&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
        SubTexture2D& operator=(SubTexture2D&&) noexcept = default;
		/// <summary>
		/// Destructor.
		/// </summary>
        ~SubTexture2D() = default;

        /// <summary>
        /// Retrieve the used sprite sheet texture.
        /// </summary>
        /// <returns>Sprite sheet texture.</returns>
        [[nodiscard]] Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

        /// <summary>
        /// Retrieve the texture coordinates for this sub texture.
        /// </summary>
        /// <returns>Texture coordinates.</returns>
        [[nodiscard]] const std::array<TRAP::Math::Vec2, 4>& GetTexCoords() const noexcept;

        /// <summary>
        /// Retrieve the name of the sprite.
        /// </summary>
        /// <returns>Name of the sprite.</returns>
        [[nodiscard]] constexpr std::string GetName() noexcept;
        /// <summary>
        /// Retrieve the name of the sprite.
        /// </summary>
        /// <returns>Name of the sprite.</returns>
        [[nodiscard]] constexpr const std::string& GetName() const noexcept;

    private:
        Ref<TRAP::Graphics::Texture> m_texture;
        std::array<TRAP::Math::Vec2, 4> m_texCoords;
        std::string m_name;
    };
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