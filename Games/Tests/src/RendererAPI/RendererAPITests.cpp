#include "RendererAPITests.h"

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

	constexpr std::array<f32, 36> QuadVertices
	{
		//XYZ RGB
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f
	};

	constexpr std::array<f32, 32> QuadVerticesIndexed
	{
		//XYZ RGB UV
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f
	};

	constexpr std::array<u16, 6> QuadIndices
	{
		0, 1, 2, 2, 3, 0
	};
}

//-------------------------------------------------------------------------------------------------------------------//

RendererAPITests::RendererAPITests()
	: Layer("RendererAPITests")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPITests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("RendererAPI Test");

	//Load Triangle vertices (with enough space for a quad)
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create({TriangleVertices.data(), QuadVertices.size()},
	                                                      TRAP::Graphics::UpdateFrequency::Dynamic);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Color"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	//Load Triangle indices (with enough space for a quad)
	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create({TriangleIndices.data(), QuadIndices.size()},
	                                                    TRAP::Graphics::UpdateFrequency::Dynamic);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load UniformBuffers
	m_sizeMultiplicatorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(SizeMultiplicatorData),
																			 TRAP::Graphics::UpdateFrequency::Dynamic);
	m_colorUniformBuffer = TRAP::Graphics::UniformBuffer::Create(sizeof(ColorData),
																 TRAP::Graphics::UpdateFrequency::Dynamic);
	m_sizeMultiplicatorUniformBuffer->AwaitLoading();
	m_colorUniformBuffer->AwaitLoading();

	TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/test.shader", TRAP::Graphics::ShaderType::Graphics);
	TRAP::Graphics::ShaderManager::LoadFile("TestPushConstant", "./Assets/Shaders/testpushconstant.shader", TRAP::Graphics::ShaderType::Graphics);
	const std::vector<TRAP::Graphics::Shader::Macro> macros{{"TEST", "0.5f"}};
	TRAP::Graphics::ShaderManager::LoadFile("TestUBO", "./Assets/Shaders/testubo.shader", TRAP::Graphics::ShaderType::Graphics, macros);

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPITests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
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
		m_vertexBuffer->SetData(TriangleVertices);
		if (m_indexed)
			m_indexBuffer->SetData(TriangleIndices);
	}
	else
	{
		if(!m_indexed)
			m_vertexBuffer->SetData(QuadVertices);
		else
		{
			m_vertexBuffer->SetData(QuadVerticesIndexed);
			m_indexBuffer->SetData(QuadIndices);
			const TRAP::Graphics::VertexBufferLayout layoutUV =
			{
				{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
				{TRAP::Graphics::ShaderDataType::Float3, "Color"},
				{TRAP::Graphics::ShaderDataType::Float2, "UV"}
			};
			m_vertexBuffer->SetLayout(layoutUV);
		}
	}

	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	if(m_pushConstantOrUBO == 1)
	{
		if(m_colorTimer.Elapsed() > 2.5f)
		{
			for(f32& color : m_colorData.Color)
				color = TRAP::Utils::Random::Get(0.0f, 1.0f);

			m_colorTimer.Reset();
		}

		TRAP::Graphics::ShaderManager::GetGraphics("TestPushConstant")->Use();

		TRAP::Graphics::RenderCommand::SetPushConstants("ColorRootConstant", m_colorData);
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

		const auto& shader = TRAP::Graphics::ShaderManager::GetGraphics("TestUBO");
		//Use UBOs
		shader->UseUBO(1, 0, *m_sizeMultiplicatorUniformBuffer);
		shader->UseUBO(1, 1, *m_colorUniformBuffer);

		shader->Use();
	}
	else
		TRAP::Graphics::ShaderManager::GetGraphics("Test")->Use();

	if(!m_indexed)
		TRAP::Graphics::RenderCommand::Draw(m_quad ? 6 : 3);
	else
		TRAP::Graphics::RenderCommand::DrawIndexed(m_quad ? 6 : 3);

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[RendererAPITests] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
		TP_INFO("[RendererAPITests] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void RendererAPITests::OnImGuiRender()
{
	ImGui::Begin("RendererAPI Test", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                     ImGuiWindowFlags_AlwaysAutoResize);
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

void RendererAPITests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&RendererAPITests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool RendererAPITests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrame = !m_wireFrame;
		TP_TRACE("[RendererAPITests] WireFrame: ", m_wireFrame ? "Enabled" : "Disabled");
	}
	if (e.GetKey() == TRAP::Input::Key::F2)
	{
		m_quad = !m_quad;
		TP_TRACE("[RendererAPITests] Geometry: ", m_quad ? "Quad" : "Triangle");
	}
	if(e.GetKey() == TRAP::Input::Key::F3)
	{
		m_indexed = !m_indexed;
		TP_TRACE("[RendererAPITests] Indexed Drawing: ", m_indexed ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::F4)
	{
		m_pushConstantOrUBO = NumericCast<u8>((m_pushConstantOrUBO + 1) % 3);
		TP_TRACE("[RendererAPITests] Push Constant / Uniform Buffer: ", m_pushConstantOrUBO != 0u ? "On" : "Off");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[RendererAPITests] VSync: ", m_vsync ? "On" : "Off");
		e.GetWindow().SetVSync(m_vsync);
	}
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}
