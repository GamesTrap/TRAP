#include "HeadlessTests.h"

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
	//TODO Test what happens when no RenderAPI was selected / no gpu found
	TRAP::Application::Shutdown();
}