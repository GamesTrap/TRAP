#include "TRAPPCH.h"
#include "SpriteAnimation.h"

#include "Utils/Time/TimeStep.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SpriteAnimation> TRAP::Graphics::SpriteAnimation::Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed)
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

TRAP::Graphics::SpriteAnimation::SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed) noexcept
	: m_name(std::move(name)), m_sprites(std::move(sprites)), m_speed(speed), m_currentTime(0), m_currentSpriteIndex(0), m_stopped(false)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::OnUpdate(const Utils::TimeStep& deltaTime) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

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

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteAnimation::GetCurrentSprite()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_sprites[m_currentSpriteIndex];
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::SetSpeed(const float speed) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_speed = speed;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::SpriteAnimation::GetSpeed() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_speed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Play() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_stopped = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Pause() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_stopped = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Stop() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	m_stopped = true;
	m_currentSpriteIndex = 0;
	m_currentTime = 0.0f;
}