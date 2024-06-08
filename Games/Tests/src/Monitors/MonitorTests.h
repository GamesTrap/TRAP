#ifndef GAMESTRAP_MONITORTESTS_H
#define GAMESTRAP_MONITORTESTS_H

#include <TRAP.h>

class MonitorTests final : public TRAP::Layer
{
public:
	constexpr MonitorTests();

	void OnAttach() override;
	void OnImGuiRender() override;

	void OnEvent(TRAP::Events::Event& event) override;
};

//-------------------------------------------------------------------------------------------------------------------//

constexpr MonitorTests::MonitorTests()
	: Layer("Monitor")
{
}

#endif /*GAMESTRAP_MONITORTESTS_H*/
