#ifndef GAMESTRAP_SANDBOX2D_H
#define GAMESTRAP_SANDBOX2D_H

#include "Graphics/Textures/SubTexture2D.h"
#include <TRAP.h>

class Sandbox2D final : public TRAP::Layer
{
public:
	Sandbox2D();

	void OnImGuiRender() override;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(const TRAP::Utils::TimeStep& deltaTime) override;
	void OnEvent(TRAP::Events::Event& event) override;

private:
	bool OnKeyPress(TRAP::Events::KeyPressEvent& event);

	bool m_wireFrame;

	TRAP::Graphics::OrthographicCameraController m_cameraController;

	TRAP::Graphics::Texture* m_spriteSheet;
	TRAP::Ref<TRAP::Graphics::SubTexture2D> m_aButtonTexture;
	TRAP::Ref<TRAP::Graphics::SubTexture2D> m_bButtonTexture;
	TRAP::Ref<TRAP::Graphics::SubTexture2D> m_enterKeyTexture;
	TRAP::Ref<TRAP::Graphics::SubTexture2D> m_circleTexture;

	std::array<float, 50> m_frameTimeHistory;
	TRAP::Utils::Timer m_updateFPSTimer;
};

#endif /*GAMESTRAP_SANDBOX2D_H*/