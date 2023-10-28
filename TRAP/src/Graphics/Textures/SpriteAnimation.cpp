#include "TRAPPCH.h"
#include "SpriteAnimation.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SpriteAnimation> TRAP::Graphics::SpriteAnimation::Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, f32 speed)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	TRAP_ASSERT(speed >= 0.0f, "SpriteAnimation::Create(): Speed must be positive!");
	TRAP_ASSERT(!sprites.empty(), "SpriteAnimation::Create(): Missing sprites!");

	if (sprites.empty())
	{
		TP_ERROR(Log::SpriteAnimationPrefix, "No sprites given for the sprite animation!");
		return nullptr;
	}

	return MakeRef<SpriteAnimation>(std::move(name), std::move(sprites), speed);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteAnimation::GetCurrentSprite()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_sprites[m_currentSpriteIndex];
}
