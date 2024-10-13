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

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);
	constexpr bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::Graphics::OrthographicCamera m_camera{-TRAP::Application::GetWindow()->GetAspectRatio(),
	                                            TRAP::Application::GetWindow()->GetAspectRatio(),
	                                            -1.0f, 1.0f, -1.0f, 1.0f};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void AntiAliasingTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&AntiAliasingTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(std::bind_front(&AntiAliasingTests::OnFrameBufferResize, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool AntiAliasingTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetProjection(-e.GetAspectRatio(),
		                   e.GetAspectRatio(),
		                   -1.0f, 1.0f, -1.0f, 1.0f);

	return false;
}

#endif /*GAMESTRAP_ANTIALIASINGTESTS_H*/
