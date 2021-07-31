#include "ParticleSystem2D.h"

// ParticleSystem2D::ParticleSystem2D()
// {
// 	m_particlePool.resize(1000);
// }

// //-------------------------------------------------------------------------------------------------------------------//

// void ParticleSystem2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
// {
// 	for(Particle& particle : m_particlePool)
// 	{
// 		if (!particle.Active)
// 			continue;

// 		if(particle.LifeRemaining <= 0.0f)
// 		{
// 			particle.Active = false;
// 			continue;
// 		}

// 		particle.LifeRemaining -= deltaTime;
// 		particle.Position += particle.Velocity * static_cast<float>(deltaTime);
// 		particle.Rotation += 0.01f * deltaTime;
// 	}
// }

// //-------------------------------------------------------------------------------------------------------------------//

// void ParticleSystem2D::OnRender()
// {
// 	for(Particle& particle : m_particlePool)
// 	{
// 		if (!particle.Active)
// 			continue;

// 		//Fade away particles
// 		const float life = particle.LifeRemaining / particle.LifeTime;
// 		TRAP::Math::Vec4 color = Lerp(particle.ColorEnd, particle.ColorBegin, life);

// 		const float size = TRAP::Math::Lerp(particle.SizeEnd, particle.SizeBegin, life);

// 		//Render
// 		TRAP::Graphics::Renderer2D::DrawQuad({ {particle.Position.x, particle.Position.y, 0.0f}, {particle.Rotation, 0.0f, 0.0f}, {size, size, 1.0f} }, color);
// 	}
// }

// //-------------------------------------------------------------------------------------------------------------------//

// void ParticleSystem2D::Emit(const ParticleProps& particleProps)
// {
// 	Particle& particle = m_particlePool[m_poolIndex];
// 	particle.Active = true;
// 	particle.Position = particleProps.Position;
// 	particle.Rotation = TRAP::Utils::Random::Get<float>(0.0f, 1.0f) * 2.0f * TRAP::Math::PI<float>();

// 	//Velocity
// 	particle.Velocity = particleProps.Velocity;
// 	particle.Velocity.x += particleProps.VelocityVariation.x * (TRAP::Utils::Random::Get<float>(0.0f, 1.0f) - 0.5f);
// 	particle.Velocity.y += particleProps.VelocityVariation.y * (TRAP::Utils::Random::Get<float>(0.0f, 1.0f) - 0.5f);

// 	//Color
// 	particle.ColorBegin = particleProps.ColorBegin;
// 	particle.ColorEnd = particleProps.ColorEnd;

// 	particle.LifeTime = particleProps.LifeTime;
// 	particle.LifeRemaining = particleProps.LifeTime;
// 	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (TRAP::Utils::Random::Get<float>(0.0f, 1.0f) - 0.5f);
// 	particle.SizeEnd = particleProps.SIzeEnd;

// 	m_poolIndex = --m_poolIndex % m_particlePool.size();
// }