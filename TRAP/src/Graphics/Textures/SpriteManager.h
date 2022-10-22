#ifndef TRAP_SPRITEMANAGER_H
#define TRAP_SPRITEMANAGER_H

#include <unordered_map>

#include "Core/Base.h"
#include "Maths/Math.h"

namespace TRAP::Graphics
{
	class SubTexture2D;
	class Texture;
}

namespace TRAP::Graphics::SpriteManager
{
    /// <summary>
    /// Create a sprite from a texture and the given coordinates and sprite size.
    /// </summary>
    /// <param name="name">Name for the sprite.</param>
    /// <param name="texture">Sprite sheet texture.</param>
    /// <param name="coords">The coordinates for the sub texture.</param>
    /// <param name="cellSize">Size the sprite in pixels (16x16 for example).</param>
    /// <param name="spriteSize">Optional: Multiplier for the cell size (allows to use multi cell sprites as a single sprite).</param>
    /// <returns>Created sprite on success, nullptr otherwise.</returns>
    TRAP::Ref<SubTexture2D> CreateFromCoords(const std::string& name,
                                             Ref<TRAP::Graphics::Texture> texture,
                                             const TRAP::Math::Vec2& coords,
                                             const TRAP::Math::Vec2& cellSize,
                                             const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

    /// <summary>
    /// Create a 2D sprite from a texture and the given pixel coordinates and size.
    /// </summary>
    /// <param name="texture">Sprite sheet texture.</param>
    /// <param name="pixelPos">Position of the top-left corner of the sprite.</param>
    /// <param name="pixelSize">Size of the sprite in pixels</param>
    /// <param name="spriteSize">Optional: Multiplied for the pixel size (allows to use multi cell sprites as a single sprite).</param>
    /// <returns>Created sprite  on success, nullptr otherwise.</returns>
    TRAP::Ref<SubTexture2D> CreateFromPixels(const std::string& name,
                                             Ref<TRAP::Graphics::Texture> texture,
                                             const TRAP::Math::Vec2& pixelPos,
                                             const TRAP::Math::Vec2& pixelSize,
                                             const TRAP::Math::Vec2& spriteSize = {1.0f, 1.0f});

	/// <summary>
	/// Add a sprite to the SpriteManager.
	/// </summary>
	/// <param name="sprite">Sprite to add.</param>
	void Add(Ref<SubTexture2D> sprite);
	/// <summary>
	/// Remove a sprite from the SpriteManager.
	/// </summary>
	/// <param name="sprite">Sprite to remove.</param>
	/// <returns>Removed sprite on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<SubTexture2D> Remove(Ref<SubTexture2D> sprite);
	/// <summary>
	/// Remove a sprite from the SpriteManager via its name.
	/// </summary>
	/// <param name="name">Name of sprite to remove.</param>
	/// <returns>Removed sprite on success, nullptr otherwise.</returns>
	[[maybe_unused]] Ref<SubTexture2D> Remove(const std::string& name);
	/// <summary>
	/// Retrieve a sprite from the SpriteManager.
	/// </summary>
	/// <param name="name">Name of sprite to retrieve.</param>
	/// <returns>Sprite, nullptr if not found.</returns>
	Ref<SubTexture2D> Get(const std::string& name);
	/// <summary>
	/// Retrieve all loaded sprites from the SpriteManager.
	/// </summary>
	/// <returns>Map of all loaded sprites.</returns>
	const std::unordered_map<std::string, Ref<SubTexture2D>>& GetSprites();
	/// <summary>
	/// Clear all sprites from the SpriteManager.
	/// </summary>
	void Clean();

	/// <summary>
	/// Reload a sprite via its name or path.
    /// Note: This will also affects other sprites using the same sprite sheet texture.
	/// </summary>
	/// <param name="nameOrPath">Name or path of a sprite.</param>
	/// <returns>
	/// Sprite if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the sprite was not found.
	/// </returns>
	Ref<SubTexture2D> Reload(const std::string& nameOrPath);
	/// <summary>
	/// Reload a sprite.
    /// Note: This will also affects other sprites using the same sprite sheet texture.
	/// </summary>
	/// <param name="sprite">Sprite to reload.</param>
	/// <returns>
	/// Sprite if found (even on unsuccessful reload), nullptr otherwise.
	/// Should only return nullptr if the sprite was not found.
	/// </returns>
	Ref<SubTexture2D> Reload(Ref<SubTexture2D> sprite);
	/// <summary>
	/// Reload all currently loaded sprites.
	/// </summary>
	void ReloadAll();

	/// <summary>
	/// Check whether a sprite exists.
	/// </summary>
	/// <param name="name">Name of sprite to check.</param>
	/// <returns>True if sprite exists, false otherwise.</returns>
	bool Exists(const std::string& name);
	/// <summary>
	/// Check whether a sprite exists by path.
	/// </summary>
	/// <param name="path">Path of sprite to check.</param>
	/// <returns>True if sprite exists, false otherwise.</returns>
	bool ExistsPath(const std::filesystem::path& path);
}

#endif /*TRAP_SPRITEMANAGER_H*/