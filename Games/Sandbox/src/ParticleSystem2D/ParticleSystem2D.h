#ifndef GAMESTRAP_PARTICLESYSTEM2D_H
#define GAMESTRAP_PARTICLESYSTEM2D_H

#include <TRAP.h>

struct ParticleProps
{
	TRAP::Math::Vec2 Position{};
	TRAP::Math::Vec2 Velocity{}, VelocityVariation{};
	TRAP::Math::Vec4 ColorBegin{}, ColorEnd{};
	f32 SizeBegin{}, SizeEnd{}, SizeVariation{};
	f32 LifeTime = 1.0f;
};

class ParticleSystem2D
{
public:
	constexpr ParticleSystem2D() = default;

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime);
	void OnRender();

	void Emit(const ParticleProps& particleProps);

	void SetMaxParticles(u32 maxParticles);

private:
	struct Particle
	{
		TRAP::Math::Vec2 Position{};
		TRAP::Math::Vec2 Velocity{};
		TRAP::Math::Vec4 ColorBegin{}, ColorEnd{};
		f32 Rotation = 0.0f;
		f32 SizeBegin{}, SizeEnd{};

		f32 LifeTime = 1.0f;
		f32 LifeRemaining = 0.0f;

		bool Active = false;
	};

	u32 m_poolIndex = 999;
	u32 m_maxParticles = 1000;
	std::vector<Particle> m_particlePool = std::vector<Particle>(m_maxParticles);
};

#endif /*GAMESTRAP_PARTICLESYSTEM2D_H*/