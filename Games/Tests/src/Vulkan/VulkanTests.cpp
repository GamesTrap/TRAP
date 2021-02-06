#include "VulkanTests.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_window(nullptr),
	  m_defaultPipelineCache(nullptr),
	  m_wireFrame(false),
	  m_quad(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::VFS::MountShaders("Assets/Shaders");
	
	TRAP::Application::GetWindow()->SetTitle("Vulkan Multi-Window Test");

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

	//TODO
	//TRAP::Graphics::RendererAPI::PipelineCacheLoadDesc cacheDesc{};
	//cacheDesc.VirtualOrPhysicalPath = "Assets/Cache/VkClearScreen.cache";
	//m_defaultPipelineCache = TRAP::Graphics::PipelineCache::Create(cacheDesc);

	TRAP::Graphics::ShaderManager::LoadFile("/shaders/test.shader")->Use();
	
	//TODO
	//TRAP::Graphics::RendererAPI::PipelineDesc desc{};
	//if (m_defaultPipelineCache)
		//desc.Cache = m_defaultPipelineCache;
	////////////////////////////////////////////////////////////

	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(), m_quadVertices.size() * sizeof(float), TRAP::Graphics::BufferUsage::Dynamic);
	const TRAP::Graphics::BufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	m_window.reset();
	
	m_vertexBuffer.reset();

	//TODO
	//if (m_defaultPipelineCache)
	//{
		//m_defaultPipelineCache->Save("Assets/Cache/VkClearScreen.cache");
		//m_defaultPipelineCache.reset();
	//}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	if (m_window)
		TRAP::Graphics::RenderCommand::Present(m_window);

	if(m_wireFrame)
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::WireFrame);
	else
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

	if(!m_quad)
		m_vertexBuffer->SetData(m_triangleVertices.data(), m_triangleVertices.size() * sizeof(float));
	else
		m_vertexBuffer->SetData(m_quadVertices.data(), m_quadVertices.size() * sizeof(float));

	m_vertexBuffer->Use();
	TRAP::Graphics::RendererAPI::GetRenderer()->Draw(m_quad ? 6 : 3, 0);

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
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnWindowClose(TRAP::Events::WindowCloseEvent& e)
{
	if (e.GetTitle() == m_window->GetTitle())
		m_window.reset();
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
		m_wireFrame = !m_wireFrame;
	if (e.GetKey() == TRAP::Input::Key::F2)
		m_quad = !m_quad;
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();
	
	return false;
}