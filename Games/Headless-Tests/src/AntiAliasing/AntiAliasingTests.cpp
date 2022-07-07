#include "AntiAliasingTests.h"

#include <Graphics/API/Objects/Queue.h>
#include <ImageLoader/PortableMaps/PPMImage.h>

AntiAliasingTests::AntiAliasingTests()
	: Layer("AntiAliasing"),
	  m_camera(-(2560.0f / 1440.0f), 2560.0f / 1440.0f, -1.0f, 1.0f, -1.0f, 1.0f)
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	TRAP::Graphics::RenderCommand::SetResolution(2560, 1440);
	TRAP::Graphics::RenderCommand::SetAntiAliasing(TRAP::Graphics::AntiAliasing::MSAA, TRAP::Graphics::SampleCount::Four);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnDetach()
{
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate(const TRAP::Utils::TimeStep& /*deltaTime*/)
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	static uint32_t frames = 0;
    if(frames == 3)
    {
        //Screenshot
	    TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
	    TRAP::INTERNAL::PPMImage::Save(testImage.get(), "antialiasing.ppm");

        TRAP::Application::Shutdown();
        return;
    }
	++frames;

	const float angle = 22.8f;

	TRAP::Graphics::Renderer2D::BeginScene(m_camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();
}
