#ifndef _GAMESTRAP_SANDBOX2D_H_
#define _GAMESTRAP_SANDBOX2D_H_

#include <TRAP.h>

class Sandbox2D final : public TRAP::Layer
{
public:
	Sandbox2D();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Event& event) override;

private:
	TRAP::Graphics::OrthographicCameraController m_cameraController;

	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_updateFPSTimer;
};

#endif /*_GAMESTRAP_SANDBOX2D_H_*/