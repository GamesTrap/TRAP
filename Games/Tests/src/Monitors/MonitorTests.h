#ifndef GAMESTRAP_MONITORTESTS_H
#define GAMESTRAP_MONITORTESTS_H

#include <TRAP.h>

class MonitorTests final : public TRAP::Layer
{
public:
	MonitorTests();

	void OnAttach() override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	static bool OnMonitorConnect(const TRAP::Events::MonitorConnectEvent& event);
	static bool OnMonitorDisconnect(const TRAP::Events::MonitorDisconnectEvent& event);

	static int32_t Euclid(int32_t a, int32_t b);
	static std::string FormatMode(const TRAP::Monitor::VideoMode& mode);
};

#endif /*GAMESTRAP_MONITORTESTS_H*/