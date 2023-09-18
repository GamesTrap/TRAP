#ifndef GAMESTRAP_WINDOWFEATURESTESTS_H
#define GAMESTRAP_WINDOWFEATURESTESTS_H

#include <TRAP.h>

class WindowFeaturesTests final : public TRAP::Layer
{
public:
	constexpr WindowFeaturesTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

private:
	TRAP::Math::Vec2i m_lastWindowPos{};
	TRAP::Math::Vec2ui m_lastWindowSize{};
	TRAP::Math::Vec2ui m_minWindowSize{400, 400};
	TRAP::Math::Vec2ui m_maxWindowSize{400, 400};
	TRAP::Math::Vec2i m_aspect{1, 1};

	bool m_limitMinSize = false;
	bool m_limitMaxSize = false;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr WindowFeaturesTests::WindowFeaturesTests()
	: Layer("WindowFeatures")
{
}

#endif /*GAMESTRAP_WINDOWFEATURESTESTS_H*/