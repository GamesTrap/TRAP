#ifndef GAMESTRAP_INPUTLAGTESTS_H
#define GAMESTRAP_INPUTLAGTESTS_H

#include <TRAP.h>

class InputLagTests final : public TRAP::Layer
{
public:
	InputLagTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

	enum class CursorMethod
	{
		SyncQuery,
		InputMessage
	};

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	bool OnMouseMove(TRAP::Events::MouseMoveEvent& event);

	void SampleInput();
	void DrawMarker(int32_t lead, TRAP::Math::Vec2 pos);

	CursorMethod m_cursorMethod;
	bool m_vsync;
	TRAP::Math::Vec2 m_cursorNew;
	TRAP::Math::Vec2 m_cursorPos;
	TRAP::Math::Vec2 m_cursorVelocity;
	bool m_showForecasts;
};

#endif /*GAMESTRAP_INPUTLAGTESTS_H*/