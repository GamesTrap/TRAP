#include "TRAPPCH.h"
#include "SpriteAnimation.h"

TRAP::Ref<TRAP::Graphics::SpriteAnimation> TRAP::Graphics::SpriteAnimation::Create(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed)
{
	ZoneScopedC(tracy::Color::Red);

	TRAP_ASSERT(speed >= 0.0f, "Speed must be positive!");
	TRAP_ASSERT(!sprites.empty(), "Missing sprites!");

	if (sprites.empty())
	{
		TP_ERROR(Log::SpriteAnimationPrefix, "No sprites given for the sprite animation!");
		return nullptr;
	}

	return MakeRef<SpriteAnimation>(std::move(name), std::move(sprites), speed);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SpriteAnimation::SpriteAnimation(std::string name, std::vector<TRAP::Ref<SubTexture2D>> sprites, float speed)
	: m_name(std::move(name)), m_sprites(std::move(sprites)), m_speed(speed), m_currentTime(0), m_currentSpriteIndex(0), m_stopped(false)
{
	ZoneScopedC(tracy::Color::Red);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::OnUpdate(const Utils::TimeStep& deltaTime)
{
	ZoneScopedC(tracy::Color::Red);

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

const std::vector<TRAP::Ref<TRAP::Graphics::SubTexture2D>>& TRAP::Graphics::SpriteAnimation::GetAllSprites() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_sprites;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::SubTexture2D> TRAP::Graphics::SpriteAnimation::GetCurrentSprite()
{
	ZoneScopedC(tracy::Color::Red);

	return m_sprites[m_currentSpriteIndex];
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::SetSpeed(const float speed)
{
	ZoneScopedC(tracy::Color::Red);

	m_speed = speed;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::SpriteAnimation::GetSpeed() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_speed;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Play()
{
	ZoneScopedC(tracy::Color::Red);

	m_stopped = false;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Pause()
{
	ZoneScopedC(tracy::Color::Red);

	m_stopped = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::SpriteAnimation::Stop()
{
	ZoneScopedC(tracy::Color::Red);

	m_stopped = true;
	m_currentSpriteIndex = 0;
	m_currentTime = 0.0f;
}