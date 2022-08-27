#ifndef TRAP_SPRITEANIMATION_H
#define TRAP_SPRITEANIMATION_H

#include <vector>

#include "Core/Base.h"
#include "Graphics/Textures/SubTexture2D.h"

namespace TRAP::Graphics
{
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
		static TRAP::Ref<SpriteAnimation> Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed);

		/// <summary>
		/// Constructor.
		/// Creates a new sprite animation
		/// </summary>
		/// <param name="name">Name for the animation.</param>
		/// <param name="sprites">Sprites used by the animation.</param>
		/// <param name="speed">Speed of the animation in seconds.</param>
		SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed);
		/// <summary>
		/// Copy Constructor.
		/// </summary>
		SpriteAnimation(const SpriteAnimation&) = default;
		/// <summary>
		/// Move Constructor.
		/// </summary>
		SpriteAnimation(SpriteAnimation&&) = default;
		/// <summary>
		/// Copy Assignment Operator.
		/// </summary>
		SpriteAnimation& operator=(const SpriteAnimation&) = default;
		/// <summary>
		/// Move Assignment Operator.
		/// </summary>
		SpriteAnimation& operator=(SpriteAnimation&&) = default;

		/// <summary>
		/// Destructor.
		/// </summary>
		~SpriteAnimation() = default;

		/// <summary>
		/// Update the sprite animation.
		/// </summary>
		/// <param name="deltaTime">Delta time.</param>
		void OnUpdate(const Utils::TimeStep& deltaTime);

		/// <summary>
		/// Retrieve all sprites used by the animation.
		/// </summary>
		/// <returns>List of sprites.</returns>
		const std::vector<TRAP::Ref<SubTexture2D>>& GetAllSprites() const;

		/// <summary>
		/// Retrieve the current sprite of the animation.
		/// </summary>
		/// <returns>Current sprite.</returns>
		TRAP::Ref<SubTexture2D> GetCurrentSprite();

		/// <summary>
		/// Set the animation speed.
		/// </summary>
		/// <param name="speed">Speed in seconds.</param>
		void SetSpeed(float speed);

		/// <summary>
		/// Retrieve the animation speed.
		/// </summary>
		/// <returns>Animation speed:</returns>
		float GetSpeed() const;

		/// <summary>
		/// Play the animation.
		/// </summary>
		void Play();
		/// <summary>
		/// Pause the animation
		/// </summary>
		void Pause();
		/// <summary>
		/// Stop the animation.
		/// </summary>
		void Stop();

	private:
		std::string m_name;
		std::vector<TRAP::Ref<SubTexture2D>> m_sprites;
		float m_speed;
		float m_currentTime;
		uint64_t m_currentSpriteIndex;
		bool m_stopped;
	};
}

#endif /*TRAP_SPRITEANIMATION_H*/