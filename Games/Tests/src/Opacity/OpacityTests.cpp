#include "OpacityTests.h"

OpacityTests::OpacityTests()
	: Layer("Opacity")
{
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnImGuiRender()
{
	ImGui::Begin("Opacity", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs);
	if(ImGui::SliderFloat("Opacity", &m_opacity, 0.0f, 1.0f))
		TRAP::Application::GetWindow()->SetOpacity(m_opacity);
	ImGui::End();
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnAttach()
{
	TRAP::Application::GetWindow()->SetTitle("Opacity");
}

//-------------------------------------------------------------------------------------------------------------------//

void OpacityTests::OnUpdate(const TRAP::Utils::TimeStep&)
{
	//Render
	TRAP::Graphics::RenderCommand::SetClearColor();
	TRAP::Graphics::RenderCommand::Clear(TRAP::Graphics::RendererBufferType::Color);
}