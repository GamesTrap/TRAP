#include "NVIDIAReflexTests.h"

NVIDIAReflexTests::NVIDIAReflexTests()
	: Layer("NVIDIA Reflex"),
	  m_shader(nullptr), m_latencyMode()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void NVIDIAReflexTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("NVIDIA Reflex");

	m_latencyMode = TRAP::Graphics::RenderCommand::GetLatencyMode();

	//Load Icosphere vertices
	m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_icoSphereVerticesIndexed.data(),
		                                                  static_cast<uint32_t>(m_icoSphereVerticesIndexed.size()) *
														  sizeof(float), TRAP::Graphics::UpdateFrequency::Static);
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
														sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::Static);
	m_indexBuffer->AwaitLoading();
	m_indexBuffer->Use();

	//Load Camera UniformBuffer
	m_cameraUBO = TRAP::Graphics::UniformBuffer::Create(sizeof(CameraUBOData),
	                                                    TRAP::Graphics::UpdateFrequency::Dynamic);
	m_cameraUBO->AwaitLoading();

	//Load Shader
	m_shader = TRAP::Graphics::ShaderManager::LoadFile("IcoSphereTest", "./Assets/Shaders/icosphere.shader");

	//Wait for all pending resources (just in case)
	TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	//Camera setup
	m_camera.SetPerspective(TRAP::Math::Radians(45.0f), 0.01f);
	m_camera.SetViewportSize(TRAP::Application::GetWindow()->GetWidth(),
	                         TRAP::Application::GetWindow()->GetHeight());
	m_cameraTransform.Position = TRAP::Math::Vec3(0.0f, 0.0f, 8.0f);

	//Enable depth testing because this is 3D stuff
	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
}

//-------------------------------------------------------------------------------------------------------------------//

void NVIDIAReflexTests::OnDetach()
{
	m_cameraUBO.reset();
	m_indexBuffer.reset();
	m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void NVIDIAReflexTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	TRAP::Graphics::RenderCommand::SetFillMode(TRAP::Graphics::FillMode::Solid);
	TRAP::Graphics::RenderCommand::SetCullMode(TRAP::Graphics::CullMode::Back);

	//Camera UBO
	{
		CameraUBOData camera{};
		camera.Projection = m_camera.GetProjectionMatrix();
		camera.View = TRAP::Math::Inverse(m_cameraTransform.GetTransform());
		camera.Model = TRAP::Math::Rotate(TRAP::Math::Radians(20.0f * TRAP::Application::GetTime()),
		                                  TRAP::Math::Vec3(1.0f, 1.0f, 1.0f));

		m_cameraUBO->SetData(&camera, sizeof(CameraUBOData));
		m_shader->UseUBO(1, 0, m_cameraUBO.get());
	}

	m_vertexBuffer->Use();
	m_indexBuffer->Use();
	m_shader->Use();
	TRAP::Graphics::RenderCommand::DrawIndexed(static_cast<uint32_t>(m_icosphereIndices.size()));
}

//-------------------------------------------------------------------------------------------------------------------//

void NVIDIAReflexTests::OnImGuiRender()
{
	ImGui::Begin("NVIDIA Reflex", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                   ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
	ImGui::Separator();
	ImGui::Text("Performance:");
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
    ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
    ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::Renderer::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::Separator();
	ImGui::Text("NVIDIA Reflex:");
	if(!TRAP::Graphics::RendererAPI::GPUSettings.ReflexSupported)
		ImGui::Text("NVIDIA Reflex is not supported on this GPU!");
	else
	{
		ImGui::Text("Current Latency Mode: %s", TRAP::Utils::String::ConvertToString(m_latencyMode).c_str());

		constexpr std::array<const char*, 3> latencyModes{"Disabled", "Enabled", "Enabled+Boost"};
		static int32_t currentLatencyMode = static_cast<int32_t>(m_latencyMode);
		if(ImGui::Combo("Latency Mode", &currentLatencyMode, latencyModes.data(), latencyModes.size()))
		{
			if(static_cast<int32_t>(m_latencyMode) != currentLatencyMode)
			{
				m_latencyMode = static_cast<TRAP::Graphics::LatencyMode>(currentLatencyMode);
				TRAP::Graphics::RenderCommand::SetLatencyMode(m_latencyMode);
				m_latencyMode = TRAP::Graphics::RenderCommand::GetLatencyMode();
			}
		}
	}
	ImGui::Separator();
	ImGui::Text("This software contains source code provided by NVIDIA Corporation.");
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void NVIDIAReflexTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool NVIDIAReflexTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}