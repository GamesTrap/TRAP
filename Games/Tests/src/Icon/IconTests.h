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
	static void SetIcon(const TRAP::Window& window, uint32_t iconColor);
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

	inline constinit static uint32_t s_cursorIconColor = 0;
};

#endif /*GAMESTRAP_ICONTESTS_H*/