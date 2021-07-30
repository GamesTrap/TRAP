#include "VulkanMultiWindowTests.h"

VulkanMultiWindowTests::VulkanMultiWindowTests()
	: Layer("VulkanMultiWindowTests"),
	  m_fpsTimer(),
	  m_vertexBuffer(nullptr),
	  m_indexBuffer(nullptr),
	  m_wireFrameMainWindow(false),
	  m_wireFrameSecondWindow(false),
	  m_window(nullptr),
	  m_colorData(),
	  m_sizeMultiplicatorData(),
	  m_colorTimer(),
	  m_vertexTimer(),
	  m_colorUniformBuffer(nullptr),
	  m_sizeMultiplicatorUniformBuffer(nullptr),
	  m_useUBO(false)
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

	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(), static_cast<uint32_t>(m_triangleVertices.size()) * sizeof(float), TRAP::Graphics::UpdateFrequency::None);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_triangleIndices.data(), static_cast<uint32_t>(m_triangleIndices.size()) * sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::None);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	TRAP::Graphics::ShaderManager::LoadFile("VKTest", "/shaders/test.shader");
	std::vector<TRAP::Graphics::Shader::Macro> macros{{"TEST", "0.5f"}};
	const TRAP::Scope<TRAP::Graphics::Shader>& vkTestUBOShader = TRAP::Graphics::ShaderManager::LoadFile("VKTestUBO", "/shaders/testubo.shader", &macros);

	m_sizeMultiplicatorUniformBuffer = vkTestUBOShader->GetUniformBuffer(1, 0);
	m_colorUniformBuffer = vkTestUBOShader->GetUniformBuffer(1, 1);
	m_sizeMultiplicatorUniformBuffer->AwaitLoading();
	m_colorUniformBuffer->AwaitLoading();

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
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line, m_window.get());
		else
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid, m_window.get());

		m_vertexBuffer->Use(m_window.get());
		m_indexBuffer->Use(m_window.get());

		if(m_useUBO)
		{
			if(m_vertexTimer.Elapsed() > 2.0f)
			{
				m_sizeMultiplicatorData.Multiplier = TRAP::Math::Vec3(1.5f);
				if(m_vertexTimer.Elapsed() > 4.0f)
					m_vertexTimer.Reset();
			}
			else
				m_sizeMultiplicatorData.Multiplier = TRAP::Math::Vec3(1.0f);

			if(m_colorTimer.Elapsed() > 1.0f)
			{
				if(m_colorTimer.Elapsed() > 2.0f)
					m_colorData.Color = TRAP::Math::Vec3(0.0f, 0.0f, 1.0f);
				else
					m_colorData.Color = TRAP::Math::Vec3(0.0f, 1.0f, 0.0f);

				if(m_colorTimer.Elapsed() > 3.0f)
					m_colorTimer.Reset();
			}
			else
				m_colorData.Color = TRAP::Math::Vec3(1.0f, 0.0f, 0.0f);

			m_sizeMultiplicatorUniformBuffer->SetData(&m_sizeMultiplicatorData, sizeof(SizeMultiplicatorData));
			m_colorUniformBuffer->SetData(&m_colorData, sizeof(ColorData));

			TRAP::Graphics::ShaderManager::Get("VKTestUBO")->Use(m_window.get());
		}
		else
			TRAP::Graphics::ShaderManager::Get("VKTest")->Use(m_window.get());

		TRAP::Graphics::RendererAPI::GetRenderer()->DrawIndexed(3, 0, 0, m_window.get());

		//Secondary Windows need to explicitly present its content
		TRAP::Graphics::RenderCommand::Present(m_window.get());
	}

	//Main Window OnUpdate
	{
		if(m_wireFrameMainWindow)
			TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line);
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
	ImGui::Text("UBO (F3): %s", m_useUBO ? "Enabled" : "Disabled");
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
	if(e.GetKey() == TRAP::Input::Key::F3)
	{
		m_useUBO = !m_useUBO;
		TP_TRACE("[VulkanMultiWindowTests] UBO: ", m_useUBO ? "Enabled" : "Disabled");
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