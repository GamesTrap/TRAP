#include "AntiAliasingTests.h"

AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing"), m_fpsTimer(), m_msaa(false),
	  m_camera(-(static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) /
	             static_cast<float>(TRAP::Application::GetWindow()->GetHeight())),
	           static_cast<float>(TRAP::Application::GetWindow()->GetWidth()) /
			   static_cast<float>(TRAP::Application::GetWindow()->GetHeight()),
	           -1.0f, 1.0f, -1.0f, 1.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("AntiAliasing");

	//Enable depth testing because this is 3D stuff
	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate(const TRAP::Utils::TimeStep& /*deltaTime*/)
{
	const float angle = TRAP::Application::GetTime().GetSeconds() * TRAP::Math::PI<float>();

	TRAP::Graphics::Renderer2D::BeginScene(m_camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[AntiAliasing] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[AntiAliasing] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnImGuiRender()
{
	ImGui::Begin("AntiAliasing", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                      ImGuiWindowFlags_AlwaysAutoResize);
	if(ImGui::Checkbox("MSAA", &m_msaa))
	{
		if(m_msaa)
			TRAP::Graphics::RenderCommand::SetAntiAliasing(TRAP::Graphics::AntiAliasing::MSAA, TRAP::Graphics::SampleCount::Eight);
		else
			TRAP::Graphics::RenderCommand::SetAntiAliasing(TRAP::Graphics::AntiAliasing::Off, TRAP::Graphics::SampleCount::One);
	}
	ImGui::Text("Press ESC to close");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e)
	{
		return OnFrameBufferResize(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool AntiAliasingTests::OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetProjection(-(static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight())),
		                   static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight()),
		                   -1.0f, 1.0f, -1.0f, 1.0f);

	return false;
}