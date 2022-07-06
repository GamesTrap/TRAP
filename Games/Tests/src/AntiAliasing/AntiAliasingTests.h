#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	AntiAliasingTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& e);
	bool OnFrameBufferResize(TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer;

	bool m_msaa;

	TRAP::Graphics::OrthographicCamera m_camera;
};

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/