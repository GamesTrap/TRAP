#ifndef TRAP_SPRITEMANAGER_H
#define TRAP_SPRITEMANAGER_H

#include <unordered_map>

#include "Core/Base.h"
#include "Maths/Types.h"
#include "Maths/Vec2.h"

namespace TRAP::Graphics
{
	class SubTexture2D;
	class Texture;
}

namespace TRAP::Graphics::SpriteManager
{
    /// @brief Create a sprite from a texture and the given coordinates and sprite size.
    /// @param name Name for the sprite.
    /// @param texture Sprite sheet texture.
    /// @param coords The coordinates for the sub texture.
    /// @param cellSize Size the sprite in pixels (16x16 for example).
    /// @param spriteSize Optional: Multiplier for the cell size (allows to use multi cell sprites as a single sprite).
    /// @return Created sprite on success, nullptr otherwise.
    TRAP::Ref<SubTexture2D> CreateFromCoords(const std::string& name,
                                             const Ref<TRAP::Graphics::Texture>& texture,
                                             const TRAP::Math::Vec2& coords,
                                             const TRAP::Math::Vec2& cellSize,
                                             const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

    /// @brief Create a 2D sprite from a texture and the given pixel coordinates and size.
    /// @param name Name for the sprite.
    /// @param texture Sprite sheet texture.
    /// @param pixelPos Position of the top-left corner of the sprite.
    /// @param pixelSize Size of the sprite in pixels
    /// @param spriteSize Optional: Multiplied for the pixel size (allows to use multi cell sprites as a single sprite).
    /// @return Created sprite  on success, nullptr otherwise.
    TRAP::Ref<SubTexture2D> CreateFromPixels(const std::string& name,
                                             const Ref<TRAP::Graphics::Texture>& texture,
                                             const TRAP::Math::Vec2& pixelPos,
                                             const TRAP::Math::Vec2& pixelSize,
                                             const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

	/// @brief Add a sprite to the SpriteManager.
	/// @param sprite Sprite to add.
	void Add(Ref<SubTexture2D> sprite);
	/// @brief Remove a sprite from the SpriteManager.
	/// @param sprite Sprite to remove.
	/// @return Removed sprite on success, nullptr otherwise.
	[[maybe_unused]] Ref<SubTexture2D> Remove(const Ref<SubTexture2D>& sprite);
	/// @brief Remove a sprite from the SpriteManager via its name.
	/// @param name Name of sprite to remove.
	/// @return Removed sprite on success, nullptr otherwise.
	[[maybe_unused]] Ref<SubTexture2D> Remove(std::string_view name);
	/// @brief Retrieve a sprite from the SpriteManager.
	/// @param name Name of sprite to retrieve.
	/// @return Sprite, nullptr if not found.
	[[nodiscard]] Ref<SubTexture2D> Get(std::string_view name);
	/// @brief Retrieve all loaded sprites from the SpriteManager.
	/// @return Map of all loaded sprites.
	[[nodiscard]] const TRAP::Utils::UnorderedStringMap<Ref<SubTexture2D>>& GetSprites() noexcept;
	/// @brief Clear all sprites from the SpriteManager.
	void Clean() noexcept;

	/// @brief Check whether a sprite exists.
	/// @param name Name of sprite to check.
	/// @return True if sprite exists, false otherwise.
	[[nodiscard]] bool Contains(std::string_view name);
	/// @brief Check whether a sprite exists by path.
	/// @param path Path of sprite to check.
	/// @return True if sprite exists, false otherwise.
	[[nodiscard]] bool ContainsByPath(const std::filesystem::path& path);
}

#endif /*TRAP_SPRITEMANAGER_H*/
