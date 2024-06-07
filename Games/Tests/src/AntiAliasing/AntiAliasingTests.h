#ifndef GAMESTRAP_ANTIALIASINGTESTS_H
#define GAMESTRAP_ANTIALIASINGTESTS_H

#include <TRAP.h>

class AntiAliasingTests final : public TRAP::Layer
{
public:
	AntiAliasingTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);
	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Graphics::AntiAliasing m_antiAliasing = TRAP::Graphics::AntiAliasing::Off;
	TRAP::Graphics::SampleCount m_sampleCount = TRAP::Graphics::SampleCount::One;

	TRAP::Graphics::OrthographicCamera m_camera{-TRAP::Application::GetWindow()->GetAspectRatio(),
	                                            TRAP::Application::GetWindow()->GetAspectRatio(),
	                                            -1.0f, 1.0f, -1.0f, 1.0f};

	std::vector<TRAP::Graphics::AntiAliasing> m_antiAliasingMethods{};
};

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/
