#include "VulkanTests.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_window(nullptr),
	  m_wireFrame(false),
	  m_quad(false),
	  m_indexed(false),
	  m_vsync(true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::VFS::SetHotShaderReloading(true);
	TRAP::VFS::MountShaders("Assets/Shaders");

	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

	TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(m_vsync);

	if (s_window)
	{
		TRAP::WindowProps windowProps
		{
			"Vulkan Test",
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
	}

	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(), static_cast<uint32_t>(m_quadVertices.size()) * sizeof(float), TRAP::Graphics::BufferUsage::Dynamic);
	const TRAP::Graphics::BufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_triangleIndices.data(), static_cast<uint32_t>(m_quadIndices.size()) * sizeof(uint16_t), TRAP::Graphics::BufferUsage::Dynamic);
	m_indexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	TRAP::Graphics::ShaderManager::LoadFile("VKTestNoUBO", "/shaders/test.shader");
	//TRAP::Graphics::ShaderManager::LoadFile("VKTestNoUBO", "/shaders/test.shader")->Use();

	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	if (s_window)
		m_window.reset();

	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	if (s_window && m_window)
		TRAP::Graphics::RenderCommand::Present(m_window);

	if(m_wireFrame)
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::WireFrame);
	else
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

	if(!m_quad)
	{
		m_vertexBuffer->SetData(m_triangleVertices.data(), static_cast<uint32_t>(m_triangleVertices.size()) * sizeof(float));
		if (m_indexed)
			m_indexBuffer->SetData(m_triangleIndices.data(), static_cast<uint32_t>(m_triangleIndices.size()) * sizeof(uint16_t));
	}
	else
	{
		if(!m_indexed)
			m_vertexBuffer->SetData(m_quadVertices.data(), static_cast<uint32_t>(m_quadVertices.size()) * sizeof(float));
		else
		{
			m_vertexBuffer->SetData(m_quadVerticesIndexed.data(), static_cast<uint32_t>(m_quadVerticesIndexed.size()) * sizeof(float));
			m_indexBuffer->SetData(m_quadIndices.data(), static_cast<uint32_t>(m_quadIndices.size()) * sizeof(uint16_t));
		}
	}

	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	TRAP::Graphics::ShaderManager::Get("VKTestNoUBO")->Use();

	if(!m_indexed)
		TRAP::Graphics::RendererAPI::GetRenderer()->Draw(m_quad ? 6 : 3);
	else
		TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(static_cast<uint32_t>(m_indexBuffer->GetCount()));

	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnImGuiRender()
{
	ImGui::Begin("LOL");
	ImGui::Text("HELLO IMGUI");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>([this](TRAP::Events::WindowCloseEvent& e) { return OnWindowClose(e); });
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (s_window)
	{
		if (e.GetWindow() == m_window.get())
			m_window.reset();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrame = !m_wireFrame;
		TP_TRACE("[VulkanTests] WireFrame: ", m_wireFrame ? "Enabled" : "Disabled");
	}
	if (e.GetKey() == TRAP::Input::Key::F2)
	{
		m_quad = !m_quad;
		TP_TRACE("[VulkanTests] Geometry: ", m_quad ? "Quad" : "Triangle");
	}
	if(e.GetKey() == TRAP::Input::Key::F3)
	{
		m_indexed = !m_indexed;
		TP_TRACE("[VulkanTests] Indexed Drawing: ", m_indexed ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[VulkanTests] VSync: ", m_vsync ? "On" : "Off");
		TRAP::Graphics::RendererAPI::GetRenderer()->SetVSync(m_vsync);
	}
	if (e.GetKey() == TRAP::Input::Key::Escape)
	{
		if(s_window && m_window)
			m_window.reset();
		else
			TRAP::Application::Shutdown();
	}

	return false;
}