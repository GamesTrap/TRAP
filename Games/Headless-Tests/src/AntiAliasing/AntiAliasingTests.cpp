#include "AntiAliasingTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

void AntiAliasingTests::OnAttach()
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	TRAP::Graphics::RenderCommand::SetResolution(2560, 1440);
	TRAP::Graphics::RenderCommand::SetAntiAliasing(TRAP::Graphics::AntiAliasing::MSAA, TRAP::Graphics::SampleCount::Four);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	constinit static u32 frames = 0;
    if(frames == 3)
    {
        //Screenshot
	    TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
	    	TRAP::INTERNAL::PPMImage::Save(testImage.get(), "antialiasing.ppm");

        TRAP::Application::Shutdown();
        return;
    }
	++frames;

	const f32 angle = 22.8f;

	TRAP::Graphics::Renderer2D::BeginScene(s_camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();
}
