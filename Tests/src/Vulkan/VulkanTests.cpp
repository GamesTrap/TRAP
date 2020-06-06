#include "VulkanTests.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"), m_window(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	TRAP::WindowProps windowProps
	{
		"Vulkan Test",
		200,
		200,
		60,
		TRAP::Window::DisplayMode::Windowed,
		TRAP::WindowProps::Advanced
		{
			0,
			true,
			false,
			true,
			true,
			true,
			true,
			false,
			TRAP::Window::CursorMode::Normal
		},
		0
	};
	m_window = TRAP::MakeScope<TRAP::Window>(windowProps);
	m_window->SetEventCallback([this](TRAP::Events::Event& e) { OnEvent(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	m_window.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	TRAP::Window::Use();
	
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

	if (m_window)
	{
		TRAP::Window::Use(m_window);
		
		//Render
		TRAP::Graphics::RenderCommand::SetClearColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color_Depth);

		TRAP::Window::Use();
		
		TRAP::Graphics::RenderCommand::Present(m_window);
	}

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetWindowTitle() == m_window->GetTitle())
		m_window.reset();
	
	return true;
}