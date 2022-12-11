#include "RenderScaleTests.h"
#include "Application.h"
#include <imgui.h>

RenderScaleTests::RenderScaleTests()
	: Layer("Render Scaling"),
	m_cameraController(TRAP::Application::GetWindow()->GetAspectRatio(), true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void RenderScaleTests::OnImGuiRender()
{
	ImGui::Begin("Render Scaling", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
	                                                     ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Press ESC to close");
    ImGui::Separator();

    float renderScale = TRAP::Graphics::RenderCommand::GetRenderScale();
    if(ImGui::SliderFloat("Render Scale", &renderScale, 0.5f, 2.0f, "%.2f", ImGuiSliderFlags_NoInput))
        TRAP::Graphics::RenderCommand::SetRenderScale(renderScale);

    const auto internalRes = TRAP::Graphics::RendererAPI::GetInternalRenderResolution(TRAP::Application::GetWindow());
    ImGui::Text("Render Resolution: %ux%u", internalRes.x, internalRes.y);

    const auto outputSize = TRAP::Application::GetWindow()->GetFrameBufferSize();
    ImGui::Text("Output Resolution: %ux%u", outputSize.x, outputSize.y);

    ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void RenderScaleTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Render Scaling");

	//Load Textures
    TRAP::Graphics::TextureManager::Load("TRAP", "./Assets/Textures/TRAPWhiteLogo2048x2048.png")->AwaitLoading();

    //Wait for all pending resources (Just in case)
    TRAP::Graphics::RendererAPI::GetResourceLoader()->WaitForAllResourceLoads();

	TRAP::Graphics::RenderCommand::SetDepthTesting(true);
	TRAP::Graphics::RenderCommand::SetBlendConstant(TRAP::Graphics::BlendConstant::SrcAlpha, TRAP::Graphics::BlendConstant::OneMinusSrcAlpha);
}

//-------------------------------------------------------------------------------------------------------------------//

void RenderScaleTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	//Update
	m_cameraController.OnUpdate(deltaTime);

	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

    TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
    TRAP::Graphics::Renderer2D::DrawQuad({ {}, {0.0f, 0.0f, TRAP::Application::GetTime() * -50.0f }, {2.0f, 2.0f, 1.0f} },
                                         { 0.2f, 0.8f, 0.3f, 1.0f }, TRAP::Graphics::TextureManager::Get2D("TRAP"));
	TRAP::Graphics::Renderer2D::EndScene();
}

//-------------------------------------------------------------------------------------------------------------------//

void RenderScaleTests::OnEvent(TRAP::Events::Event& event)
{
	m_cameraController.OnEvent(event);

	TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>([this](TRAP::Events::KeyPressEvent& e) { return OnKeyPress(e); });
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>([this](TRAP::Events::FrameBufferResizeEvent& e) { return OnFrameBufferResize(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool RenderScaleTests::OnKeyPress(TRAP::Events::KeyPressEvent& event)
{
	if (event.GetKey() == TRAP::Input::Key::Escape && event.GetRepeatCount() < 1)
        TRAP::Application::Shutdown();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool RenderScaleTests::OnFrameBufferResize([[maybe_unused]] TRAP::Events::FrameBufferResizeEvent& event)
{
    return false;
}
