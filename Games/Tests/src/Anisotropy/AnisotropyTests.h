#ifndef GAMESTRAP_ANISOTROPYESTS_H
#define GAMESTRAP_ANISOTROPYESTS_H

#include <TRAP.h>

class AnisotropyTests final : public TRAP::Layer
{
public:
	AnisotropyTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnImGuiRender() override;

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e) const;
	constexpr bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::SceneCamera m_camera{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr void AnisotropyTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&AnisotropyTests::OnKeyPress, this));
	dispatcher.Dispatch<TRAP::Events::FrameBufferResizeEvent>(std::bind_front(&AnisotropyTests::OnFrameBufferResize, this));
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool AnisotropyTests::OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e)
{
	m_camera.SetViewportSize(e.GetWidth(), e.GetHeight());

	return false;
}

#endif /*GAMESTRAP_ANISOTROPYESTS_H*/
