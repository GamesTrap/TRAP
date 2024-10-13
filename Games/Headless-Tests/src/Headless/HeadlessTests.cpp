#include "HeadlessTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

void HeadlessTests::OnAttach()
{
	TRAP::Graphics::RenderCommand::SetResolution(3840u, 2160u); //4K Resolution

	// const TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
	// if(testImage)
	//     TRAP::INTERNAL::PPMImage::Save(*testImage, "testBefore.ppm");

	// TRAP::Application::Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnUpdate([[maybe_unused]] const TRAP::Utils::TimeStep& deltaTime)
{
	//Give engine time to update resolution
	constinit static i32 count = 0;
	if(++count > 3)
	{
		const TRAP::Scope<TRAP::Image> testImage = TRAP::Graphics::RenderCommand::CaptureScreenshot();
		if(testImage)
			TRAP::INTERNAL::PPMImage::Save(*testImage, "testAfter.ppm");
		TRAP::Application::Shutdown();
	}
}
