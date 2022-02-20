#include "HeadlessTests.h"

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
	auto win = TRAP::Application::GetWindow().get();

	TRAP::Application::Shutdown();
}