#include "HeadlessTests.h"

HeadlessTests::HeadlessTests()
	: Layer("Headless")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void HeadlessTests::OnAttach()
{
<<<<<<< HEAD
	auto win = TRAP::Application::GetWindow().get();

	TRAP::Application::Shutdown();
=======
	//TODO Test what happens when no RenderAPI was selected / no gpu found
	//TRAP::Application::Shutdown();
>>>>>>> ebe28fefb8e8bf89bf60c4f394eb7ef2d139c305
}