#include "HeadlessTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

void HeadlessTests::OnAttach()
{
	TRAP::Graphics::RenderCommand::SetResolution(3840, 2160); //4K Resolution

	// TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
	// if(testImage)
	//     TRAP::INTERNAL::PPMImage::Save(testImage.get(), "testBefore.ppm");

	// TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	//Give engine time to update resolution
	constinit static i32 count = 0;
	if(++count > 3)
	{
		TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
			TRAP::INTERNAL::PPMImage::Save(testImage.get(), "testAfter.ppm");
		TRAP::Application::Shutdown();
	}
}