#ifndef _GAMESTRAP_CONTROLLERTESTS_H_
#define _GAMESTRAP_CONTROLLERTESTS_H_

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Event& event) override;

private:
	bool OnControllerConnect(const TRAP::ControllerConnectEvent& event);
	bool OnControllerDisconnect(const TRAP::ControllerDisconnectEvent& event);
	bool OnWindowDrop(const TRAP::WindowDropEvent& event);

	static std::string GetDPadDirection(const TRAP::Input::ControllerDPad& dpad);
	
	static std::vector<TRAP::Input::Controller> s_controllers;
	static bool s_dpadButtons;
};

#endif /*_GAMESTRAP_CONTROLLERTESTS_H_*/