#include "Sandbox2D.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"),
	m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), false, true, TRAP::Input::Controller::One),
	m_frameTimeHistory()
{
	//TODO
	//If controller connects switch to it
	//If controller disconnect switch to mouse/keyboard
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnImGuiRender()
{
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Performance");
	ImGui::Separator();
	ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
	ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
	ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
	ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnAttach()
{
	//Mount & Load Textures
	TRAP::VFS::Get()->MountTextures("Assets/Textures");
	TRAP::Graphics::TextureManager::Load("TRAP", "/Textures/TRAPWhiteLogo2048x2048.png");

	TP_DEBUG(TRAP::Input::GetControllerName(TRAP::Input::Controller::One));
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());	
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(-1.0f, 0.0f), TRAP::Math::Vec2(0.8f, 0.8f), TRAP::Math::Vec4(0.8f, 0.2f, 0.3f, 1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(0.5f, -0.5f), TRAP::Math::Vec2(0.5f, 0.75f), TRAP::Math::Vec4(0.2f, 0.3f, 0.8f, 1.0f));
	TRAP::Graphics::Renderer2D::DrawQuad(TRAP::Math::Vec2(0.2f, 0.5f), TRAP::Math::Vec2(0.5f, 0.5f), TRAP::Math::Vec4(0.2f, 0.3f, 0.8f, 1.0f), TRAP::Graphics::TextureManager::Get2D("TRAP"));
	TRAP::Graphics::Renderer2D::EndScene();
	
	//FPS & FrameTime
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

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnEvent(TRAP::Event& event)
{
	m_cameraController.OnEvent(event);
}