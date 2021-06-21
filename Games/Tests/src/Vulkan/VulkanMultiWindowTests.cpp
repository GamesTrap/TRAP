#include "VulkanMultiWindowTests.h"

VulkanMultiWindowTests::VulkanMultiWindowTests()
	: Layer("VulkanMultiWindowTests"),
	  m_fpsTimer(),
	  m_vertexBuffer(nullptr),
	  m_indexBuffer(nullptr),
	  m_wireFrameMainWindow(false),
	  m_wireFrameSecondWindow(false),
	  m_window(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanMultiWindowTests::OnAttach()
{
	TRAP::VFS::SetHotShaderReloading(true);
	TRAP::VFS::MountShaders("Assets/Shaders");

	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test 1");

	//Create second Window
	TRAP::WindowProps windowProps
	{
		"Vulkan Multi-Window Test 2",
		200,
		200,
		60,
		false,
		TRAP::Window::DisplayMode::Windowed,
		TRAP::WindowProps::AdvancedProps
		{
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
	TRAP::Graphics::RendererAPI::GetRenderer()->SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f }, m_window.get());

	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(), static_cast<uint32_t>(m_triangleVertices.size()) * sizeof(float), TRAP::Graphics::BufferUsage::Static);
	const TRAP::Graphics::BufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_triangleIndices.data(), static_cast<uint32_t>(m_triangleIndices.size()) * sizeof(uint16_t), TRAP::Graphics::BufferUsage::Static);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	TRAP::Graphics::ShaderManager::LoadFile("VKTest", "/shaders/test.shader");

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanMultiWindowTests::OnDetach()
{
	if(m_window)
		m_window.reset();

	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanMultiWindowTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	//Second Window OnUpdate
	if(m_window)
	{
		if(m_wireFrameSecondWindow)
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::WireFrame, m_window.get());
		else
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid, m_window.get());

		m_vertexBuffer->Use(m_window.get());
		m_indexBuffer->Use(m_window.get());

		TRAP::Graphics::ShaderManager::Get("VKTest")->Use(m_window.get());

		TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(3, 0, 0, m_window.get());

		//Secondary Windows need to explicitly present its content
		TRAP::Graphics::RenderCommand::Present(m_window);
	}

	//Main Window OnUpdate
	{
		if(m_wireFrameMainWindow)
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::WireFrame);
		else
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

		m_vertexBuffer->Use();
		m_indexBuffer->Use();

		TRAP::Graphics::ShaderManager::Get("VKTest")->Use();

		TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(3);
	}

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanMultiWindowTests::OnImGuiRender()
{
	ImGui::Begin("Vulkan Multi-Window Test");
	ImGui::Text("Main-Window WireFrame (F1): %s", m_wireFrameMainWindow ? "Enabled" : "Disabled");
	ImGui::Text("Secondary-Window WireFrame (F2): %s", m_wireFrameSecondWindow ? "Enabled" : "Disabled");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanMultiWindowTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanMultiWindowTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetWindow() == m_window.get())
		m_window.reset();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanMultiWindowTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrameMainWindow = !m_wireFrameMainWindow;
		TP_TRACE("[VulkanMultiWindowTests] Main-Window WireFrame: ", m_wireFrameMainWindow ? "Enabled" : "Disabled");
	}
	if (e.GetKey() == TRAP::Input::Key::F2)
	{
		m_wireFrameSecondWindow = !m_wireFrameSecondWindow;
		TP_TRACE("[VulkanMultiWindowTests] Secondary-Window WireFrame: ", m_wireFrameSecondWindow ? "Enabled" : "Disabled");
	}

	if (e.GetKey() == TRAP::Input::Key::Escape)
	{
		if(m_window)
			m_window.reset();
		else
			TRAP::Application::Shutdown();
	}

	return false;
}