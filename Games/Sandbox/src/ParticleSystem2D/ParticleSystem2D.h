#ifndef _GAMESTRAP_PARTICLESYSTEM2D_H_
#define _GAMESTRAP_PARTICLESYSTEM2D_H_

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
	ParticleSystem2D();

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime);
	void OnRender();

	void Emit(const ParticleProps& particleProps);

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

	std::vector<Particle> m_particlePool;
	uint32_t m_poolIndex = 999;
};

#endif /*_GAMESTRAP_PARTICLESYSTEM2D_H_*/