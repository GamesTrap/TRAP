#ifndef GAMESTRAP_RENDERSCALETESTS_H
#define GAMESTRAP_RENDERSCALETESTS_H

#include <TRAP.h>

class RenderScaleTests final : public TRAP::Layer
{
public:
	RenderScaleTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

	TRAP::Graphics::OrthographicCameraController m_cameraController{TRAP::Application::GetWindow()->GetAspectRatio(), true};
};

#endif /*GAMESTRAP_RENDERSCALETESTS_H*/
