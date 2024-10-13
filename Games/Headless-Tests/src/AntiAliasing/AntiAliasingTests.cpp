#include "AntiAliasingTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

namespace
{
	constexpr TRAP::Math::Vec2ui32 resolution(2560u, 1440u);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnAttach()
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	TRAP::Graphics::RenderCommand::SetResolution(resolution.x(), resolution.y());
	TRAP::Graphics::RenderCommand::SetAntiAliasing(TRAP::Graphics::AntiAliasing::MSAA, TRAP::Graphics::SampleCount::Four);
}

//-------------------------------------------------------------------------------------------------------------------//

void AntiAliasingTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	if(TRAP::Graphics::RendererAPI::GetRenderAPI() == TRAP::Graphics::RenderAPI::NONE)
		return;

	constinit static u32 frames = 0u;
    if(frames == 3u)
    {
        //Screenshot
	    const TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
	    	TRAP::INTERNAL::PPMImage::Save(*testImage, "antialiasing.ppm");

        TRAP::Application::Shutdown();
        return;
    }
	++frames;

	static constexpr f32 angle = 22.8f;

	static constexpr f32 aspectRatio = NumericCast<f32>(resolution.x()) / resolution.y();
	static constexpr TRAP::Graphics::OrthographicCamera camera{-(aspectRatio), aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f};

	TRAP::Graphics::Renderer2D::BeginScene(camera);
	TRAP::Graphics::Renderer2D::DrawQuad({ {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, angle}, {1.0f, 1.0f, 1.0f} }, {1.0f, 1.0f, 1.0f, 1.0f});
	TRAP::Graphics::Renderer2D::EndScene();
}
