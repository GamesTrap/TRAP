#include "SPIRVTests.h"

SPIRVTests::SPIRVTests()
	: Layer("SPIRV"), m_vertexBuffer(nullptr), m_indexBuffer(nullptr)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("SPIRV");

	//Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(m_quadVerticesIndexed.data(),
                                                          static_cast<uint32_t>(m_quadVerticesIndexed.size()) *
                                                          sizeof(float), TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Load Quad indices
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(m_quadIndices.data(),
                                                        static_cast<uint16_t>(m_quadIndices.size()) *
                                                        sizeof(uint16_t), TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Shader
	std::vector<TRAP::Graphics::Shader::Macro> macros
	{
		{"HALF", "0.5f"},
		{"COLOR", "vec3(1.0f, 0.0f, 0.0f)"}
	};

    // TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/testspirv.shader", &macros);
    TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/testspirv.spirv", &macros);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnDetach()
{
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnUpdate(const TRAP::Utils::TimeStep& /*deltaTime*/)
{
    m_vertexBuffer->Use();
    m_indexBuffer->Use();

    //Use shader
    TRAP::Graphics::ShaderManager::Get("Test")->Use();

    //Render Quad
    TRAP::Graphics::RenderCommand::DrawIndexed(m_indexBuffer->GetCount());

    //Update FPS & FrameTime history
    if (m_titleTimer.Elapsed() >= 0.025f)
    {
        m_titleTimer.Reset();
        static int frameTimeIndex = 0;
        if (frameTimeIndex < static_cast<int>(m_frameTimeHistory.size() - 1))
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::Renderer::GetFrameTime();
            frameTimeIndex++;
        }
        else
        {
            std::move(m_frameTimeHistory.begin() + 1, m_frameTimeHistory.end(), m_frameTimeHistory.begin());
            m_frameTimeHistory[m_frameTimeHistory.size() - 1] = TRAP::Graphics::Renderer::GetFrameTime();
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnImGuiRender()
{
	ImGui::Begin("SPIRV", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                            ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Press ESC to close");
    ImGui::Separator();
    ImGui::Text("CPU: %ix %s", TRAP::Utils::GetCPUInfo().LogicalCores, TRAP::Utils::GetCPUInfo().Model.c_str());
    ImGui::Text("GPU: %s", TRAP::Graphics::RendererAPI::GetRenderer()->GetCurrentGPUName().c_str());
    ImGui::Text("FPS: %u", TRAP::Graphics::Renderer::GetFPS());
    ImGui::Text("FrameTime: %.3fms", TRAP::Graphics::Renderer::GetFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), static_cast<int>(m_frameTimeHistory.size()), 0, nullptr, 0,
                     33, ImVec2(200, 50));
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e)
	{
		return OnKeyPress(e);
	});
}

//-------------------------------------------------------------------------------------------------------------------//

bool SPIRVTests::OnKeyPress(TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}