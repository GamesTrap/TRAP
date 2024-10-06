#ifndef TRAP_SPRITEANIMATION_H
#define TRAP_SPRITEANIMATION_H

#include <vector>

#include "Core/Base.h"
#include "Utils/Time/TimeStep.h"

namespace TRAP::Utils
{
	class TimeStep;
}

namespace TRAP::Graphics
{
	class SubTexture2D;

	class SpriteAnimation
	{
	public:
		/// @brief Create a sprite animation from a list of sprites.
		/// @param name Name for the animation.
		/// @param sprites Sprites to be used by the animation.
		/// @param speed Speed of the animation in seconds.
		/// @return Created SpriteAnimation.
		[[nodiscard]] static TRAP::Ref<SpriteAnimation> Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, f32 speed);

		/// @brief Constructor.
		/// Creates a new sprite animation
		/// @param name Name for the animation.
		/// @param sprites Sprites used by the animation.
		/// @param speed Speed of the animation in seconds.
		constexpr SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, f32 speed) noexcept;
		/// @brief Copy Constructor.
		constexpr SpriteAnimation(const SpriteAnimation&) noexcept = default;
		/// @brief Move Constructor.
		constexpr SpriteAnimation(SpriteAnimation&&) noexcept = default;
		/// @brief Copy Assignment Operator.
		constexpr SpriteAnimation& operator=(const SpriteAnimation&) noexcept = default;
		/// @brief Move Assignment Operator.
		constexpr SpriteAnimation& operator=(SpriteAnimation&&) noexcept = default;

		/// @brief Destructor.
		constexpr ~SpriteAnimation() = default;

		/// @brief Update the sprite animation.
		/// @param deltaTime Delta time.
		constexpr void OnUpdate(const Utils::TimeStep& deltaTime) noexcept;

		/// @brief Retrieve all sprites used by the animation.
		/// @return List of sprites.
		[[nodiscard]] constexpr const std::vector<TRAP::Ref<SubTexture2D>>& GetAllSprites() const noexcept;

		/// @brief Retrieve the current sprite of the animation.
		/// @return Current sprite.
		[[nodiscard]] TRAP::Ref<SubTexture2D> GetCurrentSprite();

		/// @brief Set the animation speed.
		/// @param speed Speed in seconds.
		constexpr void SetSpeed(f32 speed) noexcept;

		/// @brief Retrieve the animation speed.
		/// @return Animation speed:
		[[nodiscard]] constexpr f32 GetSpeed() const noexcept;

		/// @brief Play the animation.
		constexpr void Play() noexcept;
		/// @brief Pause the animation
		constexpr void Pause() noexcept;
		/// @brief Stop the animation.
		constexpr void Stop() noexcept;

	private:
		std::string m_name;
		std::vector<TRAP::Ref<SubTexture2D>> m_sprites;
		f32 m_speed;
		f32 m_currentTime = 0;
		u64 m_currentSpriteIndex = 0;
		bool m_stopped = false;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::SpriteAnimation::SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, f32 speed) noexcept
	: m_name(std::move(name)), m_sprites(std::move(sprites)), m_speed(speed)
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::SpriteAnimation::OnUpdate(const Utils::TimeStep& deltaTime) noexcept
{
	if (m_stopped)
		return;

	m_currentTime += deltaTime.GetSeconds();

	if (m_currentTime >= m_speed)
	{
		m_currentSpriteIndex = (m_currentSpriteIndex + 1) % m_sprites.size();
		m_currentTime = 0.0f;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteAnimation::GetAllSprites() const noexcept
{
	return m_sprites;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::SpriteAnimation::SetSpeed(const f32 speed) noexcept
{
	m_speed = speed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::SpriteAnimation::GetSpeed() const noexcept
{
	return m_speed;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::SpriteAnimation::Play() noexcept
{
	m_stopped = false;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::SpriteAnimation::Pause() noexcept
{
	m_stopped = true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::Graphics::SpriteAnimation::Stop() noexcept
{
	m_stopped = true;
	m_currentSpriteIndex = 0;
	m_currentTime = 0.0f;
}

#endif /*TRAP_SPRITEANIMATION_H*/
