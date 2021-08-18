#ifndef _GAMESTRAP_CONTROLLERTESTS_H_
#define _GAMESTRAP_CONTROLLERTESTS_H_

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event);
	bool OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event);
	bool OnWindowDrop(const TRAP::Events::WindowDropEvent& event);

	static std::string GetDPadDirection(const TRAP::Input::ControllerDPad& dpad);
	static std::string GetBatteryStatus(TRAP::Input::Controller controller);

	static std::vector<TRAP::Input::Controller> s_controllers;
	static bool s_dpadButtons;
};

#endif /*_GAMESTRAP_CONTROLLERTESTS_H_*/