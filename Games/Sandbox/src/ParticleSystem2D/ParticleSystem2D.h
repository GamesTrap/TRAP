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

	constexpr void OnUpdate(const TRAP::Utils::TimeStep& deltaTime);
	void OnRender();

	void Emit(const ParticleProps& particleProps);

	constexpr void SetMaxParticles(u32 maxParticles);

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

	u32 m_poolIndex = 999u;
	u32 m_maxParticles = 1000u;
	std::vector<Particle> m_particlePool = std::vector<Particle>(m_maxParticles);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ParticleSystem2D::SetMaxParticles(const u32 maxParticles)
{
	if(m_particlePool.size() < maxParticles)
		m_particlePool.resize(maxParticles);

	m_maxParticles = maxParticles;
	m_poolIndex = m_maxParticles - 1;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ParticleSystem2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	for(Particle& particle : m_particlePool)
	{
		if (!particle.Active)
			continue;

		if(particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= deltaTime;
		particle.Position += particle.Velocity * deltaTime.GetSeconds();
		particle.Rotation += 0.01f * deltaTime;
	}
}

#endif /*GAMESTRAP_PARTICLESYSTEM2D_H*/
