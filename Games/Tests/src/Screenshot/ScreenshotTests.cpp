#include "ScreenshotTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

ScreenshotTests::ScreenshotTests()
	: Layer("Screenshot"), m_cameraController(static_cast<float>(TRAP::Application::GetWindow()->GetFrameBufferSize().x) /
	 										  static_cast<float>(TRAP::Application::GetWindow()->GetFrameBufferSize().y), true)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void ScreenshotTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Screenshot");
}

//-------------------------------------------------------------------------------------------------------------------//

void ScreenshotTests::OnImGuiRender()
{
	ImGui::ShowDemoWindow();
}

//-------------------------------------------------------------------------------------------------------------------//

void ScreenshotTests::OnUpdate(const TRAP::Utils::TimeStep& deltaTime)
{
	//Render
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color_Depth);

	TRAP::Graphics::Renderer2D::ResetStats();
	TRAP::Graphics::Renderer2D::BeginScene(m_cameraController.GetCamera());
	{
		TRAP::Graphics::Renderer2D::DrawQuad({ {-0.5f, -0.5f, 0.0f}, {}, {0.8f, 0.8f, 1.0f} }, { 1.0f, 0.0f, 0.0f, 1.0f });
		TRAP::Graphics::Renderer2D::DrawQuad({ {0.5f, 0.5f, 0.0f}, {}, {0.5f, 0.75f, 1.0f} }, { 0.0f, 1.0f, 0.0f, 1.0f });
	}
	TRAP::Graphics::Renderer2D::EndScene();

	static float time = 0.0f;
	static bool done = false;
	time += deltaTime.GetSeconds();
	if(time >= 3.0f && !done)
	{
		TRAP::Scope<TRAP::Image> renderedImg = TRAP::Graphics::RendererAPI::GetRenderer()->CaptureScreenshot(TRAP::Application::GetWindow());
		TRAP::INTERNAL::PPMImage::Save(renderedImg.get(), "output.ppm");
		done = true;
	}
}