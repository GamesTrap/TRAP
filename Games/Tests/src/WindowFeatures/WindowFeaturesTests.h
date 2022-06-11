#ifndef GAMESTRAP_WINDOWFEATURESTESTS_H
#define GAMESTRAP_WINDOWFEATURESTESTS_H

#include <TRAP.h>

class WindowFeaturesTests final : public TRAP::Layer
{
public:
	WindowFeaturesTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

private:
	TRAP::Math::Vec2i m_lastWindowPos;
	TRAP::Math::Vec2ui m_lastWindowSize;
	TRAP::Math::Vec2ui m_minWindowSize;
	TRAP::Math::Vec2ui m_maxWindowSize;
	TRAP::Math::Vec2i m_aspect;

	bool m_fullscreen;
	bool m_limitAspectRatio;
	bool m_limitMinSize;
	bool m_limitMaxSize;
};

#endif /*GAMESTRAP_WINDOWFEATURESTESTS_H*/