#include "VulkanTests.h"

VulkanTests::VulkanTests()
	: Layer("VulkanTests"),
	  m_fpsTimer(),
	  m_vertexBuffer(nullptr),
	  m_indexBuffer(nullptr),
	  m_wireFrame(false),
	  m_quad(false),
	  m_indexed(false),
	  m_vsync(TRAP::Application::GetConfig().Get<bool>("VSync")),
	  m_pushConstantOrUBO(0),
	  m_colorData(),
	  m_sizeMultiplicatorData(),
	  m_colorTimer(),
	  m_vertexTimer(),
	  m_colorUniformBuffer(nullptr),
	  m_sizeMultiplicatorUniformBuffer(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Test");

	TRAP::VFS::MountShaders("Assets/Shaders");

	//Load Triangle vertices (with enough space for a quad)
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_triangleVertices.data(),
	                                                      static_cast<uint32_t>(m_quadVertices.size()) *
														  sizeof(float), TRAP::Graphics::UpdateFrequency::PerFrame);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	//Load Triangle indices (with enough space for a quad)
	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_triangleIndices.data(),
	                                                    static_cast<uint32_t>(m_quadIndices.size()) *
														sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::PerFrame);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load UniformBuffers
	m_sizeMultiplicatorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(SizeMultiplicatorData),
																			 TRAP::Graphics::UpdateFrequency::PerFrame);
	m_colorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(ColorData),
																 TRAP::Graphics::UpdateFrequency::PerFrame);
	m_sizeMultiplicatorUniformBuffer->AwaitLoading();
	m_colorUniformBuffer->AwaitLoading();

	TRAP::Graphics::ShaderManager::LoadFile("VKTest", "/shaders/test.shader");
	TRAP::Graphics::ShaderManager::LoadFile("VKTestPushConstant", "/shaders/testpushconstant.shader");
	std::vector<TRAP::Graphics::Shader::Macro> macros{{"TEST", "0.5f"}};
	const auto& vkTestUBOShader = TRAP::Graphics::ShaderManager::LoadFile("VKTestUBO", "/shaders/testubo.shader",
	 																	  &macros);
	vkTestUBOShader->UseUBO(0, m_sizeMultiplicatorUniformBuffer.get());
	vkTestUBOShader->UseUBO(1, m_colorUniformBuffer.get());

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnDetach()
{
	m_colorUniformBuffer.reset();
	m_sizeMultiplicatorUniformBuffer.reset();
	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	if(m_wireFrame)
		TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line);
	else
		TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);

	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);

	if(!m_quad)
	{
		m_vertexBuffer->SetData(m_triangleVertices.data(), static_cast<uint32_t>(m_triangleVertices.size()) *
		                        sizeof(float));
		if (m_indexed)
			m_indexBuffer->SetData(m_triangleIndices.data(), static_cast<uint32_t>(m_triangleIndices.size()) *
			                       sizeof(uint16_t));
	}
	else
	{
		if(!m_indexed)
			m_vertexBuffer->SetData(m_quadVertices.data(), static_cast<uint32_t>(m_quadVertices.size()) *
			                        sizeof(float));
		else
		{
			m_vertexBuffer->SetData(m_quadVerticesIndexed.data(),
			                        static_cast<uint32_t>(m_quadVerticesIndexed.size()) * sizeof(float));
			m_indexBuffer->SetData(m_quadIndices.data(), static_cast<uint32_t>(m_quadIndices.size()) *
			                       sizeof(uint16_t));
			const TRAP::Graphics::VertexBufferLayout layout =
			{
				{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
				{TRAP::Graphics::ShaderDataType::Float3, "Color"},
				{TRAP::Graphics::ShaderDataType::Float2, "UV"}
			};
			m_vertexBuffer->SetLayout(layout);
		}
	}

	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	if(m_pushConstantOrUBO == 1)
	{
		if(m_colorTimer.Elapsed() > 2.5f)
		{
			for(uint32_t i = 0; i < static_cast<uint32_t>(m_colorData.Color.Length()); ++i)
				m_colorData.Color[i] = TRAP::Utils::Random::Get(0.0f, 1.0f);

			m_colorTimer.Reset();
		}

		TRAP::Graphics::ShaderManager::Get("VKTestPushConstant")->Use();

		TRAP::Graphics::RenderCommand::SetPushConstants("ColorRootConstant", &m_colorData);
	}
	else if(m_pushConstantOrUBO == 2)
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

		TRAP::Graphics::ShaderManager::Get("VKTestUBO")->Use();
	}
	else
		TRAP::Graphics::ShaderManager::Get("VKTest")->Use();

	if(!m_indexed)
		TRAP::Graphics::RenderCommand::Draw(m_quad ? 6 : 3);
	else
		TRAP::Graphics::RenderCommand::DrawIndexed(m_quad ? 6 : 3);

	//Simple performance metrics
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
	ImGui::Begin("Vulkan Test", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
										 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Press ESC to close");
	ImGui::Text("WireFrame (F1): %s", m_wireFrame ? "Enabled" : "Disabled");
	ImGui::Text("Geometry (F2): %s", m_quad ? "Quad" : "Triangle");
	ImGui::Text("Indexed Drawing (F3): %s", m_indexed ? "Enabled" : "Disabled");
	std::string shaderData = "Uniform Buffer";
	if(m_pushConstantOrUBO == 0)
		shaderData = "Disabled";
	else if(m_pushConstantOrUBO == 1)
		shaderData = "Push Constants";
	ImGui::Text("Shader Data (F4): %s", shaderData.c_str());
	ImGui::Text("VSync (V): %s", m_vsync ? "Enabled" : "Disabled");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
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
	if(e.GetKey() == TRAP::Input::Key::F4)
	{
		m_pushConstantOrUBO = (m_pushConstantOrUBO + 1) % 3;
		TP_TRACE("[VulkanTests] Push Constant / Uniform Buffer: ", m_pushConstantOrUBO ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[VulkanTests] VSync: ", m_vsync ? "On" : "Off");
		e.GetWindow()->SetVSync(m_vsync);
	}
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}