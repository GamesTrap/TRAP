#ifndef GAMESTRAP_MONITORTESTS_H
#define GAMESTRAP_MONITORTESTS_H

#include <TRAP.h>

class MonitorTests final : public TRAP::Layer
{
public:
	constexpr MonitorTests();

	void OnAttach() override;
	void OnImGuiRender() override;

	constexpr void OnEvent(TRAP::Events::Event& event) override;

private:
	static constexpr bool OnMonitorConnect(const TRAP::Events::MonitorConnectEvent& event);
	static constexpr bool OnMonitorDisconnect(const TRAP::Events::MonitorDisconnectEvent& event);
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr MonitorTests::MonitorTests()
	: Layer("Monitor")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void MonitorTests::OnEvent(TRAP::Events::Event& event)
{
	const TRAP::Events::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<TRAP::Events::MonitorConnectEvent>(OnMonitorConnect);
	dispatcher.Dispatch<TRAP::Events::MonitorDisconnectEvent>(OnMonitorDisconnect);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool MonitorTests::OnMonitorConnect(const TRAP::Events::MonitorConnectEvent& event)
{
	TP_TRACE(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr bool MonitorTests::OnMonitorDisconnect(const TRAP::Events::MonitorDisconnectEvent& event)
{
	TP_TRACE(event);

	return true;
}

#endif /*GAMESTRAP_MONITORTESTS_H*/
