#ifndef GAMESTRAP_WINDOWFEATURESTESTS_H
#define GAMESTRAP_WINDOWFEATURESTESTS_H

#include <TRAP.h>

class WindowFeaturesTests final : public TRAP::Layer
{
public:
	constexpr WindowFeaturesTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

private:
	TRAP::Math::Vec2i m_lastWindowPos{};
	TRAP::Math::Vec2ui m_lastWindowSize{};
	TRAP::Math::Vec2ui m_minWindowSize{400u, 400u};
	TRAP::Math::Vec2ui m_maxWindowSize{400u, 400u};
	TRAP::Math::Vec2i m_aspect{1, 1};

	bool m_limitMinSize = false;
	bool m_limitMaxSize = false;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr WindowFeaturesTests::WindowFeaturesTests()
	: Layer("WindowFeatures")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void WindowFeaturesTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::KeyPressEvent>(std::bind_front(&WindowFeaturesTests::OnKeyPress, this));
}

#endif /*GAMESTRAP_WINDOWFEATURESTESTS_H*/
