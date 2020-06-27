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
	ImGui::Begin("Performance", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Performance");
	ImGui::Separator();
	ImGui::Text("GPU: %s", TRAP::Graphics::API::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
	ImGui::Text("DrawCalls: %u", TRAP::Graphics::Renderer::GetDrawCalls());
	ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
	ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
	ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0, 33, ImVec2(200, 50));
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
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);
	TRAP::Graphics::RenderCommand::SetCull(false);

	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.8f, 0.8f, 1.0f} }, { 0.8f, 0.2f, 0.3f, 1.0f });
	TRAP::Graphics::Renderer2D::DrawTriangle({ {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.5f, 0.75f, 1.0f} }, { 0.2f, 0.3f, 0.8f, 1.0f });
	TRAP::Graphics::Renderer2D::DrawQuad({ {0.2f, 0.5f, 0.0f}, {TRAP::Application::GetTime() * 25.0f, TRAP::Application::GetTime() * 50.0f, 0.0f }, {0.5f, 0.5f, 1.0f} },
	                                     {0.2f, 0.3f, 0.8f, 1.0f},
	                                     TRAP::Graphics::TextureManager::Get2D("TRAP"));
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
	if (event.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	if (event.GetKey() == TRAP::Input::Key::F1 && event.GetRepeatCount() < 1) //Switch to Vulkan
		TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::Vulkan);
	if (event.GetKey() == TRAP::Input::Key::F2 && event.GetRepeatCount() < 1) //Switch to OpenGL
		TRAP::Graphics::API::Context::SwitchRenderAPI(TRAP::Graphics::API::RenderAPI::OpenGL);

	if (event.GetKey() == TRAP::Input::Key::F5 && event.GetRepeatCount() < 1) //Make Window windowed
		TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Windowed);
	if (event.GetKey() == TRAP::Input::Key::F6 && event.GetRepeatCount() < 1) //Make Window Borderless Fullscreen
		TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Borderless);
	if (event.GetKey() == TRAP::Input::Key::F7 && event.GetRepeatCount() < 1) //Make Window Exclusive Fullscreen
		TRAP::Application::GetWindow()->SetDisplayMode(TRAP::Window::DisplayMode::Fullscreen);

	if (event.GetKey() == TRAP::Input::Key::F10 && event.GetRepeatCount() < 1) //Enable/Disable WireFrame Mode
	{
		m_wireFrame = !m_wireFrame;
		TRAP::Graphics::RenderCommand::SetWireFrame(m_wireFrame);
	}

	if (event.GetKey() == TRAP::Input::Key::F11 && event.GetRepeatCount() < 1)
		TRAP::Utils::MsgBox::Show("Just a prank bro!", "Critical Error");

	return true;
}