#include "IcoSphereTests.h"

namespace
{
	constexpr std::array<f32, 12ull * 6> IcoSphereVerticesIndexed
	{
		//XYZ RGB
		-1.0f,                                     (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,    0.0f,                                        1.0f, 0.0f, 0.0f,
		 1.0f,                                     (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,    0.0f,                                        1.0f, 0.0f, 0.0f,
		-1.0f,                                    -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),  0.0f,                                        1.0f, 0.0f, 0.0f,
		 1.0f,                                    -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),  0.0f,                                        1.0f, 0.0f, 0.0f,

		 0.0f,                                    -1.0f,                                      (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,      0.0f, 1.0f, 0.0f,
		 0.0f,                                     1.0f,                                      (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,      0.0f, 1.0f, 0.0f,
		 0.0f,                                    -1.0f,                                     -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),    0.0f, 1.0f, 0.0f,
		 0.0f,                                     1.0f,                                     -((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f),    0.0f, 1.0f, 0.0f,

		 (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,   0.0f,                                     -1.0f,                                        0.0f, 0.0f, 1.0f,
		 (1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f,   0.0f,                                      1.0f,                                        0.0f, 0.0f, 1.0f,
		-((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f), 0.0f,                                     -1.0f,                                        0.0f, 0.0f, 1.0f,
		-((1.0f + TRAP::Math::Sqrt(5.0f)) / 2.0f), 0.0f,                                      1.0f,                                        0.0f, 0.0f, 1.0f
	};

	constexpr std::array<u16, 20ull * 3> IcoSphereIndices
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};
}

//-------------------------------------------------------------------------------------------------------------------//

IcoSphereTests::IcoSphereTests()
	: Layer("IcoSphere")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void IcoSphereTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("IcoSphere");

	//Load Icosphere vertices
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(IcoSphereVerticesIndexed, TRAP::Graphics::UpdateFrequency::Static);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Col"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	//Load Icosphere indices
	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(IcoSphereIndices, TRAP::Graphics::UpdateFrequency::Static);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load Camera UniformBuffer
	m_cameraUBO = TRAP::Graphics::UniformBuffer::Create(sizeof(CameraUBOData),
	                                                    TRAP::Graphics::UpdateFrequency::Dynamic);
	m_cameraUBO->AwaitLoading();

	//Load Shader
	m_shader = TRAP::Graphics::ShaderManager::LoadFile(TRAP::Graphics::ShaderType::Graphics, "IcoSphereTest", "./Assets/Shaders/icosphere.shader");

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	//Camera setup
	m_camera.SetPerspective(TRAP::Math::Radians(m_FOV), 0.01f);
	m_camera.SetViewportSize(TRAP::Application::GetWindow()->GetFrameBufferSize().x(),
	                         TRAP::Application::GetWindow()->GetFrameBufferSize().y());
	m_cameraTransform.Position = TRAP::Math::Vec3(0.0f, 0.0f, 8.0f);

	//Enable depth testing because this is 3D stuff
	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void IcoSphereTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	if(m_wireFrame)
	{
		TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Line);
		TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::None);
	}
	else
	{
		TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Solid);
		TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::Back);
	}

	//Camera UBO
	{
		CameraUBOData camera{};
		camera.Projection = m_camera.GetProjectionMatrix();
		camera.View = TRAP::Math::Inverse(m_cameraTransform.GetTransform());
		camera.Model = TRAP::Math::Rotate(TRAP::Math::Radians(m_rotationSpeed * TRAP::Application::GetTime()),
		                                  TRAP::Math::Vec3(1.0f, 1.0f, 1.0f));

		m_cameraUBO->SetData(&camera, sizeof(CameraUBOData));
		m_shader->UseUBO(1, 0, *m_cameraUBO);
	}

	m_vertexBuffer->Use();
	m_indexBuffer->Use();
	m_shader->Use();
	TRAP::Graphics::RenderCommand::DrawIndexed(NumericCast<u32>(IcoSphereIndices.size()));

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[IcoSphere] FPS: ", TRAP::Graphics::RenderCommand::GetCPUFPS());
		TP_INFO("[IcoSphere] CPU FrameTime: ", TRAP::Graphics::RenderCommand::GetCPUFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void IcoSphereTests::OnImGuiRender()
{
	f32 fov = TRAP::Math::Degrees(m_camera.GetPerspectiveVerticalFOV());
	TRAP::Math::Vec3 pos = m_cameraTransform.Position;

	ImGui::Begin("IcoSphere", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Text("WireFrame (F1): %s", m_wireFrame ? "Enabled" : "Disabled");
	ImGui::Text("VSync (V): %s", m_vsync ? "Enabled" : "Disabled");
	if(ImGui::SliderFloat("Camera FoV", &fov, 45.0f, 100.0f))
        m_camera.SetPerspectiveVerticalFOV(TRAP::Math::Radians(fov));
	if(ImGui::SliderFloat3("Camera Pos", &std::get<0>(pos), -10.0f, 10.0f))
		m_cameraTransform.Position = pos;
	ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.0f, 500.0f);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void IcoSphereTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&IcoSphereTests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool IcoSphereTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrame = !m_wireFrame;
		TP_TRACE("[IcoSphere] WireFrame: ", m_wireFrame ? "Enabled" : "Disabled");
	}
	if(e.GetKey() == TRAP::Input::Key::V)
	{
		m_vsync = !m_vsync;
		TP_TRACE("[IcoSphere] VSync: ", m_vsync ? "On" : "Off");
		e.GetWindow().SetVSync(m_vsync);
	}
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}
