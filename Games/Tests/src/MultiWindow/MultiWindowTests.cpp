#include "MultiWindowTests.h"

namespace
{
	constexpr std::array<f32, 18> TriangleVertices
	{
		//XYZ RGB
		 0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
	};

	constexpr std::array<u16, 3> TriangleIndices
	{
		0, 1, 2
	};
}

//-------------------------------------------------------------------------------------------------------------------//

MultiWindowTests::MultiWindowTests()
	: Layer("MultiWindow")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void MultiWindowTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Multi-Window Test 1");

	//Create second Window
	TRAP::WindowProps::AdvancedProps advWinProps{};
	advWinProps.Focused = false;
	advWinProps.FocusOnShow = false;

	TRAP::WindowProps winProps{};
	winProps.Title = "Multi-Window Test 2";
	winProps.Width = 200;
	winProps.Height = 200;
	winProps.RefreshRate = 60;
	winProps.VSync = false;
	winProps.DisplayMode = TRAP::Window::DisplayMode::Windowed;
	winProps.Advanced = advWinProps;

	m_window = TRAP::MakeScope<TRAP::Window>(winProps);

	if(m_window)
	{
		m_window->SetEventCallback([this](TRAP::Events::Event& e) { OnEvent(e); });
		TRAP::Graphics::RenderCommand::SetClearColor({1.0f, 0.0f, 1.0f, 1.0f}, *m_window);
	}

	//Load Triangle vertices
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(TriangleVertices, TRAP::Graphics::UpdateFrequency::Static);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	//Load Triangle indices
	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(TriangleIndices, TRAP::Graphics::UpdateFrequency::Static);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load UniformBuffers
	m_sizeMultiplicatorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(SizeMultiplicatorData),
																			 TRAP::Graphics::UpdateFrequency::Dynamic);
	m_colorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(ColorData),
																 TRAP::Graphics::UpdateFrequency::Dynamic);
	m_sizeMultiplicatorUniformBuffer->AwaitLoading();
	m_colorUniformBuffer->AwaitLoading();

	//Load Shaders
	TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/test.shader", TRAP::Graphics::ShaderType::Graphics);
	const std::vector<TRAP::Graphics::Shader::Macro> macros{{"TEST", "0.5f"}};
	TRAP::Graphics::ShaderManager::LoadFile("TestUBO", "./Assets/Shaders/testubo.shader", TRAP::Graphics::ShaderType::Graphics, macros);

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void MultiWindowTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	//Second Window OnUpdate
	if(m_window)
	{
		if(m_wireFrameSecondWindow)
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line, *m_window);
		else
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid, *m_window);

		m_vertexBuffer->Use(*m_window);
		m_indexBuffer->Use(*m_window);

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

			const auto& shader = TRAP::Graphics::ShaderManager::GetGraphics("TestUBO");
			//Use UBOs
			shader->UseUBO(1, 0, *m_sizeMultiplicatorUniformBuffer);
			shader->UseUBO(1, 1, *m_colorUniformBuffer);

			shader->Use(*m_window);
		}
		else
			TRAP::Graphics::ShaderManager::GetGraphics("Test")->Use(*m_window);

		TRAP::Graphics::RenderCommand::DrawIndexed(3, 0, 0, *m_window);

		//Secondary Windows need to explicitly present its content
		TRAP::Graphics::RenderCommand::Flush(*m_window);
	}

	//Main Window OnUpdate
	{
		if(m_wireFrameMainWindow)
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line);
		else
			TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

		m_vertexBuffer->Use();
		m_indexBuffer->Use();
		TRAP::Graphics::ShaderManager::GetGraphics("Test")->Use();
		TRAP::Graphics::RenderCommand::DrawIndexed(3);
	}

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[MultiWindow] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
		TP_INFO("[MultiWindow] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void MultiWindowTests::OnImGuiRender()
{
	ImGui::Begin("Multi-Window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                           ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("Main-Window WireFrame (F1): %s", m_wireFrameMainWindow ? "Enabled" : "Disabled");
	ImGui::Text("Secondary-Window WireFrame (F2): %s", m_wireFrameSecondWindow ? "Enabled" : "Disabled");
	ImGui::Text("UBO (F3): %s", m_useUBO ? "Enabled" : "Disabled");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void MultiWindowTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::WindowCloseEvent>(std::bind_front(&MultiWindowTests::OnWindowClose, this));
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&MultiWindowTests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool MultiWindowTests::OnWindowClose(const TRAP::Events::WindowCloseEvent& e)
{
	if (m_window && e.GetWindow() == *m_window)
		m_window.reset();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool MultiWindowTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrameMainWindow = !m_wireFrameMainWindow;
		TP_TRACE("[MultiWindow] Main-Window WireFrame: ", m_wireFrameMainWindow ? "Enabled" : "Disabled");
	}
	if (e.GetKey() == TRAP::Input::Key::F2)
	{
		m_wireFrameSecondWindow = !m_wireFrameSecondWindow;
		TP_TRACE("[MultiWindow] Secondary-Window WireFrame: ", m_wireFrameSecondWindow ? "Enabled" :
		 																				 "Disabled");
	}
	if(e.GetKey() == TRAP::Input::Key::F3)
	{
		m_useUBO = !m_useUBO;
		TP_TRACE("[MultiWindow] UBO: ", m_useUBO ? "Enabled" : "Disabled");
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
