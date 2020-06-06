#ifndef _GAMESTRAP_VULKANTESTS_H_
#define _GAMESTRAP_VULKANTESTS_H_

#include <TRAP.h>

class VulkanTests final : public TRAP::Layer
{
public:
	VulkanTests();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;

	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnWindowClose(TRAP::Events::WindowCloseEvent& e);
	
	TRAP::Scope<TRAP::Window> m_window;

	TRAP::Utils::Timer m_fpsTimer;
};

#endif /*_GAMESTRAP_VULKANTESTS_H_*/