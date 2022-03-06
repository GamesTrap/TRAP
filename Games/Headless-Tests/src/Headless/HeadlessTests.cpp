#include "HeadlessTests.h"

#include <ImageLoader/PortableMaps/PPMImage.h>

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
	// const std::vector<uint8_t> pixelData{255, 0, 0,    0, 255, 0,    0, 0, 255,    255, 255, 0,    0, 255, 255,    255, 0, 255,    255, 255, 255,    0, 0, 0};
	// TRAP::Scope<TRAP::Image> testImage = TRAP::Image::LoadFromMemory(2, 4, TRAP::Image::ColorFormat::RGB, pixelData);
	// TRAP::INTERNAL::PPMImage::Save(testImage, "test.ppm");

	TRAP::Application::Shutdown();
}