#ifndef GAMESTRAP_CONTROLLERTESTS_H
#define GAMESTRAP_CONTROLLERTESTS_H

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	constexpr ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event);
	static bool OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event);
	static bool OnWindowDrop(const TRAP::Events::WindowDropEvent& event);

	static constexpr std::string GetDPadDirection(const TRAP::Input::ControllerDPad& dpad);
	static std::string GetBatteryStatus(TRAP::Input::Controller controller);

	inline static std::vector<TRAP::Input::Controller> s_controllers{};
	inline constinit static bool s_dpadButtons = false;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr ControllerTests::ControllerTests()
	: Layer("Controllers")
{
}

//-------------------------------------------------------------------------------------------------------------------//

inline constexpr std::string ControllerTests::GetDPadDirection(const TRAP::Input::ControllerDPad& dpad)
{
	switch (dpad)
	{
	case TRAP::Input::ControllerDPad::Centered:
		return "Centered";

	case TRAP::Input::ControllerDPad::Up:
		return "Up";

	case TRAP::Input::ControllerDPad::Right:
		return "Right";

	case TRAP::Input::ControllerDPad::Down:
		return "Down";

	case TRAP::Input::ControllerDPad::Left:
		return "Left";

	case TRAP::Input::ControllerDPad::Right_Up:
		return "Right Up";

	case TRAP::Input::ControllerDPad::Right_Down:
		return "Right Down";

	case TRAP::Input::ControllerDPad::Left_Up:
		return "Left Up";

	case TRAP::Input::ControllerDPad::Left_Down:
		return "Left Down";

	default:
		return "";
	}
}

#endif /*GAMESTRAP_CONTROLLERTESTS_H*/