#ifndef GAMESTRAP_ICONTESTS_H
#define GAMESTRAP_ICONTESTS_H

#include <TRAP.h>

class IconTests final : public TRAP::Layer
{
public:
	IconTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static void SetIcon(const TRAP::Scope<TRAP::Window>& window, int32_t iconColor);
	static bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

	static int32_t s_cursorIconColor;
};

#endif /*GAMESTRAP_ICONTESTS_H*/