#ifndef GAMESTRAP_PARTICLESYSTEM2D_H
#define GAMESTRAP_PARTICLESYSTEM2D_H

#include <TRAP.h>

struct ParticleProps
{
	TRAP::Math::Vec2 Position{};
	TRAP::Math::Vec2 Velocity{}, VelocityVariation{};
	TRAP::Math::Vec4 ColorBegin{}, ColorEnd{};
	float SizeBegin{}, SizeEnd{}, SizeVariation{};
	float LifeTime = 1.0f;
};

class ParticleSystem2D
{
public:
	constexpr ParticleSystem2D() = default;

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime);
	void OnRender();

	void Emit(const ParticleProps& particleProps);

	void SetMaxParticles(uint32_t maxParticles);

private:
	struct Particle
	{
		TRAP::Math::Vec2 Position{};
		TRAP::Math::Vec2 Velocity{};
		TRAP::Math::Vec4 ColorBegin{}, ColorEnd{};
		float Rotation = 0.0f;
		float SizeBegin{}, SizeEnd{};

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};

	uint32_t m_poolIndex = 999;
	uint32_t m_maxParticles = 1000;
	std::vector<Particle> m_particlePool = std::vector<Particle>(m_maxParticles);
};

#endif /*GAMESTRAP_PARTICLESYSTEM2D_H*/