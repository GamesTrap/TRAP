#include "SPIRVTests.h"

SPIRVTests::SPIRVTests()
	: Layer("SPIRV")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("SPIRV");

	//Load Quad vertices
    m_vertexBuffer = TRAP::Graphics::VertexBuffer::Create(QuadVerticesIndexed,
                                                          TRAP::Graphics::UpdateFrequency::Static);
    const TRAP::Graphics::VertexBufferLayout layout =
    {
        { TRAP::Graphics::ShaderDataType::Float3, "Pos" },
        { TRAP::Graphics::ShaderDataType::Float2, "UV" }
    };
    m_vertexBuffer->SetLayout(layout);
    m_vertexBuffer->AwaitLoading();

    //Load Quad indices
    m_indexBuffer = TRAP::Graphics::IndexBuffer::Create(QuadIndices,
                                                        TRAP::Graphics::UpdateFrequency::Static);
    m_indexBuffer->AwaitLoading();

    //Load Shader
	const std::vector<TRAP::Graphics::Shader::Macro> macros
	{
		{"HALF", "0.5f"},
		{"COLOR", "vec3(1.0f, 0.0f, 0.0f)"}
	};

    // TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/testspirv.shader", macros);
    TRAP::Graphics::ShaderManager::LoadFile("Test", "./Assets/Shaders/testspirv.tp-spv", macros);

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
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
        constinit static usize frameTimeIndex = 0;
        if (frameTimeIndex < m_frameTimeHistory.size() - 1)
        {
            m_frameTimeHistory[frameTimeIndex] = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
            frameTimeIndex++;
        }
        else
        {
            std::shift_left(m_frameTimeHistory.begin(), m_frameTimeHistory.end(), 1);
            m_frameTimeHistory.back() = TRAP::Graphics::RenderCommand::GetCPUFrameTime();
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
	ImGui::Text("GPU: %s", TRAP::Graphics::RenderCommand::GetGPUName().c_str());
    ImGui::Text("CPU FPS: %u", TRAP::Graphics::RenderCommand::GetCPUFPS());
    ImGui::Text("GPU FPS: %u", TRAP::Graphics::RenderCommand::GetGPUFPS());
    ImGui::Text("CPU FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetCPUFrameTime());
    ImGui::Text("GPU Graphics FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUGraphicsFrameTime());
    ImGui::Text("GPU Compute FrameTime: %.3fms", TRAP::Graphics::RenderCommand::GetGPUComputeFrameTime());
    ImGui::PlotLines("", m_frameTimeHistory.data(), NumericCast<i32>(m_frameTimeHistory.size()), 0, nullptr, 0,
                     33, ImVec2(200, 50));
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void SPIRVTests::OnEvent(TRAP::Events::Event& event)
{
	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&SPIRVTests::OnKeyPress, this));
}

//-------------------------------------------------------------------------------------------------------------------//

bool SPIRVTests::OnKeyPress(const TRAP::Events::KeyPressEvent& e)
{
	if (e.GetKey() == TRAP::Input::Key::Escape)
		TRAP::Application::Shutdown();

	return false;
}