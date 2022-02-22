#include "HeadlessTests.h"

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
	TRAP::Application::Shutdown();
}