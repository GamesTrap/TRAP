#include "VulkanIcoSphereTests.h"

VulkanIcoSphereTests::VulkanIcoSphereTests()
	: Layer("VulkanIcoSphereTests"),
	  m_wireFrame(false),
	  m_vsync(TRAP::Application::GetConfig().Get<bool>("VSync")),
	  m_FOV(45.0f),
	  m_rotationSpeed(20.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanIcoSphereTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Vulkan Test");

	TRAP::VFS::MountShaders("Assets/Shaders");

	//Load Icosphere vertices
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_icoSphereVerticesIndexed.data(),
		                                                  static_cast<uint32_t>(m_icoSphereVerticesIndexed.size()) *
														  sizeof(float), TRAP::Graphics::UpdateFrequency::None);
	const TRAP::Graphics::VertexBufferLayout layout =
	{
		{TRAP::Graphics::ShaderDataType::Float3, "Pos"},
		{TRAP::Graphics::ShaderDataType::Float3, "Col"}
	};
	m_vertexBuffer->SetLayout(layout);
	m_vertexBuffer->AwaitLoading();
	m_vertexBuffer->Use();

	//Load Icosphere indices
	m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_icosphereIndices.data(),
	                                                    static_cast<uint32_t>(m_icosphereIndices.size()) *
														sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::None);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load Camera UniformBuffer
	m_cameraUBO = TRAP::Graphics::UniformBuffer::Create(sizeof(CameraUBOData),
	                                                    TRAP::Graphics::UpdateFrequency::PerFrame);
	m_cameraUBO->AwaitLoading();

	//Load Shader
	const auto& shader = TRAP::Graphics::ShaderManager::LoadFile("VKIcoSphereTest", "/shaders/icosphere.shader");
	shader->UseUBO(0, m_cameraUBO.get());

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	//Camera setup
	m_camera.SetPerspective(TRAP::Math::Radians(m_FOV), 0.01f, 1000.0f);
	m_camera.SetViewportSize(TRAP::Application::GetWindow()->GetWidth(),
	                         TRAP::Application::GetWindow()->GetHeight());
	m_cameraTransform.Position = TRAP::Math::Vec3(0.0f, 0.0f, 8.0f);

	//Enable depth testing because this is 3D stuff
	TRAP::Graphics::RendererAPI::GetRenderer()->SetDepthTesting(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanIcoSphereTests::OnDetach()
{
	m_cameraUBO.reset();
	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanIcoSphereTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	if(m_wireFrame)
	{
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Line);
		TRAP::Graphics::RendererAPI::GetRenderer()->SetCullMode(TRAP::Graphics::RendererAPI::CullMode::None);
	}
	else
	{
		TRAP::Graphics::RendererAPI::GetRenderer()->SetFillMode(TRAP::Graphics::RendererAPI::FillMode::Solid);
		TRAP::Graphics::RendererAPI::GetRenderer()->SetCullMode(TRAP::Graphics::RendererAPI::CullMode::Back);
	}

	m_indexBuffer->Use();
	m_vertexBuffer->Use();

	//Camera UBO
	{

		CameraUBOData camera{};
		camera.Projection = m_camera.GetProjectionMatrix();
		camera.View = TRAP::Math::Inverse(m_cameraTransform.GetTransform());
		camera.Model = TRAP::Math::Rotate(TRAP::Math::Radians(m_rotationSpeed * TRAP::Application::GetTime()),
		                                  TRAP::Math::Vec3(1.0f, 1.0f, 1.0f));

		m_cameraUBO->SetData(&camera, sizeof(CameraUBOData));
	}

	TRAP::Graphics::ShaderManager::Get("VKIcoSphereTest")->Use();

	TRAP::Graphics::RenderCommand::DrawIndexed(m_icosphereIndices.size());

	//Simple performance metrics
	if (m_fpsTimer.Elapsed() >= 5.0f) //Output Every 5 Seconds
	{
		TP_INFO("[Sandbox] FPS: ", TRAP::Graphics::Renderer::GetFPS());
		TP_INFO("[Sandbox] FrameTime: ", TRAP::Graphics::Renderer::GetFrameTime(), "ms");
		m_fpsTimer.Reset();
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanIcoSphereTests::OnImGuiRender()
{
	float fov = TRAP::Math::Degrees(m_camera.GetPerspectiveVerticalFOV());
	TRAP::Math::Vec3 pos = m_cameraTransform.Position;

	ImGui::Begin("Vulkan IcoSphere Test", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                               ImGuiWindowFlags_AlwaysAutoResize |
												   ImGuiWindowFlags_NoSavedSettings |
												   ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
	ImGui::Text("Press ESC to close");
	ImGui::Text("WireFrame (F1): %s", m_wireFrame ? "Enabled" : "Disabled");
	ImGui::Text("VSync (V): %s", m_vsync ? "Enabled" : "Disabled");
	if(ImGui::SliderFloat("Camera FoV", &fov, 45.0f, 100.0f))
        m_camera.SetPerspectiveVerticalFOV(TRAP::Math::Radians(fov));
	if(ImGui::SliderFloat3("Camera Pos", &pos[0], -10.0f, 10.0f))
		m_cameraTransform.Position = pos;
	ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.0f, 500.0f);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void VulkanIcoSphereTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool VulkanIcoSphereTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::F1)
	{
		m_wireFrame = !m_wireFrame;
		TP_TRACE("[VulkanTests] WireFrame: ", m_wireFrame ? "Enabled" : "Disabled");
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