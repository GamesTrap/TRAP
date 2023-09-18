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

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& e);
	bool OnFrameBufferResize(const TRAP::Events::FrameBufferResizeEvent& e);

	TRAP::Utils::Timer m_fpsTimer{};

	TRAP::SceneCamera m_camera{};

	uint32_t m_maxAnisotropyLevelIdx = 0;
	uint32_t m_currAnisotropyLevelIdx = 0;
};

#endif /*GAMESTRAP_ANISOTROPYESTS_H*/