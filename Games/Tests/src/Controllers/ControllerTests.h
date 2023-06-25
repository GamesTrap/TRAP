#ifndef GAMESTRAP_CONTROLLERTESTS_H
#define GAMESTRAP_CONTROLLERTESTS_H

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event);
	static bool OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event);
	static bool OnWindowDrop(const TRAP::Events::WindowDropEvent& event);

	static std::string GetDPadDirection(const TRAP::Input::ControllerDPad& dpad);
	static std::string GetBatteryStatus(TRAP::Input::Controller controller);

	inline constinit static std::vector<TRAP::Input::Controller> s_controllers{};
	inline constinit static bool s_dpadButtons = false;
};

#endif /*GAMESTRAP_CONTROLLERTESTS_H*/