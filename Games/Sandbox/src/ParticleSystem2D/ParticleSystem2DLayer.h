#ifndef GAMESTRAP_PARTICLESYSTEM2DLAYER_H
#define GAMESTRAP_PARTICLESYSTEM2DLAYER_H

#include <TRAP.h>

#include "ParticleSystem2D.h"

class ParticleSystem2DLayer : public TRAP::Layer
{
public:
	ParticleSystem2DLayer()
		: Layer("ParticleSystem2D"),
		  m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight())),
		  m_particle(),
		  m_maxParticles(1000),
		  m_frameTimeHistory()
	{
	}

	void OnAttach() override
	{
		TRAP::Application::GetWindow()->SetTitle("ParticleSystem2D");

		TRAP::Graphics::RenderCommand::SetDepthTesting(false);
		TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha, TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);

		//Init here
		m_particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
		m_particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
		m_particle.SizeBegin = 0.5f, m_particle.SizeVariation = 0.3f, m_particle.SizeEnd = 0.0f;
		m_particle.LifeTime = 1.0f;
		m_particle.Velocity = { 0.0f, 0.0f };
		m_particle.VelocityVariation = { 3.0f, 1.0f };
		m_particle.Position = { 0.0f, 0.0f };
	}

	void OnEvent(TRAP::Events::Event& event) override
	{
		m_cameraController.OnEvent(event);
	}

	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override
	{
		m_cameraController.OnUpdate(deltaTime);

		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color);

		if(TRAP::Input::IsMouseButtonPressed(TRAP::Input::MouseButton::Left))
		{
			const TRAP::Math::Vec2 mousePosition = TRAP::Input::GetMousePosition();
			const TRAP::Math::Vec2ui resolution = TRAP::Application::GetWindow()->GetSize();

			const TRAP::Graphics::OrthographicCameraBounds bounds = m_cameraController.GetBounds();
			const TRAP::Math::Vec3 pos = m_cameraController.GetCamera().GetPosition();

			const float x = (mousePosition.x / resolution.x) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			const float y = bounds.GetHeight() * 0.5f - (mousePosition.y / resolution.y) * bounds.GetHeight();
			m_particle.Position = { x + pos.x, y + pos.y };
			for (uint32_t i = 0; i < 5; i++)
				m_particleSystem.Emit(m_particle);
		}

		m_particleSystem.OnUpdate(deltaTime);

		TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
		m_particleSystem.OnRender();
		TRAP::Graphics::Renderer2D::EndScene();

		//Update FPS & FrameTime history
		if (m_updateFPSTimer.Elapsed() >= 0.025f)
		{
			m_updateFPSTimer.Reset();
			static int frameTimeIndex = 0;
			if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
			{
				m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::Renderer::GetFrameTime();
				frameTimeIndex++;
			}
			else
			{
				std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
				m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::Renderer::GetFrameTime();
			}
		}
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::ColorEdit4("Spawn Color", &m_particle.ColorBegin[0]);
		ImGui::ColorEdit4("Decay Color", &m_particle.ColorEnd[0]);
		ImGui::DragFloat("Life Time", &m_particle.LifeTime, 0.1f, 0.0f, 1000.0f);
		if(ImGui::SliderInt("Max Particles", &m_maxParticles, 1, 100000))
			m_particleSystem.SetMaxParticles(m_maxParticles);
		ImGui::End();

		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Performance");
		ImGui::Separator();
		//ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
		ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
		ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
		ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
		ImGui::End();
	}

private:
	TRAP::Graphics::OrthographicCameraController m_cameraController;
	ParticleProps m_particle;
	ParticleSystem2D m_particleSystem;
	int32_t m_maxParticles;

	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_updateFPSTimer;
};

#endif /*GAMESTRAP_PARTICLESYSTEM2DLAYER_H*/