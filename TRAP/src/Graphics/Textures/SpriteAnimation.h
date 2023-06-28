#ifndef TRAP_SPRITEANIMATION_H
#define TRAP_SPRITEANIMATION_H

#include <vector>

#include "Core/Base.h"

namespace TRAP::Utils
{
	struct TimeStep;
}

namespace TRAP::Graphics
{
	class SubTexture2D;

	class SpriteAnimation
	{
	public:
		/// <summary>
		/// Create a sprite animation from a list of sprites.
		/// </summary>
		/// <param name="name">Name for the animation.</param>
		/// <param name="sprites">Sprites to be used by the animation.</param>
		/// <param name="speed">Speed of the animation in seconds.</param>
		/// <returns>Created SpriteAnimation.</returns>
		[[nodiscard]] static TRAP::Ref<SpriteAnimation> Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed);

		/// <summary>
		/// Constructor.
		/// Creates a new sprite animation
		/// </summary>
		/// <param name="name">Name for the animation.</param>
		/// <param name="sprites">Sprites used by the animation.</param>
		/// <param name="speed">Speed of the animation in seconds.</param>
		constexpr SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed) noexcept;
		/// <summary>
		/// Copy Constructor.
		/// </summary>
		SpriteAnimation(const SpriteAnimation&) noexcept = default;
		/// <summary>
		/// Move Constructor.
		/// </summary>
		SpriteAnimation(SpriteAnimation&&) noexcept = default;
		/// <summary>
		/// Copy Assignment Operator.
		/// </summary>
		SpriteAnimation& operator=(const SpriteAnimation&) noexcept = default;
		/// <summary>
		/// Move Assignment Operator.
		/// </summary>
		SpriteAnimation& operator=(SpriteAnimation&&) noexcept = default;

		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr ~SpriteAnimation() = default;

		/// <summary>
		/// Update the sprite animation.
		/// </summary>
		/// <param name="deltaTime">Delta time.</param>
		void OnUpdate(const Utils::TimeStep& deltaTime) noexcept;

		/// <summary>
		/// Retrieve all sprites used by the animation.
		/// </summary>
		/// <returns>List of sprites.</returns>
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<SubTexture2D>>& GetAllSprites() const noexcept;

		/// <summary>
		/// Retrieve the current sprite of the animation.
		/// </summary>
		/// <returns>Current sprite.</returns>
		[[nodiscard]] TRAP::Ref<SubTexture2D> GetCurrentSprite();

		/// <summary>
		/// Set the animation speed.
		/// </summary>
		/// <param name="speed">Speed in seconds.</param>
		void SetSpeed(float speed) noexcept;

		/// <summary>
		/// Retrieve the animation speed.
		/// </summary>
		/// <returns>Animation speed:</returns>
		[[nodiscard]] float GetSpeed() const noexcept;

		/// <summary>
		/// Play the animation.
		/// </summary>
		void Play() noexcept;
		/// <summary>
		/// Pause the animation
		/// </summary>
		void Pause() noexcept;
		/// <summary>
		/// Stop the animation.
		/// </summary>
		void Stop() noexcept;

	private:
		std::string m_name;
		std::vector<TRAP::Ref<SubTexture2D>> m_sprites;
		float m_speed;
		float m_currentTime = 0;
		uint64_t m_currentSpriteIndex = 0;
		bool m_stopped = false;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::SpriteAnimation::SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed) noexcept
	: m_name(std::move(name)), m_sprites(std::move(sprites)), m_speed(speed)
{
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteAnimation::GetAllSprites() const noexcept
{
	return m_sprites;
}

#endif /*TRAP_SPRITEANIMATION_H*/