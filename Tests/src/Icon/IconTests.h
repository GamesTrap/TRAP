#ifndef _GAMESTRAP_ICONTESTS_H_
#define _GAMESTRAP_ICONTESTS_H_

#include <TRAP.h>

class IconTests final : public TRAP::Layer
{
public:
	IconTests();

	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static void SetIcon(const TRAP::Scope<TRAP::Window>& window, int32_t iconColor);
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);
	
	static std::array<std::string, 16> s_iconStrings;
	static std::array<std::array<uint8_t, 4>, 5> s_iconColors;
	static int32_t s_cursorIconColor;
};

#endif /*_GAMESTRAP_ICONTESTS_H_*/