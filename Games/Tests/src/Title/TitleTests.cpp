#include "TitleTests.h"

TitleTests::TitleTests()
	: Layer("Title")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void TitleTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("English 日本語 русский язык 官話");
}

//-------------------------------------------------------------------------------------------------------------------//

void TitleTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::ClearBuffer::Color);
}