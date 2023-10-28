#ifndef GAMESTRAP_ICONTESTS_H
#define GAMESTRAP_ICONTESTS_H

#include <TRAP.h>

class IconTests final : public TRAP::Layer
{
public:
	constexpr IconTests();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static void SetIcon(const TRAP::Window& window, u32 iconColor);
	static bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

	inline constinit static u32 s_cursorIconColor = 0;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr IconTests::IconTests()
	: Layer("Icon")
{
}

#endif /*GAMESTRAP_ICONTESTS_H*/