#include "Sandbox2D.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"),
	m_wireFrame(false),
	m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) / static_cast<float>(TRAP::Application::GetWindow()->GetHeight()), true),
	m_frameTimeHistory()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnImGuiRender()
{
	TP_PROFILE_FUNCTION();

	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Performance");
	ImGui::Separator();
	ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
	ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
	//ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
	ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
	ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
	ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
	ImGui::End();

	const TRAP::Graphics::Renderer2D::Statistics stats = TRAP::Graphics::Renderer2D::GetStats();
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::Begin("Renderer2D Stats:", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                                               ImGuiWindowFlags_NoNav);
	ImGui::Text("Renderer2D Stats");
	ImGui::Separator();
	ImGui::Text("DrawCalls: %u", stats.DrawCalls);
	ImGui::Text("Quads: %u", stats.QuadCount);
	ImGui::Text("Vertices: %u", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %u", stats.GetTotalIndexCount());
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnAttach()
{
	TP_PROFILE_FUNCTION();

	TRAP::Application::SetHotShaderReloading(true);
	TRAP::Application::SetHotTextureReloading(true);
	TRAP::Application::GetWindow()->SetTitle("Sandbox2D");

	//Mount & Load Textures
	TRAP::VFS::MountTextures("Assets/Textures");
	TRAP::Graphics::TextureManager::Load("TRAP", "/Textures/TRAPWhiteLogo2048x2048.png");
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnDetach()
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	TP_PROFILE_FUNCTION();

	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

	TRAP::Graphics::Renderer2D::ResetStats();
	{
		TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
		for(float y = -10.0; y < 10.0; y += 0.25f)
		{
			for(float x = -10.0; x < 10.0; x += 0.25f)
			{
				TRAP::Math::Vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				TRAP::Graphics::Renderer2D::DrawQuad({{x, y, 0.0f}, {}, {0.45f, 0.45f, 1.0f}}, color);
			}
		}
		//TRAP::Graphics::Renderer2D::EndScene();

		//TODO
		//TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
		TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, -0.1f}, {}, {0.8f, 0.8f, 1.0f} }, { 0.8f, 0.2f, 0.3f, 1.0f });
		TRAP::Graphics::Renderer2D::DrawQuad({ {0.5f, -0.5f, -0.1f}, {}, {0.5f, 0.75f, 1.0f} }, { 0.2f, 0.3f, 0.8f, 1.0f });
		/*TRAP::Graphics::Renderer2D::DrawQuad({ {0.2f, 0.5f, 0.1f}, {0.0f, 0.0f, TRAP::Application::GetTime() * -50.0f }, {1.0f, 1.0f, 1.0f} },
			{ 0.2f, 0.8f, 0.3f, 1.0f },
			TRAP::Graphics::TextureManager::Get2D("TRAP"));*/
		TRAP::Graphics::Renderer2D::EndScene();
	}

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

//-------------------------------------------------------------------------------------------------------------------//

void Sandbox2D::OnEvent(TRAP::Events::Event& event)
{
	m_cameraController.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool Sandbox2D::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::F10 && event.GetRepeatCount() < 1) //Enable/Disable WireFrame Mode
	{
		m_wireFrame = !m_wireFrame;
		TRAP::Graphics::RenderCommand::SetFillMode(m_wireFrame ? TRAP::Graphics::FillMode::Line :
			                                                     TRAP::Graphics::FillMode::Solid);
	}

	if (event.GetKey() == TRAP::Input::Key::F11 && event.GetRepeatCount() < 1)
		TRAP::Utils::Dialogs::ShowMsgBox("Just a prank bro!", "Critical Error");

	return true;
}