#ifndef GAMESTRAP_CONTROLLERTESTS_H
#define GAMESTRAP_CONTROLLERTESTS_H

#include <TRAP.h>

class ControllerTests final : public TRAP::Layer
{
public:
	constexpr ControllerTests();

	void OnAttach() override;
	void OnImGuiRender() override;
	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static constexpr bool OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event);
	static constexpr bool OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event);
	static bool OnWindowDrop(const TRAP::Events::WindowDropEvent& event);

	inline static std::vector<TRAP::Input::Controller> s_controllers{};
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr ControllerTests::ControllerTests()
	: Layer("Controllers")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void ControllerTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::ControllerConnectEvent>(OnControllerConnect);
	dispatcher.Dispatch<TRAP::Events::ControllerDisconnectEvent>(OnControllerDisconnect);
	dispatcher.Dispatch<TRAP::Events::WindowDropEvent>(OnWindowDrop);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool ControllerTests::OnControllerConnect(const TRAP::Events::ControllerConnectEvent& event)
{
	s_controllers.push_back(event.GetController());

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool ControllerTests::OnControllerDisconnect(const TRAP::Events::ControllerDisconnectEvent& event)
{
	std::erase(s_controllers, event.GetController());

	return true;
}

#endif /*GAMESTRAP_CONTROLLERTESTS_H*/
