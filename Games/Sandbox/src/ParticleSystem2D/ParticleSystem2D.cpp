#include "ParticleSystem2D.h"

void ParticleSystem2D::OnRender()
{
	for(Particle& particle : m_particlePool)
	{
		if (!particle.Active)
			continue;

		//Fade away particles
		const f32 life = particle.LifeRemaining / particle.LifeTime;
		const TRAP::Math::Vec4 color = Lerp(particle.ColorEnd, particle.ColorBegin, life);

		const f32 size = TRAP::Math::Lerp(particle.SizeEnd, particle.SizeBegin, life);

		//Render
		TRAP::Graphics::Renderer2D::DrawQuad({ .Position={particle.Position.x(), particle.Position.y(), 0.0f}, .Rotation={particle.Rotation, 0.0f, 0.0f}, .Scale={size, size, 1.0f} }, color);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void ParticleSystem2D::Emit(const ParticleProps& particleProps)
{
	Particle& particle = m_particlePool[m_poolIndex];
	particle.Active = true;
	particle.Position = particleProps.Position;
	particle.Rotation = TRAP::Utils::Random::Get<f32>(0.0f, 1.0f) * 2.0f * TRAP::Math::PI<f32>();

	//Velocity
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x() += particleProps.VelocityVariation.x() * (TRAP::Utils::Random::Get<f32>(0.0f, 1.0f) - 0.5f);
	particle.Velocity.y() += particleProps.VelocityVariation.y() * (TRAP::Utils::Random::Get<f32>(0.0f, 1.0f) - 0.5f);

	//Color
	particle.ColorBegin = particleProps.ColorBegin;
	particle.ColorEnd = particleProps.ColorEnd;

	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (TRAP::Utils::Random::Get<f32>(0.0f, 1.0f) - 0.5f);
	particle.SizeEnd = particleProps.SizeEnd;

	m_poolIndex = (m_poolIndex - 1) % m_maxParticles;
}
