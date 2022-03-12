#include "HeadlessTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
	TRAP::Graphics::RenderCommand::SetResolution(3840, 2160); //4K Resolution

	TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
	TRAP::INTERNAL::PPMImage::Save(testImage, "testBefore.ppm");
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnUpdate(const TRAP::Utils::TimeStep& /*deltaTime*/)
{
	//Give engine time to update resolution
	static int count = 0;
	if(++count > 1)
	{
		TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		TRAP::INTERNAL::PPMImage::Save(testImage, "testAfter.ppm");
		TRAP::Application::Shutdown();
	}
}