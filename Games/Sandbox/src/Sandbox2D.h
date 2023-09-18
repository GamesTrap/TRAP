#ifndef GAMESTRAP_SANDBOX2D_H
#define GAMESTRAP_SANDBOX2D_H

#include <TRAP.h>

class Sandbox2D final : public TRAP::Layer
{
public:
	Sandbox2D();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(const TRAP::Events::KeyPressEvent& event);

	bool m_wireFrame = false;

	TRAP::Graphics::OrthographicCameraController m_cameraController{TRAP::Application::GetWindow()->GetAspectRatio(), true};

	TRAP::Ref<TRAP::Graphics::Texture> m_spriteSheet = nullptr;

	std::array<float, 50> m_frameTimeHistory{};
	TRAP::Utils::Timer m_updateFPSTimer{};
};

#endif /*GAMESTRAP_SANDBOX2D_H*/